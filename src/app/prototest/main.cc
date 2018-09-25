#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "lib/ArduinoJson.h"
#include "lib/modernjson/json.h"
#include "object/stdbuf.h"
#include "object/xdrstream.h"

char buf[256];

void loop(void)
{
	char json[] = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
	ArduinoJson::StaticJsonBuffer<200> jsonBuffer;
	ArduinoJson::JsonObject& root = jsonBuffer.parseObject(json);
	const char *sensor = root["sensor"];
	kout << "sensor: " << sensor << endl;

	nlohmann::json j = R"({"compact": true, "schema": 0})"_json;
	std::vector<std::uint8_t> v_cbor = nlohmann::json::to_cbor(j);

	kout << "CBOR vector is " << hex;
	for (unsigned int i = 0; i < v_cbor.size(); i++) {
		kout << v_cbor[i] << " ";
	}
	kout << endl;

	kout << "manual JSON: {\"sensor\":\"gps\",\"time\":" << dec << 1351824120;
	kout << ",\"data\":[" << 48.756080 << "," << 2.302038 << "]}" << endl;

	BufferOutput<XDRStream> foostream(buf);

	char test[] = "Obai World!";

	foostream << 123 << 0 << 12345678;
	foostream.setNextArrayLen(3);
	foostream << fixed << "Hai";
	foostream.setNextArrayLen(sizeof(test));
	foostream << variable << (char const *)test;

	kout << "foostream is " << hex;
	for (unsigned int i = 0; i < 64; i += 4) {
		kout << (unsigned char)buf[i] << (unsigned char)buf[i+1];
		kout << (unsigned char)buf[i+2] << (unsigned char)buf[i+3] << " ";
	}
	kout << endl;

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
