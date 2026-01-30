# MQTT → DMX Gateway for ATMega2560 + WiFi Boards

Setup:

* Configure and flash <https://git.finalrewind.org/derf/esp8266-nodemcu-uart-bridge> on an ESP8266 (e.g. the on-board one, if you are using an “Arduino ATMega2560 + WiFi” board)
* Connect UART1 TX to MAX485 RX → DMX input
* Connect UART3 RX to ESP8266 TX
* Copy .../mqtt-to-dmx-atmega2560/.config to multipass/.config
* Run `./mp`
