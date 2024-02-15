#!/usr/bin/env python3

import re
import serial
import serial.threaded
import sys
import time

counter_freq = None
timer_overflow = None


class SerialReader(serial.threaded.Protocol):
    """
    Character- to line-wise data buffer for serial interfaces.

    Reads in new data whenever it becomes available and exposes a line-based
    interface to applications.
    """

    def __init__(self, callback):
        """Create a new SerialReader object."""
        self.callback = callback
        self.recv_buf = ""

    def __call__(self):
        return self

    def data_received(self, data):
        """Append newly received serial data to the line buffer."""
        try:
            str_data = data.decode("UTF-8")
            self.recv_buf += str_data

            # We may get anything between \r\n, \n\r and simple \n newlines.
            # We assume that \n is always present and use str.strip to remove leading/trailing \r symbols
            # Note: Do not call str.strip on lines[-1]! Otherwise, lines may be mangled
            lines = self.recv_buf.split("\n")
            if len(lines) > 1:
                self.recv_buf = lines[-1]
                for line in lines[:-1]:
                    self.callback(str.strip(line))

        except UnicodeDecodeError:
            pass
            # sys.stderr.write('UART output contains garbage: {data}\n'.format(data = data))


class SerialMonitor:
    """SerialMonitor captures serial output for a specific amount of time."""

    def __init__(self, port: str, baud: int, callback):
        """
        Create a new SerialMonitor connected to port at the specified baud rate.

        Communication uses no parity, no flow control, and one stop bit.
        Data collection starts immediately.
        """
        self.ser = serial.serial_for_url(port, do_not_open=True)
        self.ser.baudrate = baud
        self.ser.parity = "N"
        self.ser.rtscts = False
        self.ser.xonxoff = False

        try:
            self.ser.open()
        except serial.SerialException as e:
            sys.stderr.write(
                "Could not open serial port {}: {}\n".format(self.ser.name, e)
            )
            sys.exit(1)

        self.reader = SerialReader(callback=callback)
        self.worker = serial.threaded.ReaderThread(self.ser, self.reader)
        self.worker.start()

    def close(self):
        """Close serial connection."""
        self.worker.stop()
        self.ser.close()


def handle_line(line):
    if counter_freq is not None:
        print_line = line
        for match in re.finditer("(\d+)/(\d+) cycles", line):
            cycles = int(match.group(1))
            overflows = int(match.group(2))
            ms = (cycles + timer_overflow * overflows) * 1000 / counter_freq
            match_line = f"{cycles}/{overflows} cycles"
            new_line = f"{ms} ms ({cycles}/{overflows} cycles)"
            print_line = re.sub(match_line, new_line, print_line)
        print(print_line)
    else:
        print(line)


if __name__ == "__main__":
    tty = sys.argv[1]
    baud = int(sys.argv[2])

    if len(sys.argv) > 4:
        counter_freq = int(sys.argv[3])
        timer_overflow = int(sys.argv[4])

    monitor = SerialMonitor(tty, baud, handle_line)
    time.sleep(20)
    monitor.close()
