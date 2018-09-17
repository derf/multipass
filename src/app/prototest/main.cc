#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "lib/ArduinoJson.h"

void loop(void)
{
	char json[] = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
	ArduinoJson::StaticJsonBuffer<200> jsonBuffer;
	ArduinoJson::JsonObject& root = jsonBuffer.parseObject(json);
	const char *sensor = root["sensor"];
	kout << "sensor: " << sensor << endl;
	gpio.led_toggle(1);
#ifdef TIMER_S
	kout << dec << uptime.get_s() << endl;
#endif
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();

	gpio.led_on(0);
	kout << "Hello, World!" << endl;
	arch.idle_loop();

	return 0;
}
