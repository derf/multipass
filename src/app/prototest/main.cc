#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
//#include "driver/counter.h"

#ifdef PROTOTEST_ARDUINOJSON
#include "lib/ArduinoJson.h"
#endif
#ifdef PROTOTEST_CAPNPROTO_C
#include <capnp_c.h>
#include "capnp_c_bench.capnp.h"
#endif
#ifdef PROTOTEST_MODERNJSON
#include "lib/modernjson/json.h"
#endif
#ifdef PROTOTEST_MPACK
#include "mpack.h"
#endif
#ifdef PROTOTEST_NANOPB
#include <pb.h>
#include "nanopbbench.pb.h"
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

#ifdef PROTOTEST_ARDUINOJSON
char buf[256];
#endif
#ifdef PROTOTEST_XDR
char buf[256];
#endif

#ifdef PROTOTEST_NANOPB
bool encode_hurr(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
if (!pb_encode_tag_for_field(stream, field)) return false;
return pb_encode_string(stream, (uint8_t*)"durr", 4);
}
bool encode_sensor(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
if (!pb_encode_tag_for_field(stream, field)) return false;
return pb_encode_string(stream, (uint8_t*)"gps", 3);
}
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
	uint32_t tmp;

#ifdef PROTOTEST_INCLUDE_LOCAL
#include "prototest_local.cc.inc"
#endif

	/*
	 * XDR
	 */


kout << "{";
kout << "\"data\":[";
kout << "48.75608" << ",";
kout << "2.302038";
kout << "]" << ",";
kout << "\"nesting\":[";
kout << "[";
kout << "1";
kout << "]" << ",";
kout << "[";
kout << "2" << ",";
kout << "2";
kout << "]" << ",";
kout << "{";
kout << "\"foo\":{";
kout << "\"hurr\":\"durr\"";
kout << "}";
kout << "}";
kout << "]" << ",";
kout << "\"sensor\":\"gps\"" << ",";
kout << "\"time\":" << dec << ts;
kout << "}" << endl;

#ifdef PROTOTEST_CAPNPROTO_C
	uint8_t buf[1024];
	for (unsigned int i = 0; i < sizeof(buf); i++) {
		buf[i] = 0;
	}
	struct capn c;
	capn_init_malloc(&c);
	capn_ptr cr = capn_root(&c);
	struct capn_segment *cs = cr.seg;

	struct Benchmark benchmark;
	//struct Benchmark_Nesting benchmark_nesting;
	//struct Benchmark_Nesting_Foo benchmark_nesting_foo;
	benchmark.time = ts + 1;

	capn_text sensor_text;
	sensor_text.len = 3;
	sensor_text.str = "gps";
	sensor_text.seg = NULL;

	benchmark.sensor = sensor_text;

	//benchmark.nesting = new_Benchmark_Nesting(cs);
	//benchmark_nesting.foo = new_Benchmark_Nesting_Foo(cs);

	//write_Benchmark_Nesting_Foo(&benchmark_nesting_foo, benchmark_nesting.foo);
	//write_Benchmark_Nesting(&benchmark_nesting, benchmark.nesting);

	Benchmark_ptr benchmark_ptr = new_Benchmark(cs);
	write_Benchmark(&benchmark, benchmark_ptr);

	capn_write_mem(&c, buf, sizeof(buf), 0 /* packed */);
	capn_free(&c);

	kout << "capnproto is " << hex;
	for (unsigned int i = 0; i < sizeof(buf); i++) {
		kout << buf[i];
	}
	kout << endl;
#endif

