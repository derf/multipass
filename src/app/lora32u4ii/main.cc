/*
 * Copyright 2021 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/uptime.h"
#include "driver/adc.h"

/*
 * Work in progress! Joining is sometimes successful, but that's about it.
 * TX with user-defined payloads after joining doesn't appear to work yet.
 */

#define CFG_eu868 1
#define CFG_sx1276_radio 1
#define DISABLE_LMIC_FAILURE_TO

#include <lmic.h>
#include <hal/hal.h>

static const u1_t PROGMEM APPEUI[8]={ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8);}

static const u1_t PROGMEM DEVEUI[8]={ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);}

static const u1_t PROGMEM APPKEY[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16);}

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 10;

bool joined = false;

// DIYMall BSFrance LoRa32u4 II v1.3
// See also: arduino packages/adafruit/hardware/avr/1.4.13/variants/feather32u4/pins_arduino.h
// PB0 -> Charger enable and Vbat/2 measurement voltage divider enable
// PB0 is also the !SS pin, so it must be configured as output in order to use SPI.
// PB5 "D9" <- Vbat/2 via voltage divider. Appears to also have a connection to the user LED
// PC7 "D13" -> User LED
const lmic_pinmap lmic_pins = {
	.nss = GPIO::pb4,
	.rxtx = LMIC_UNUSED_PIN,
	.rst = GPIO::pd4,
	.dio = {GPIO::pe6, GPIO::pc6, LMIC_UNUSED_PIN},
};

static osjob_t sendjob;

void do_send(osjob_t* j){
	uint16_t bat = adc.getVBat_mV(false);

	// Check if there is not a current TX/RX job running
	if (LMIC.opmode & OP_TXRXPEND) {
		kout.pprint(PSTR("OP_TXRXPEND\n"));
	} else {
		// Prepare upstream data transmission at the next possible time.
		LMIC_setTxData2(1, (uint8_t *)&bat, sizeof(bat), 0);
		kout.pprint(PSTR("Packet queued\n"));
	}
	// Next TX is scheduled after TX_COMPLETE event.
}

void do_sleep(){
    kout.pprint(PSTR("naptime\n"));

    for(int i=0; i<75; i++){
         arch.idle();
    }
}

void onEvent (ev_t ev) {
    switch(ev) {
        case EV_SCAN_TIMEOUT:
            kout.pprint(PSTR("EV_SCAN_TIMEOUT\n"));
            break;
        case EV_JOINING:
            kout.pprint(PSTR("EV_JOINING\n"));
            break;
        case EV_JOINED:
            kout.pprint(PSTR("EV_JOINED\n"));
            #if 1
            {
              u4_t netid = 0;
              devaddr_t devaddr = 0;
              u1_t nwkKey[16];
              u1_t artKey[16];
              LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
              kout.pprint(PSTR("netid: "));
              kout << dec << netid << endl;
              kout.pprint(PSTR("devaddr: "));
              kout << hex << devaddr << endl;
              //Serial.print(PSTR("AppSKey: "));
              for (size_t i=0; i<sizeof(artKey); ++i) {
                if (i != 0) {
                  //Serial.print(PSTR("-"));
                }
                //printHex2(artKey[i]);
              }
              kout.pprint(PSTR(""));
              //Serial.print(PSTR("NwkSKey: "));
              for (size_t i=0; i<sizeof(nwkKey); ++i) {
                      if (i != 0) {
                              //Serial.print("-");
                      }
                      //printHex2(nwkKey[i]);
              }
              kout << endl;
            }
            #endif
            joined = true;
            break;
        case EV_JOIN_FAILED:
            kout.pprint(PSTR("EV_JOIN_FAILED\n"));
            break;
        case EV_TXCOMPLETE:
            kout.pprint(PSTR("EV_TXCOMPLETE (includes waiting for RX windows)\n"));
            // Schedule next transmission
            if (joined) {
                do_sleep();
                do_send(&sendjob);
            } else {
                os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);
            }
            break;
        case EV_RESET:
            kout.pprint(PSTR("EV_RESET\n"));
            break;
        case EV_RXCOMPLETE:
            // data received in ping slot
            kout.pprint(PSTR("EV_RXCOMPLETE\n"));
            break;
        case EV_LINK_DEAD:
            kout.pprint(PSTR("EV_LINK_DEAD\n"));
            break;
        case EV_LINK_ALIVE:
            kout.pprint(PSTR("EV_LINK_ALIVE\n"));
            break;
        case EV_TXSTART:
            kout.pprint(PSTR("EV_TXSTART\n"));
            break;
        case EV_TXCANCELED:
            kout.pprint(PSTR("EV_TXCANCELED\n"));
            break;
        case EV_RXSTART:
            /* do not print anything -- it wrecks timing */
            break;
        case EV_JOIN_TXCOMPLETE:
            kout.pprint(PSTR("EV_JOIN_TXCOMPLETE: no JoinAccept\n"));
            break;

        default:
            kout.pprint(PSTR("Unknown event: "));
            kout << (unsigned) ev;
            break;
    }
}

void loop(void)
{
	os_runloop_once();
	gpio.led_toggle();
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();
	gpio.input(GPIO::pb5);
	gpio.led_on();

	kout.pprint(PSTR("Hello, World!\n"));
	kout.pprint(PSTR("Test, World!\n"));

	os_init();
	LMIC_reset();
	LMIC_setClockError(MAX_CLOCK_ERROR * 20 / 100);
	LMIC_setAdrMode(0);
	do_send(&sendjob);

	while (1) {
		os_runloop_once();
		gpio.led_toggle();
	}
	arch.idle_loop();

	return 0;
}
