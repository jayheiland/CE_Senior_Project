import serial
import time

waitsec = 1
ser = serial.Serial("/dev/tty.usbserial-A9Y75F57", 115200)
while True: #use this loop in the serial port init to get to the first Reistor? prompt, the program can then write whatever
    line = ser.readline()
    print(line)
    if "Resistor?" in line:
        break

#print(ser.readline())
ser.write(b'1')
time.sleep(waitsec)
print(ser.readline())
ser.write(b'F')
time.sleep(waitsec)
print(ser.readline())
ser.write(b'F')
time.sleep(waitsec)
print(ser.readline())

ser.write(b'1')
time.sleep(waitsec)
print(ser.readline())
ser.write(b'8')
time.sleep(waitsec)
print(ser.readline())
ser.write(b'0')
time.sleep(waitsec)
print(ser.readline())
ser.close()