/*******************************************************************************
 * Copyright (c) 2015 Matthijs Kooijman
 * Copyright (c) 2018-2019 MCCI Corporation
 * Copyright (C) 2021 Daniel Friesel
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which is available at http://www.eclipse.org/legal/epl-v10.html
 *
 * This the HAL to run LMIC on top of multipass, adapted from the HAL to
 * run LMIC on top of the Arduino environment.
 *******************************************************************************/

#include "arch.h"
#include "driver/gpio.h"
#include "driver/spi.h"
#include <avr/interrupt.h>
// include all the lmic header files, including ../lmic/hal.h
#include "../lmic.h"
// include the C++ hal.h
#include "hal.h"
// we may need some things from stdio.
#include <stdio.h>

// -----------------------------------------------------------------------------
// I/O

static const Multipass_LMIC::HalPinmap_t *plmic_pins;
static Multipass_LMIC::HalConfiguration_t *pHalConfig;
static Multipass_LMIC::HalConfiguration_t nullHalConig;
static hal_failure_handler_t* custom_hal_failure_handler = NULL;

// 16bit hardware counter + 16bit overflow counter == 32bit tick counter
static volatile uint16_t timer3_overflow = 0;

static void hal_interrupt_init(); // Fwd declaration

static void hal_io_init () {
    // NSS and DIO0 are required, DIO1 is required for LoRa, DIO2 for FSK
    ASSERT(plmic_pins->nss != LMIC_UNUSED_PIN);
    ASSERT(plmic_pins->dio[0] != LMIC_UNUSED_PIN);
    ASSERT(plmic_pins->dio[1] != LMIC_UNUSED_PIN || plmic_pins->dio[2] != LMIC_UNUSED_PIN);

//    Serial.print("nss: "); Serial.println(plmic_pins->nss);
//    Serial.print("rst: "); Serial.println(plmic_pins->rst);
//    Serial.print("dio[0]: "); Serial.println(plmic_pins->dio[0]);
//    Serial.print("dio[1]: "); Serial.println(plmic_pins->dio[1]);
//    Serial.print("dio[2]: "); Serial.println(plmic_pins->dio[2]);

    // initialize SPI chip select to high (it's active low)
    gpio.output(plmic_pins->nss, 1);

    if (plmic_pins->rxtx != LMIC_UNUSED_PIN) {
        // initialize to RX
        gpio.output(plmic_pins->rxtx, !plmic_pins->rxtx_rx_active);
    }
    if (plmic_pins->rst != LMIC_UNUSED_PIN) {
        // initialize RST to floating
        gpio.input(plmic_pins->rst);
    }

    hal_interrupt_init();
}

// val == 1  => tx
void hal_pin_rxtx (u1_t val) {
    if (plmic_pins->rxtx != LMIC_UNUSED_PIN)
        gpio.write(plmic_pins->rxtx, val != plmic_pins->rxtx_rx_active);
}

// set radio RST pin to given value (or keep floating!)
void hal_pin_rst (u1_t val) {
    if (plmic_pins->rst == LMIC_UNUSED_PIN)
        return;

    if(val == 0 || val == 1) { // drive pin
        gpio.output(plmic_pins->rst, val);
    } else { // keep pin floating
        gpio.input(plmic_pins->rst);
    }
}

s1_t hal_getRssiCal (void) {
    return plmic_pins->rssi_cal;
}

//--------------------
// Interrupt handling
//--------------------
static constexpr unsigned NUM_DIO_INTERRUPT = 3;
static_assert(NUM_DIO_INTERRUPT <= NUM_DIO, "Number of interrupt-sensitive lines must be less than number of GPIOs");
static ostime_t interrupt_time[NUM_DIO_INTERRUPT] = {0};

#if !defined(LMIC_USE_INTERRUPTS)
static void hal_interrupt_init() {
    gpio.input(plmic_pins->dio[0]);
    if (plmic_pins->dio[1] != LMIC_UNUSED_PIN)
        gpio.input(plmic_pins->dio[1]);
    if (plmic_pins->dio[2] != LMIC_UNUSED_PIN)
        gpio.input(plmic_pins->dio[2]);
    static_assert(NUM_DIO_INTERRUPT == 3, "Number of interrupt lines must be set to 3");
}

static bool dio_states[NUM_DIO_INTERRUPT] = {0};
void hal_pollPendingIRQs_helper() {
    uint8_t i;
    for (i = 0; i < NUM_DIO_INTERRUPT; ++i) {
        if (plmic_pins->dio[i] == LMIC_UNUSED_PIN)
            continue;

        if (dio_states[i] != (gpio.read(plmic_pins->dio[i]) ? true : false)) {
            dio_states[i] = !dio_states[i];
            if (dio_states[i] && interrupt_time[i] == 0) {
                ostime_t const now = os_getTime();
                interrupt_time[i] = now ? now : 1;
            }
        }
    }
}

