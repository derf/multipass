#!/usr/bin/env python3

import numpy as np
import paho.mqtt.client as mqtt
import re
import requests
import serial
import serial.threaded
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

    mqtt = mqtt.Client()
    mqtt.connect('172.23.225.193');

    accel_factor = 2. / 32768
    gyro_factor = 250. / 32768
    magnet_factor = 150e-9

    accel_minmax = np.zeros((6))
    gyro_minmax = np.zeros((6))
    magnet_minmax = np.zeros((6))

    max_accel = 0
    max_magnet = 0
    vcc = 0
    intervals = [60, 300, 600, 3600, 7200]
    accel_by_5s = list()

    step = 0

    def parse_line(line):

        global max_accel
        global max_magnet
        global vcc

        match = re.match('button(.)', line)
        if match:
            mqtt.publish('sensor/button', int(match.group(1)))

        match = re.match('Min Accel: ([^ ]+) / ([^ ]+) / ([^ ]+)', line)
        if match:
            accel_minmax[0] = int(match.group(1)) * accel_factor
            accel_minmax[1] = int(match.group(2)) * accel_factor
            accel_minmax[2] = int(match.group(3)) * accel_factor

        match = re.match('Max Accel: ([^ ]+) / ([^ ]+) / ([^ ]+)', line)
        if match:
            accel_minmax[3] = int(match.group(1)) * accel_factor
            accel_minmax[4] = int(match.group(2)) * accel_factor
            accel_minmax[5] = int(match.group(3)) * accel_factor

        match = re.match('Min Gyro: ([^ ]+) / ([^ ]+) / ([^ ]+)', line)
        if match:
            gyro_minmax[0] = int(match.group(1)) * gyro_factor
            gyro_minmax[1] = int(match.group(2)) * gyro_factor
            gyro_minmax[2] = int(match.group(3)) * gyro_factor

        match = re.match('Max Gyro: ([^ ]+) / ([^ ]+) / ([^ ]+)', line)
        if match:
            gyro_minmax[3] = int(match.group(1)) * gyro_factor
            gyro_minmax[4] = int(match.group(2)) * gyro_factor
            gyro_minmax[5] = int(match.group(3)) * gyro_factor

        match = re.match('Min Magnet: ([^ ]+) / ([^ ]+) / ([^ ]+)', line)
        if match:
            magnet_minmax[0] = int(match.group(1)) * magnet_factor
            magnet_minmax[1] = int(match.group(2)) * magnet_factor
            magnet_minmax[2] = int(match.group(3)) * magnet_factor

        match = re.match('Max Magnet: ([^ ]+) / ([^ ]+) / ([^ ]+)', line)
        if match:
            magnet_minmax[3] = int(match.group(1)) * magnet_factor
            magnet_minmax[4] = int(match.group(2)) * magnet_factor
            magnet_minmax[5] = int(match.group(3)) * magnet_factor

        match = re.match('CPU VCC: ([^ ]+)', line)
        if match:
            vcc = int(match.group(1))

        match = re.match('MPU Temp: ([^ ]+)', line)
        if match:
            temp = float(match.group(1))
            max_accel = np.linalg.norm(accel_minmax[:3] - accel_minmax[3:])
            max_magnet = np.linalg.norm(magnet_minmax[3:])
            mqtt.publish('sensor/accel_g', max_accel)
            mqtt.publish('sensor/flux_t', max_magnet)
            #mqtt.publish('sensor/gyro_dps', np.linalg.norm(gyro_minmax[:3] - gyro_minmax[3:]))
            requests.post('http://192.168.0.200:8086/write?db=hosts', data='embedded,name=structure,area=hm17 mpu9250_degc={:f},mpu9250_mv={:d}'.format(temp, vcc))

    monitor = SerialMonitor('/dev/ttyUSB0', 57600, parse_line)

    try:
        while True:
            time.sleep(5)

            accel_by_5s.append(max_accel)
            step += 1

            if step == 4:
                for interval in intervals:
                    index_interval = int(interval / 5)
                    mqtt.publish('sensor/accel{}'.format(interval), max(accel_by_5s[ -index_interval : ]))
                accel_by_5s = accel_by_5s[ -intervals[-1] : ]
                step = 0

    except KeyboardInterrupt:
        monitor.close()
        mqtt.disconnect()
