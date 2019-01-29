multipass - a multi-architecture library operating system
---------------------------------------------------------

multipass aims to aid development and evaluation of operating system components
on a diverse set of architectures. It provides a basic set of drivers (e.g.
for standard output on a serial interface) and tries to get out of the way
as much as possible.

multipass is single-threaded by design. At compile-time, the switch `app=...`
selects an application, which must implement `int main(void)` and do
everything itself from that point on. When using `loop=1`, users must also
implement `void loop(void)`, which will be executed roughly once per second
- but only if the main application is idle.

# Getting Started

The compilation process is controlled by the Makefile, which also contains
targets for flashing microcontrollers, info, and help output. Most targets
require two mandatory arguments: `arch` (target architecture) and `app`
(which application to build and run). Operating system behaviour is fine-tuned
using additional (optional) flags, which may be set both on the command line
and in an application Makefile.

To avoid redundancy in the typical `make program arch=... app=... && make monitor
arch=... app=...` workflow, two helper scripts are provided:

* `./mp` is a shortcut for `make info` and `make program` (build and flash)
* `./mpm` is a shortcut for `make info`, `make program`, and `make monitor` (build, flash, and monitor output)

Flags are passed to each `make` invocation.

For a quick start, try ledblink:

`./mpm arch=posix app=ledblink`

You should see some data about the compilation process, "Hello, world!", and
some numbers. As POSIX is a fake-architecture (it builds an ELF binary which
is executed directly on Linux), you do not need a microcontroller to run it.
Terminate execution using Ctrl+C.

To see the blinkenlights, there's an optional (`arch=posix`-specific) flag:

`./mpm arch=posix app=ledblink gpio_trace=1`

Now, you should see a simulated LED being toggled every second.

# Supported Architectures, Drivers, and Flags

To see all supported architectures, run `make help arch=posix`. It will also
show architecture-independent flags and drivers.

For architecture-specific options, set the `arch` flag, e.g.
`make help arch=arduino-nano`.