#else
// Interrupt handlers

static void hal_isrPin0() {
    if (interrupt_time[0] == 0) {
        ostime_t now = os_getTime();
        interrupt_time[0] = now ? now : 1;
    }
}
static void hal_isrPin1() {
    if (interrupt_time[1] == 0) {
        ostime_t now = os_getTime();
        interrupt_time[1] = now ? now : 1;
    }
}
static void hal_isrPin2() {
    if (interrupt_time[2] == 0) {
        ostime_t now = os_getTime();
        interrupt_time[2] = now ? now : 1;
    }
}

typedef void (*isr_t)();
static const isr_t interrupt_fns[NUM_DIO_INTERRUPT] = {hal_isrPin0, hal_isrPin1, hal_isrPin2};
static_assert(NUM_DIO_INTERRUPT == 3, "number of interrupts must be 3 for initializing interrupt_fns[]");

static void hal_interrupt_init() {
  for (uint8_t i = 0; i < NUM_DIO_INTERRUPT; ++i) {
      if (plmic_pins->dio[i] == LMIC_UNUSED_PIN)
          continue;

      pinMode(plmic_pins->dio[i], INPUT);
      attachInterrupt(digitalPinToInterrupt(plmic_pins->dio[i]), interrupt_fns[i], RISING);
  }
}
#endif // LMIC_USE_INTERRUPTS

void hal_processPendingIRQs() {
    uint8_t i;
    for (i = 0; i < NUM_DIO_INTERRUPT; ++i) {
        ostime_t iTime;
        if (plmic_pins->dio[i] == LMIC_UNUSED_PIN)
            continue;

        // NOTE(tmm@mcci.com): if using interrupts, this next step
        // assumes uniprocessor and fairly strict memory ordering
        // semantics relative to ISRs. It would be better to use
        // interlocked-exchange, but that's really far beyond
        // Arduino semantics. Because our ISRs use "first time
        // stamp" semantics, we don't have a value-race. But if
        // we were to disable ints here, we might observe a second
        // edge that we'll otherwise miss. Not a problem in this
        // use case, as the radio won't release IRQs until we
        // explicitly clear them.
        iTime = interrupt_time[i];
        if (iTime) {
            interrupt_time[i] = 0;
            radio_irq_handler_v2(i, iTime);
        }
    }
}

// -----------------------------------------------------------------------------
// SPI

static void hal_spi_init () {
    spi.setup();
}

static void hal_spi_trx(u1_t cmd, u1_t* buf, size_t len, bit_t is_read) {
    u1_t nss = plmic_pins->nss;

    gpio.write(nss, 0);

    spi.xmit(1, &cmd, 0, NULL);
    spi.xmit(is_read ? 0 : len, buf, is_read ? len : 0, buf);

    gpio.write(nss, 1);
}

void hal_spi_write(u1_t cmd, const u1_t* buf, size_t len) {
    hal_spi_trx(cmd, (u1_t*)buf, len, 0);
}

void hal_spi_read(u1_t cmd, u1_t* buf, size_t len) {
    hal_spi_trx(cmd, buf, len, 1);
}

// -----------------------------------------------------------------------------
// TIME

static void hal_time_init () {
    /*
	  * Use 16-bit Timer 3 for ticks (one tick == 32 µs == 256 CPU cycles).
	  * Increment global 16-bit variable timer3_overflow on overflow.
	  * Resulting counter resolution: 32 bit (overflows after 38 hours).
	  */
    TCCR3A = 0;
    TCCR3B = _BV(WGM12) | _BV(CS32); // normal mode, /256
    TIMSK3 = _BV(TOIE3);
}

u4_t hal_ticks () {
    return ((uint32_t)timer3_overflow << 16) | TCNT3;
}

// Returns the number of ticks until time. Negative values indicate that
// time has already passed.
static s4_t delta_time(u4_t time) {
    return (s4_t)(time - hal_ticks());
}

u4_t hal_waitUntil (u4_t time) {
    sei();
    s4_t delta = delta_time(time);
    cli();
    // check for already too late.
    if (delta < 0)
        return -delta;

    // Enable interrupts so that timer3_overflow is incremented on overflow.
    sei();
    while (delta_time(time) > 0);
    cli();

    return 0;
}

// check and rewind for target time
u1_t hal_checkTimer (u4_t time) {
    // No need to schedule wakeup, since we're not sleeping
    return delta_time(time) <= 0;
}

static uint8_t irqlevel = 0;

void hal_disableIRQs () {
    cli();
    irqlevel++;
}

