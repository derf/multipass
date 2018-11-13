#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"

#ifdef PROTOTEST_ARDUINOJSON
#include "lib/ArduinoJson.h"
#endif
#ifdef PROTOTEST_MODERNJSON
#include "lib/modernjson/json.h"
#endif
#ifdef PROTOTEST_NANOPB
#include <pb.h>
#include "nanopb.pb.h"
#include <pb_encode.h>
#include <pb_decode.h>
#endif
#ifdef PROTOTEST_XDR
#include "object/stdbuf.h"
#include "object/xdrstream.h"
#include "object/xdrinput.h"
#endif

#include <stdint.h>

#ifdef PROTOTEST_XDR
char buf[256];
#endif

// TODOs
//
// Code -> JSON
// Code -> XDR
// Code -> MsgPack
// Code -> ProtoBuf
// Code -> CBOR
//
// JSON -> Code/Data
// XDR -> Code/Data
// MsgPack -> Code/Data
// ProtoBuf -> Code/Data
// CBOR -> Code/Data

void loop(void)
{
	static uint16_t ts = 0;

	/*
	 * XDR
	 */

#ifdef PROTOTEST_XDR
	BufferOutput<XDRStream> foostream(buf);
	XDRInput input(buf);

	char test[] = "Obai World!";

	foostream << (uint32_t)123 << (int16_t)-2 << ts << (uint16_t)0 << (uint64_t)4294967296 << (uint16_t)0;
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

	kout << dec;
	kout << "foostream = " << input.get_uint32() << " = " << 123;
	kout << ", " << input.get_int32() << " = " << -2;
	kout << ", " << input.get_uint32() << " = " << ts;
	kout << ", " << input.get_uint32();
	kout << ", " << input.get_uint64();
	kout << ", " << input.get_uint32();
	kout << ", " << input.get_opaque(3);
	uint32_t len = input.get_opaque_length();
	kout << ", " << input.get_opaque(len);
	kout << endl;
#endif

	/*
	 * ArduinoJSON
	 */

#ifdef PROTOTEST_ARDUINOJSON
	char json[] = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
	ArduinoJson::StaticJsonBuffer<200> jsonBuffer;
	ArduinoJson::JsonObject& root = jsonBuffer.parseObject(json);
	const char *sensor = root["sensor"];
	kout << "sensor: " << sensor << endl;
#endif

	/*
	 * NanoPB
	 */

#ifdef PROTOTEST_NANOPB

	uint8_t buf[128];
	size_t len;
	bool status;

	{
		TestMessage msg = TestMessage_init_zero;
		pb_ostream_t stream = pb_ostream_from_buffer(buf, sizeof(buf));
		msg.number = 423;
		status = pb_encode(&stream, TestMessage_fields, &msg);
		len = stream.bytes_written;
		kout << len << " bytes written" << endl;
	}

#endif

	/*
	 * Common
	 */

	gpio.led_toggle(1);
#ifdef TIMER_S
	kout << dec << uptime.get_s() << endl;
#endif
	ts++;
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
