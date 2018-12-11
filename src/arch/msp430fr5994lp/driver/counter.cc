#include "driver/counter.h"

#if defined(TIMER_CYCLES)
#warn "timer_cycles and counter are mutually exclusive. Expect odd behaviour."
#endif

Counter counter;
