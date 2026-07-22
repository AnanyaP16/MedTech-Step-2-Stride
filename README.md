# MedTech Smart Cane

A PlatformIO-based Arduino project for a smart cane that uses an MPU6050 sensor to detect tilt-based movement and estimate steps. The system also drives an 8-LED NeoPixel ring to visually show progress toward a daily step goal.

## Overview

This project combines:
- An Arduino Uno with an MPU6050 accelerometer/gyroscope
- A NeoPixel LED ring for visual feedback
- Serial output for logging tilt angle and step count
- Python scripts for collecting and plotting sensor data

It is designed to help track cane movement and provide simple step-count-like feedback from the device’s motion patterns.

## Hardware

- Arduino Uno
- MPU6050 motion sensor
- NeoPixel ring (8 LEDs)
- USB connection for serial communication

## Software Requirements

### Arduino / Firmware
- PlatformIO
- Arduino framework support
- The following libraries are configured in platformio.ini:
  - Adafruit MPU6050
  - I2Cdevlib-MPU6050
  - Adafruit NeoPixel

### Python Analysis Scripts
Install the following Python packages:

```bash
pip install pandas matplotlib pyserial
```

## Project Structure

```text
src/main.cpp           # Arduino firmware
analysis/read_serial.py # Reads serial data from the Arduino and saves it
analysis/plot.py       # Plots saved sensor data
data/walkTrial.csv     # Example recorded data
platformio.ini         # PlatformIO configuration
```

## Getting Started

1. Open the project in VS Code with PlatformIO.
2. Connect the Arduino Uno to your computer.
3. Build and upload the firmware:

```bash
pio run -t upload
```

4. Open the serial monitor at 38400 baud:

```bash
pio device monitor
```

## Data Collection

The Arduino sends data in the form:

```text
angle,steps
```

To capture and save this data:

1. Update the serial port in [analysis/read_serial.py](analysis/read_serial.py) if needed.
2. Run:

```bash
python analysis/read_serial.py
```

This will save the collected data to [data/walkTrial.csv](data/walkTrial.csv).

## Plotting Data

To visualize the saved data:

```bash
python analysis/plot.py
```

## Notes

- The serial baud rate is set to 38400 in the firmware and scripts.
- The NeoPixel ring updates at different step milestones based on a configurable daily goal.
- The tilt detection logic may need tuning depending on the cane’s motion and sensor placement.

## Future Improvements

Possible enhancements include:
- More accurate step detection
- Better calibration for different users
- Bluetooth or wireless logging
- A mobile app or dashboard for real-time monitoring
