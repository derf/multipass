#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"

#ifdef PROTOTEST_BENCH_CYCLES
#include "driver/counter.h"
#endif

#ifdef PROTOTEST_ARDUINOJSON
#include "lib/ArduinoJson.h"
#endif
#ifdef PROTOTEST_BINN
#include "lib/binn.h"
#endif
#ifdef PROTOTEST_CAPNPROTO_C
#include <capnp_c.h>
#include "capnp_c_bench.capnp.h"
#endif
#ifdef PROTOTEST_MANUALJSON
#include "object/stdbuf.h"
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

void loop(void)
{
	//static uint16_t ts = 0;
	//ts++;

#ifdef PROTOTEST_INCLUDE_LOCAL
#include "prototest_local.cc.inc"
#endif

}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();

	//gpio.led_on(0);
	kout << "Hello, World!" << endl;

#if defined(PROTOTEST_BENCH_CYCLES) and !defined(PROTOTEST_ARCH_esp8266)
	while (1) {
		loop();
	}
#else
	arch.idle_loop();
#endif

	return 0;
}
