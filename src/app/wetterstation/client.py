#!/usr/bin/env python3
#
# Copyright 2020 Daniel Friesel
#
# SPDX-License-Identifier: BSD-2-Clause

import numpy as np
import re
import requests
import serial
import serial.threaded
import sys
import time

class SerialReader(serial.threaded.Protocol):
    """
    Character- to line-wise data buffer for serial interfaces.

    Reads in new data whenever it becomes available and exposes a line-based
    interface to applications.
    """
    def __init__(self, callback):
        """Create a new SerialReader object."""
        self.callback = callback
        self.recv_buf = ''

    def __call__(self):
        return self

    def data_received(self, data):
        """Append newly received serial data to the line buffer."""
        try:
            str_data = data.decode('UTF-8')
            self.recv_buf += str_data

            # We may get anything between \r\n, \n\r and simple \n newlines.
            # We assume that \n is always present and use str.strip to remove leading/trailing \r symbols
            # Note: Do not call str.strip on lines[-1]! Otherwise, lines may be mangled
            lines = self.recv_buf.split('\n')
            if len(lines) > 1:
                self.recv_buf = lines[-1]
                for line in lines[:-1]:
                    self.callback(str.strip(line))

        except UnicodeDecodeError:
            pass
            #sys.stderr.write('UART output contains garbage: {data}\n'.format(data = data))

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
        self.ser.parity = 'N'
        self.ser.rtscts = False
        self.ser.xonxoff = False

        try:
            self.ser.open()
        except serial.SerialException as e:
            sys.stderr.write('Could not open serial port {}: {}\n'.format(self.ser.name, e))
            sys.exit(1)

        self.reader = SerialReader(callback = callback)
        self.worker = serial.threaded.ReaderThread(self.ser, self.reader)
        self.worker.start()

    def close(self):
        """Close serial connection."""
        self.worker.stop()
        self.ser.close()

if __name__ == '__main__':

    got_data = False
    step = 0

    def parse_line(line):

        global got_data

        match = re.match('Anemometer Count = ([^ ]+)', line)
        if match:
            got_data = True
            wind_clicks = float(match.group(1))
            windspeed_kmh = wind_clicks * 2.4 / 10
            requests.post('http://influxdb:8086/write?db=sensors', data='anemometer,area=hm17_,location=balkon windspeed_kmh={}'.format(windspeed_kmh))

    monitor = SerialMonitor('/dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.1:1.0-port0', 57600, parse_line)

    try:
        while True:
            time.sleep(5)

            step += 1

            if step == 4:
                if not got_data:
                    print('Error: received no data for 20 seconds', file=sys.stderr)
                    sys.exit(1)
                got_data = False
                step = 0

    except KeyboardInterrupt:
        monitor.close()
