import serial
import time

ser = serial.Serial("/dev/tty.usbserial-A9Y75F57", 115200)

while True:
    line = ser.readline()
    print(line)
    if (b'Resistor?\n' in line) or (b'Value MSN?\n' in line) or (b'Value LSN?\n' in line):
        cmd = input()
        if "quit" in cmd:
            ser.close()
            break
        ser.write(cmd.encode("utf-8"))