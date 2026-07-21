import serial
import pandas as pd
import time


SERIAL_PORT = 'COM3'  # Update with your serial port
BAUD_RATE = 38400
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

time.sleep(2) #wait for Arduino to initialize
data = []

try:
    while True:
        line = ser.readline().decode('ascii', errors='ignore').strip()
        try:
            angle, steps = line.split(',')
            data.append([float(angle), int(steps)])
        except:
            pass
except KeyboardInterrupt:
    print("Data collection stopped.")

ser.close()

df = pd.DataFrame(data, columns=['Angle', 'Steps'])
df.to_csv('../data/walkTrial.csv', index=False)
print("Data saved to data/walkTrial.csv")