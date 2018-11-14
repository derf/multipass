#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"

#ifdef PROTOTEST_ARDUINOJSON
#include "lib/ArduinoJson.h"
#endif
#ifdef PROTOTEST_MODERNJSON
#include "lib/modernjson/json.h"
#endif
#ifdef PROTOTEST_MPACK
#include "mpack.h"
#endif
#ifdef PROTOTEST_NANOPB
#include <pb.h>
#include "nanopb.pb.h"
#include <pb_encode.h>
#include <pb_decode.h>
#endif
#ifdef PROTOTEST_UBJSON
#include "ubj.h"
#endif
#ifdef PROTOTEST_XDR
#include "object/stdbuf.h"
#include "object/xdrstream.h"
#include "object/xdrinput.h"
#endif

#include <stdint.h>

#ifdef PROTOTEST_INCLUDE_GLOBAL
#include "prototest_global.cc.inc"
#endif

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

#ifdef PROTOTEST_INCLUDE_LOCAL
#include "prototest_local.cc.inc"
#endif

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
	 * ModernJSON
	 */

#ifdef PROTOTEST_MODERNJSON
	nlohmann::json js1;
	js1["sensor"] = "gps";
	js1["time"] = ts;
	js1["data"] = {48.756080, 2.302038};
	kout << js1.dump() << endl;

	nlohmann::json js2 = {
		{"sensor", "gps"},
		{"time", ts},
		{"data", {48.756080, 2.302038} }
	};
	kout << js2.dump() << endl;

	std::vector<std::uint8_t> v_cbor = nlohmann::json::to_cbor(js2);
	kout << "CBOR vector is " << hex;
	for (unsigned int i = 0; i < v_cbor.size(); i++) {
		kout << v_cbor[i] << " ";
	}
	kout << endl;
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
	 * MPack
	 */

#ifdef PROTOTEST_MPACK
	char buf[128];
	for (unsigned int i = 0; i < 128; i++) {
		buf[i] = 0;
	}
	mpack_writer_t writer;
	mpack_writer_init(&writer, buf, sizeof(buf));

	mpack_start_map(&writer, 2);
	mpack_write_cstr(&writer, "gps");
	mpack_write_uint(&writer, ts);
	mpack_start_array(&writer, 2);
	mpack_write_float(&writer, 48.756080);
	mpack_write_float(&writer, 2.302038);
	mpack_finish_array(&writer);
	mpack_finish_map(&writer);

	if (mpack_writer_destroy(&writer) != mpack_ok) {
		kout << "Encoding failed" << endl;
	}
	kout << "mpack is " << hex;
	for (unsigned int i = 0; i < 128; i++) {
		kout << (uint8_t)buf[i];
	}
	kout << endl;

#endif

	/*
	 * UBJSON
	 */

#ifdef PROTOTEST_UBJSON

	uint8_t buf[128];
	for (unsigned int i = 0; i < 128; i++) {
		buf[i] = 0;
	}

	ubjw_context_t* ctx = ubjw_open_memory(buf, buf + sizeof(buf));
	ubjw_begin_array(ctx, UBJ_MIXED, 0);
	ubjw_write_int16(ctx, ts);
	ubjw_write_string(ctx, "Noot Noot");
	ubjw_end(ctx);

	kout << "ubjr_close_context: " << ubjw_close_context(ctx) << endl;

	kout << "ubjr is " << hex;
	for (unsigned int i = 0; i < 128; i++) {
		kout << (uint8_t)buf[i];
	}
	kout << endl;

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
