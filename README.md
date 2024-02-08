# multipass - a multi-architecture library operating system

**multipass** is a C++ Library Operating System for a few embedded
targets / architectures.  As such, it does not provide multi-threading support
or similar conveniences.  Its objective is similar to the Arduino environment:
provide a simple framework for embedded application/driver development and
evaluation with a `main()` and optional `loop()` function and a text output
channel, and then get out of the way as much as possible. It favors simplicity
over performance and proper abstraction.  Re-using components outside of
multipass should be fairly easy.

Application, target / architecture, and drivers are configured using `make
config` (X11, kconfig-qconf) or `make nconfig` (Terminal, kconfig-nconf). Each
application must implement `int main(void)` and do everything itself from that
point on.  If the loop or wakeup features are enabled, `void loop(void)` or
`void wakeup(void)` must be implemented as well.

## Getting Started

* make config
* make
* make program
* make monitor

Two helper scripts are provided:

* `./mp` is a shortcut for `make info` and `make program` (build and flash)
* `./mpm` is a shortcut for `make info`, `make program`, and `make monitor` (build, flash, and monitor output)

Flags are passed to each `make` invocation.

For common applications, the `arch` and `app` compile switches can be used,
e.g. `./mpm arch=posix app=ledblink`

You should see some data about the compilation process, "Hello, world!", and
some numbers. As POSIX is not a standalone target (it builds an ELF binary that
is executed directly on Linux), you do not need a microcontroller to run it.
Terminate execution using Ctrl+C.

## Supported Targets

See `make config` for an up-to-date list. Unless noted otherwise, all targets
support GPIO input/output, UART output (typically using the built-in USB-TTL
chip of the respective development board), and an optional cycle counter.
The following table gives a quick overview over targets and additional
features; the remainder of this README covers details.

| Target | Clock | Non-Volatile Memory | Volatile Memory | Supported Drivers / Features |
| :--- | ---: | ---: | ---: | :--- |
| ATMega168P | 16 MHz | 512 KiB Flash + 512 B EEPROM | 1 KiB SRAM | I²C, SPI, UART, WS2812B, ADC |
| ATMega328P | 16 MHz | 32 KiB Flash + 1 KiB EEPROM | 2 KiB SRAM | I²C, SPI, UART, WS2812B, ADC |
| ATMega2560 | 16 MHz | 256 KiB Flash + 4 KiB EEPROM | 8 KiB SRAM | I²C, UART, DMX, ADC |
| LM4F120H5QR (Cortex-M4F) | 80 MHz | 48 KiB Flash | 4 KiB SRAM | |
| MSP430FR5969 | 16 MHz | 48 (64) KiB FRAM | 2 KiB SRAM | I²C, SPI, UART, DMX, ADC |
| MSP430FR5994 | 16 MHz | 48 (256) KiB FRAM | 4 (8) KiB SRAM | I²C, SPI, UART, DMX, ADC |
| RM46L852 (Cortex-R4F) | 160 MHz | 1.25 MiB Flash | 192 KiB SRAM | |
| STM32F446RE (Cortex-M4) | 168 MHz | 512 KiB Flash | 128 KiB SRAM | |
| STM32F746ZG (Cortex-M7) | 216 MHz | 1 MiB Flash | 320 KiB SRAM | |
| POSIX | – | – | – | I²C |

## Supported Architectures

See `make config` for an up-to-date list.

### ATMega168P, ATMega328P (Arduino Nano)

Peripheral communication:

* I²C controller
* SPI controller
* UART input/output
* WS2812B output via Adafruit NeoPixel driver

Hardware features:

* ADC (partially)

### ATMega2560

Peripheral communication:

* I²C controller
* UART input/output on USART0 / USART1 / USART2 / USART3
* DMX output on USART1 / USART2 / USART3

Hardware features:

* ADC (partially)

### LM4F120H5QR (Stellaris Launchpad)

Peripheral communication:

* UART output

### MSP430FR5969 (MSP430FR5969 Launchpad)

Peripheral communication:

* I²C controller on eUSCI\_B0
* SPI controller on eUSCI\_B0
* UART input/output on eUSCI\_A0 / eUSCI\_A1
* DMX output on eUSCI\_A1

Hardware features:

* ADC (partially)

### MSP430FR5994 (MSP430FR5994 Launchpad)

Peripheral communication:

* I²C controller on eUSCI\_B1
* SPI controller on eUSCI\_B1
* UART input/output on eUSCI\_A0
* DMX output on eUSCI\_A1 / eUSCI\_A2 / eUSCI\_A3

Hardware features:

* 20bit mode (use up to 256 KiB FRAM for code and data)
* ADC (partially)

### RM46L8 (Hercules RM46L8 Launchpad)

Peripheral communication:

* UART output

### STM32F446RE (NUCLEO-F446RE)

Peripheral communication:

* UART output on USART2

### STM32F746ZG (NUCLEO-F746ZG)

Preliminary support, timers may be incorrect.

Peripheral communication:

* UART output on USART3

### POSIX

Runs the selected application as POSIX thread, e.g. under Linux on a Raspberry Pi.

Peripheral communication:

* I²C controller via `/dev/i2c`
* stdin/stdout

## Supported Peripherals

See `make config` for an up-to-date list.

### Sensors

* AM2320 Temperature+Humidity (I²C)
* BME280 Temperature+Humidity+Pressure via Bosch SensorTec driver (I²C)
* BME280 Temperature+Humidity+Pressure+IAQ via Bosch SensorTec driver (I²C)
* CCS811 VOC (I²C)
* HDC1080 Temperature+Humidity (I²C)
* LM75 Temperature (I²C)
* MAX44006 R+G+B+Clear+IR irradiance (I²C)
* MAX44009 illuminance (I²C)
* MPU9250 accelerometer+gyroscope+magnetometer (I²C)
* SCD4x CO₂+Temperature+Humidity (I²C)
* SEN5x PM1.0+PM2.5+PM4.0+PM10+Temperature+Humidity+VOC (I²C)
* VEML6075 UVA+UVB irradiance (I²C)

"VOC" refers to volatile organic compounds; "PM" refers to particulate matter.

### Displays

* LS013B4DN04 / 430BOOST-SHARP96 96×96 transflective LCD (SPI)
* Pervasive Aurora Mb V230/V231 4.2" iTC E-Paper Display via EPD Extension Board Gen 2 (SPI)
* SSD1306 128×64 / 128×32 OLED display controller (I²C)

### Others

* ADS111x ADC (I²C)
* DS2482 1-Wire Bus Controller (I²C)
