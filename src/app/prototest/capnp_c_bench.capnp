@0xad5b236043de2389;

struct Benchmark {
	data @0 :List(Float64);
	nesting @1 :Nesting;
	sensor @2 :Text;
	time @3 :Int64;

	struct Nesting {
		foo @0 :Foo;

		struct Foo {
			hurr @0 :Text;
			qwop @1 :Int64;
		}
	}
}