#ifdef PROTOTEST_XDR
	BufferOutput<XDRStream> xdrstream(buf);
	xdrstream.setNextArrayLen(2);
	xdrstream << 48.75608;
	xdrstream << 2.302038;
	xdrstream.setNextArrayLen(3);
	xdrstream.setNextArrayLen(1);
	xdrstream << 1;
	xdrstream.setNextArrayLen(2);
	xdrstream << 2;
	xdrstream << 2;
	xdrstream.setNextArrayLen(4);
	xdrstream << variable << "durr";
	xdrstream.setNextArrayLen(3);
	xdrstream << variable << "gps";
	xdrstream << ts;

	kout << "xdrstream is " << hex;
	for (unsigned int i = 0; i < 64; i += 4) {
		kout << (unsigned char)buf[i] << (unsigned char)buf[i+1];
		kout << (unsigned char)buf[i+2] << (unsigned char)buf[i+3] << " ";
	}
	kout << endl;

	XDRInput input(buf);
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
	for (unsigned int i = 0; i < 128; i++) {
		buf[i] = 0;
	}

	{
		counter.start();
		ArduinoJson::StaticJsonBuffer<200> jsonBuffer;

		ArduinoJson::JsonObject& root = jsonBuffer.createObject();
		ArduinoJson::JsonArray& rootl = root.createNestedArray("data");
		rootl.add(48.75608);
		rootl.add(2.302038);
		ArduinoJson::JsonArray& rootl_ = root.createNestedArray("nesting");
		ArduinoJson::JsonArray& rootl_l = rootl_.createNestedArray();
		rootl_l.add(1);
		ArduinoJson::JsonArray& rootl_l_ = rootl_.createNestedArray();
		rootl_l_.add(2);
		rootl_l_.add(2);
		ArduinoJson::JsonObject& rootl_o = rootl_.createNestedObject();
		ArduinoJson::JsonObject& rootl_oo = rootl_o.createNestedObject("foo");
		rootl_oo["hurr"] = "durr";
		root["sensor"] = "gps";
		root["time"] = ts;


/*
		ArduinoJson::JsonObject& root = jsonBuffer.createObject();
		root["sensor"] = "gps";
		root["time"] = ts;
		ArduinoJson::JsonArray& data = root.createNestedArray("data");
		data.add(48.756080);
		data.add(2.302038);
		*/

		root.printTo(buf);
		tmp = counter.stop();
		kout << "ArduinoJSON serialization took " << tmp << " >" << counter.overflowed << " cycles" << endl;
		kout << "buf is " << buf << endl;
	}

	{
		char json[] = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
		ArduinoJson::StaticJsonBuffer<200> jsonBuffer;
		ArduinoJson::JsonObject& root = jsonBuffer.parseObject(json);
		const char *sensor = root["sensor"];
		kout << "sensor: " << sensor << endl;
	}
#endif

	/*
	 * ModernJSON
	 */

