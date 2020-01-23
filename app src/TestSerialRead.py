import serial
import time

ser = serial.Serial("/dev/tty.usbserial-A9Y75F57", 115200)

ser.write(b'r')
info = []

while True:
    line = ser.readline()
    info.append(line)
    if "Resistor?" in line:
        break

for piece in info:
    print(piece)