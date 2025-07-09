
**I2S Bluetooth Transmitter**

In this example, a simple ESP32 board is used as a Bluetooth transmitter. 
The suggestion for this came from schreibfaul1 (https://github.com/schreibfaul1/ESP32-audioI2S/tree/master/examples/I2S%20Bluetooth%20Transmitter).

The BT transmitter is the I2S slave and is connected to the master as described there.

The ESP32-A2DP library is used by P. Schatzmann, https://github.com/pschatzmann/ESP32-A2DP.git

Since the ESP32-audioI2s library uses an I2S bus sampling rate of 48000Hz, the ESP BT library has been adapted to this sampling rate for the A2DP source mode. This eliminates the need to resample the I2S data to 44100 Hz.

The adapted BT library is used here as a precompiled archive (libbt48.a). Since the archive was created with ESP-IDF version 5.4.2, it can only be used with Arduino releases 3.2.0 and 3.2.1.