#ifdef PROTOTEST_MODERNJSON
	nlohmann::json js1;
	js1["data"] = {48.756080, 2.302038};
	js1["nesting"][0] = {1};
	js1["nesting"][1] = {2, 2};
	js1["nesting"][2]["foo"]["hurr"] = "durr";
	js1["sensor"] = "gps";
	js1["time"] = ts;
	kout << "string:" << js1.dump() << endl;

	std::vector<std::uint8_t> v_cbor = nlohmann::json::to_cbor(js1);
	kout << "CBOR:" << hex;
	for (unsigned int i = 0; i < v_cbor.size(); i++) {
		kout << v_cbor[i] << " ";
	}
	kout << endl;

	std::vector<std::uint8_t> v_msgpack = nlohmann::json::to_msgpack(js1);
	kout << "MsgPack:" << hex;
	for (unsigned int i = 0; i < v_msgpack.size(); i++) {
		kout << v_msgpack[i] << " ";
	}
	kout << endl;

	std::vector<std::uint8_t> v_ubjson = nlohmann::json::to_ubjson(js1);
	kout << "UBJSON:" << hex;
	for (unsigned int i = 0; i < v_ubjson.size(); i++) {
		kout << v_ubjson[i] << " ";
	}
	kout << endl;
	kout << "UBJSON: " << hex;
	for (unsigned int i = 0; i < v_ubjson.size(); i++) {
		kout << (char)v_ubjson[i] << " ";
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
		Benchmark msg = Benchmark_init_zero;
		pb_ostream_t stream = pb_ostream_from_buffer(buf, sizeof(buf));
		msg.data[0] = 48.75608;
		msg.data[1] = 2.302038;
		msg.nesting.foo.hurr.funcs.encode = encode_hurr;
		msg.nesting.foo.qwop = 9001;
		msg.sensor.funcs.encode = encode_sensor;
		msg.time = ts;
		status = pb_encode(&stream, Benchmark_fields, &msg);
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
	mpack_start_map(&writer, 4);
	mpack_write_cstr(&writer, "data");
	mpack_start_array(&writer, 2);
	mpack_write_float(&writer, 48.756080);
	mpack_write_float(&writer, 2.302038);
	mpack_finish_array(&writer);
	mpack_write_cstr(&writer, "nesting");
	mpack_start_map(&writer, 1);
	mpack_write_cstr(&writer, "foo");
	mpack_start_map(&writer, 2);
	mpack_write_cstr(&writer, "hurr");
	mpack_write_cstr(&writer, "durr");
	mpack_write_cstr(&writer, "qwop");
	mpack_write_uint(&writer, 9001);
	mpack_finish_map(&writer);
	mpack_finish_map(&writer);
	mpack_write_cstr(&writer, "sensor");
	mpack_write_cstr(&writer, "gps");
	mpack_write_cstr(&writer, "time");
	mpack_write_uint(&writer, ts);
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

	uint8_t buf[192];
	for (unsigned int i = 0; i < sizeof(buf); i++) {
		buf[i] = 0;
	}

	ubjw_context_t* ctx = ubjw_open_memory(buf, buf + sizeof(buf));
	ubjw_begin_object(ctx, UBJ_MIXED, 0);
	ubjw_write_key(ctx, "data");

	ubjw_begin_array(ctx, UBJ_FLOAT32, 0);
	ubjw_write_float64(ctx, 48.75608);
	ubjw_write_float64(ctx, 2.302038);
	ubjw_end(ctx);

	ubjw_write_key(ctx, "nesting");
	ubjw_begin_array(ctx, UBJ_MIXED, 0);

	ubjw_begin_array(ctx, UBJ_MIXED, 0);
	ubjw_write_integer(ctx, 1);
	ubjw_end(ctx);

	ubjw_begin_array(ctx, UBJ_MIXED, 0);
	ubjw_write_integer(ctx, 2);
	ubjw_write_integer(ctx, 2);
	ubjw_end(ctx);

	ubjw_begin_object(ctx, UBJ_MIXED, 0);
	ubjw_write_key(ctx, "foo");

	ubjw_begin_object(ctx, UBJ_MIXED, 0);
	ubjw_write_key(ctx, "hurr");
	ubjw_write_string(ctx, "durr");
	ubjw_end(ctx);
	ubjw_end(ctx);
	ubjw_end(ctx);

	ubjw_write_key(ctx, "sensor");
	ubjw_write_string(ctx, "gps");
	ubjw_write_key(ctx, "time");
	ubjw_write_integer(ctx, ts);

	ubjw_end(ctx);

	kout << "ubjw_close_context: " << ubjw_close_context(ctx) << endl;

	kout << "ubjson is " << hex;
	for (unsigned int i = 0; i < sizeof(buf); i++) {
		kout << (uint8_t)buf[i];
	}
	kout << endl;
	kout << "ubjson is " << (char*)buf << endl;

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

#if defined(PROTOTEST_BENCH_CYCLES) and (defined(MULTIPASS_ARCH_arduino_nano) or defined(MULTIPASS_ARCH_msp430fr5969lp))
	while (1) {
		loop();
	}
#else
	arch.idle_loop();
#endif

	return 0;
}