void hal_enableIRQs () {
    if(--irqlevel == 0) {
        sei();

#if !defined(LMIC_USE_INTERRUPTS)
        // Instead of using proper interrupts (which are a bit tricky
        // and/or not available on all pins on AVR), just poll the pin
        // values. Since os_runloop disables and re-enables interrupts,
        // putting this here makes sure we check at least once every
        // loop.
        //
        // As an additional bonus, this prevents the can of worms that
        // we would otherwise get for running SPI transfers inside ISRs.
        // We merely collect the edges and timestamps here; we wait for
        // a call to hal_processPendingIRQs() before dispatching.
        hal_pollPendingIRQs_helper();
#endif /* !defined(LMIC_USE_INTERRUPTS) */
    }
}

uint8_t hal_getIrqLevel(void) {
    return irqlevel;
}

void hal_sleep () {
    // Not implemented
}

// -----------------------------------------------------------------------------

#if defined(LMIC_PRINTF_TO)
#if !defined(__AVR)
static ssize_t uart_putchar (void *, const char *buf, size_t len) {
    return LMIC_PRINTF_TO.write((const uint8_t *)buf, len);
}

static cookie_io_functions_t functions =
 {
     .read = NULL,
     .write = uart_putchar,
     .seek = NULL,
     .close = NULL
 };

void hal_printf_init() {
    stdout = fopencookie(NULL, "w", functions);
    if (stdout != nullptr) {
        setvbuf(stdout, NULL, _IONBF, 0);
    }
}
#else // defined(__AVR)
static int uart_putchar (char c, FILE *)
{
    LMIC_PRINTF_TO.write(c) ;
    return 0 ;
}

void hal_printf_init() {
    // create a FILE structure to reference our UART output function
    static FILE uartout;
    memset(&uartout, 0, sizeof(uartout));

    // fill in the UART file descriptor with pointer to writer.
    fdev_setup_stream (&uartout, uart_putchar, NULL, _FDEV_SETUP_WRITE);

    // The uart is the standard output device STDOUT.
    stdout = &uartout ;
}

#endif // !defined(ESP8266) || defined(ESP31B) || defined(ESP32)
#endif // defined(LMIC_PRINTF_TO)

void hal_init (void) {
    // use the global constant
    Multipass_LMIC::hal_init_with_pinmap(&lmic_pins);
}

// hal_init_ex is a C API routine, written in C++, and it's called
// with a pointer to an lmic_pinmap.
void hal_init_ex (const void *pContext) {
    const lmic_pinmap * const pHalPinmap = (const lmic_pinmap *) pContext;
    if (! Multipass_LMIC::hal_init_with_pinmap(pHalPinmap)) {
        hal_failed(__FILE__, __LINE__);
    }
}

// C++ API: initialize the HAL properly with a configuration object
namespace Multipass_LMIC {
bool hal_init_with_pinmap(const HalPinmap_t *pPinmap)
    {
    if (pPinmap == nullptr)
        return false;

    // set the static pinmap pointer.
    plmic_pins = pPinmap;

    // set the static HalConfiguration pointer.
    HalConfiguration_t * const pThisHalConfig = pPinmap->pConfig;

    if (pThisHalConfig != nullptr)
        pHalConfig = pThisHalConfig;
    else
        pHalConfig = &nullHalConig;

    pHalConfig->begin();

    // configure radio I/O and interrupt handler
    hal_io_init();
    // configure radio SPI
    hal_spi_init();
    // configure timer and interrupt handler
    hal_time_init();
#if defined(LMIC_PRINTF_TO)
    // printf support
    hal_printf_init();
#endif
    // declare success
    return true;
    }
}; // namespace Multipass_LMIC


void hal_failed (const char *file, u2_t line) {
    if (custom_hal_failure_handler != NULL) {
        (*custom_hal_failure_handler)(file, line);
    }

#if defined(LMIC_FAILURE_TO)
    LMIC_FAILURE_TO.println("FAILURE ");
    LMIC_FAILURE_TO.print(file);
    LMIC_FAILURE_TO.print(':');
    LMIC_FAILURE_TO.println(line);
    LMIC_FAILURE_TO.flush();
#endif

    hal_disableIRQs();

    // Infinite loop
    while (1) {
        ;
    }
}

void hal_set_failure_handler(const hal_failure_handler_t* const handler) {
    custom_hal_failure_handler = handler;
}

ostime_t hal_setModuleActive (bit_t val) {
    // setModuleActive() takes a c++ bool, so
    // it effectively says "val != 0". We
    // don't have to.
    return pHalConfig->setModuleActive(val);
}

bit_t hal_queryUsingTcxo(void) {
    return pHalConfig->queryUsingTcxo();
}

uint8_t hal_getTxPowerPolicy(
    u1_t inputPolicy,
    s1_t requestedPower,
    u4_t frequency
    ) {
    return (uint8_t) pHalConfig->getTxPowerPolicy(
                        Multipass_LMIC::HalConfiguration_t::TxPowerPolicy_t(inputPolicy),
                        requestedPower,
                        frequency
                        );
}

#ifndef __acweaving
ISR(TIMER3_OVF_vect)
{
	timer3_overflow++;
}
#endif
