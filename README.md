# multipass - a multi-architecture library operating system

**multipass** is a C++ Library Operating System for a few embedded
architectures.  As such, it does not provide multi-threading support or similar
conveniences.  Its objective is similar to the Arduino environment: provide a
simple framework for embedded application/driver development and evaluation,
and get out of the way as much as possible. It favors simplicity over
performance and abstraction.  Re-using components outside of multipass should
be fairly easy.

Application, architecture, and drivers are configured using `make config` (X11,
kconfig-qconf) or `make nconfig` (Terminal, kconfig-nconf). Each application
must implement `int main(void)` and do everything itself from that point on.
When the loop feature is enabled, `void loop(void)` must be implemented also.

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
some numbers. As POSIX is not a standalone architecture (it builds an ELF
binary that is executed directly on Linux), you do not need a microcontroller
to run it. Terminate execution using Ctrl+C.

## Supported Architectures

See `make config` for an up-to-date list.

### ATMega168P, ATMega328P (Arduino Nano)

Peripheral communication:

* I²C (master only, interrupt-driven)
* SPI (master only, polling)
* UART (output polling, input interrupt-driven)
* DMX (polling, via UART, overrides regular serial output)
* NeoPixel/WS2812B (Adafruit driver)

Hardware features:

* ADC (partially)

### MSP430FR5969, MSP430FR5994 (MSP430FR59xx Launchpad)

Peripheral communication:

* I²C on eUSCI\_B0 (FR5969) / eUSCI\_B1 (FR5994) (master only, interrupt-driven)
* SPI on eUSCI\_B0 (FR5969) / eUSCI\_B1 (FR5994) (master only, polling)
* UART on eUSCI\_A0 (FR5969) / eUSCI\_A1 (FR5994) (output polling, input interrupt-driven)
* DMX on eUSCI\_A1 (FR5969) / eUSCI\_A3 (FR5994) (polling)

Hardware features:

* 20bit mode (use up to 256kB FRAM for code and data)
* ADC (partially)

### POSIX

Runs the selected application as POSIX thread, e.g. under Linux on a Raspberry Pi.

Peripheral communication:

* I²C (master only, via `/dev/i2c`)
* stdin/stdout

## Included Drivers

See `make config` for an up-to-date list.

* AM2320 I²C Temperature and Humidity Sensor
* BME280 I²C Temperature, Humidity, and Pressure Sensor (Bosch SensorTec driver)
* BME680 I²C Temperature, Humidity, Pressure, and Air Quality Sensor (Bosch SensorTec driver)
* CCS811 I²C Air Quality Sensor
* DS2482 I²C 1-Wire Bus Controller
* HDC1080 I²C Temperature and Humidity Sensor
* LM75 I²C Temperature Sensor
* LS013B4DN04 96×96 transflective LCD (430BOOST-SHARP96)
* MAX44006 I²C RGB and IR Light Sensor
* MAX44009 I²C Light Sensor
* MPU9250 I²C Accelerometer, Gyroscope, and Magnetometer Sensor
* Pervasive Aurora Mb V230/V231 4.2" iTC E-Paper Display via EPD Extension Board Gen 2
* SCD40/SCD41 I²C CO₂ Sensor
* SEN5x I²C particulate matter (PM1.0/PM2.5/PM4.0/PM10) and VOC sensor
* SSD1306 I²C OLED Display Controller
* VEML6075 I²C UVA/UVB Light Sensor
