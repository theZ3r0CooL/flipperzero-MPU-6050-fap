# Flipperzero-Accelerometer-Debugger-fap

This is a simple accelerometer debugger for FlipperZero using a MPU-6050 module. 

# GPIO Connections:
#### MPU-6050 Pin (Name) -> Flipper Pin (Name) / [Purpose]

Pin 1 (VCC) -> Pin 9 (3v3)  / [Voltage In]
Pin 2 (GND) -> Pin 18 (GND) / [Ground]
Pin 3 (SCL) -> Pin 16 (C0)  / [Serial Clock]
Pin 4 (SDA) -> Pin 15 (C1)  / [Serial Data]     
Pin 5 (XDA) -> Not used     / [Aux Serial Data]
Pin 6 (XCL) -> Not used     / [Aux Serial Clock]
Pin 7 (AD0) -> Not used     / [I2C Protocol]
Pin 8 (INT) -> Pin 7 (C3)   / [Interrupt]