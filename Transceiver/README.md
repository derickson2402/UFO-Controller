# UFO Transceiver System

This submodule uses a pair of nRF24L01 transmitters, one as a transmitter one as a receiver. Check out the [RF24 GitHub](https://github.com/nRF24/RF24) for more help.
The PPM code is adapted from [this guide](https://quadmeup.com/generate-ppm-signal-with-arduino/) (thanks Pawel!).

## Wiring

See the corresponding images in ```Wiring-Diagrams``` for help wiring the modules.
It may be helpful to note that the chip needs a 3.3V power source but the logic pins are 5V tolerant, and therefore can be connected directly to the Arduino.
Also, to reduce noise, you might need a 10µF electrolytic or tantalum capacitor across the VCC and GND pins (this helps when using a poor power supply).
See the [COMMON_ISSUES.md](https://github.com/nRF24/RF24/blob/master/COMMON_ISSUES.md) file in the RF24 source repo for more help.

### Specific Configuration for Chips With External Antennas

nRF24L01+PA+LNA chips have a built in Power Amplifier and Low Noise Amplifier so they are very good for long range transmission.
With that said, they need an external 3.3V power source because said amplifier needs 3x as much current as the Arduino can give it.
Additionally, the chips may need electromagnetic shielding from the power supply.
Again see the [COMMON_ISSUES.md](https://github.com/nRF24/RF24/blob/master/COMMON_ISSUES.md) file in the RF24 source repo for more help.

## Deploying Code

When uploading the sketch to your Arduino's, there are some parameters at the top of ```Transceiver.ino``` you must configure.
There are short descriptions in the code but you can get more detail here:

1. Is the unit a receiver or a transmitter? Should debug mode be used?  
    a. Turning on debug mode deactivates PPM output (due to it needing timer interrupts)
2. Choose a unique 40 bit address for the Arduinos to talk on  
    a. Must be same for transmitter/receiver pair, and unique from other pairs
3. Choose a channel to use [0-125]  
    a. Must be same for transmitter/receiver pair, and unique from other pairs  
    b. Frequency band is 2,400MHz + (channel)MHz (so channel 81 = 2481MHz)  
    c. Higher channels sometimes interfere less with WiFi networks, may work better
4. You can re-define the CE and CSN pins in the code if you need (see nRF24 docs for more)
5. For the receiver only, choose PPM pin number and frame/pulse length  
    a. This must connect to the MultiWii controller pin 2  
    b. Communication is 12 channel PPM sum, where each channel is from [1000, 2000]µs (see [this guide](https://quadmeup.com/generate-ppm-signal-with-arduino/) for PPM help)  
    c. Channel order is set in MultiWii, default for this repo is in ```MultiWii/config.h``` as ```THROTTLE,YAW,PITCH,ROLL,AUX1,AUX2,AUX3,AUX4,8,9,10,11```  
    d. Frame length (total length of one transmission) is approximately ```(numChannels * 2000µsMaxSignalLength) + 3000µsFrameSpace```, and pulse length (signaling start of next channel) can be somewhere in [100, 500]µs
