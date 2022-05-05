# UFO Transceiver System

This submodule uses a pair of nRF24L01 transmitters, one as a transmitter one as a receiver. Check out the [RF24 GitHub](https://github.com/nRF24/RF24) for more help.
This code is adapted from [akarsh98 on GitHub](https://github.com/akarsh98/DIY-Radio-Controller-for-Drone-Arduino-Based-Quadcopter). Thanks akarsh98!

## Wiring

See the corresponding images in ```Wiring-Diagrams``` for help wiring the modules.
It may be helpful to note that the chip needs a 3.3V power source but the logic pins are 5V tolerant, and can be connected directly to the Arduino.
Also, to reduce noise, you might need a 10μF electrolytic or tantalum capacitor across the VCC and GND pins (this helps when using a poor power supply).
See the [COMMON_ISSUES.md](https://github.com/nRF24/RF24/blob/master/COMMON_ISSUES.md) file in the RF24 source repo for more help.

### Specific Configuration for Chips With External Antennas

nRF24L01+PA+LNA chips have a built in Power Amplifier and Low Noise Amplifier so they are very good for longer-range transmission.
With that said, they need an external 3.3V power source because the amplifier inside needs 3x as much current as the Arduino can give it.
Additionally, they may need electromagnetic shielding from the power supply.
Again see the [COMMON_ISSUES.md](https://github.com/nRF24/RF24/blob/master/COMMON_ISSUES.md) file in the RF24 source repo for more help.

## Deploying Code

When uploading the sketch to your Arduino's, there are 3 parameters you must configure.

1. Is the unit a receiver or a transmitter?
2. Choose a unique 40 bit address for the Arduinos to talk on  
    a. Must be same for transmitter/receiver pair, and unique from other pairs
3. Choose a channel to use [0-125]  
    a. Must be same for transmitter/receiver pair, and unique from other pairs  
    b. Frequency band is 2,400MHz + (channel)MHz (so channel 81 = 2481MHz)  
    c. Higher channels sometimes interfere less with WiFi networks, may work better
4. You can re-define the CE and CSN pins in the code if you need
