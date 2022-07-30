/*##############################################################################

Date
	21 July 2022

Written By
	Dan Erickson (dan@danerick.com)

Description
	Radio transceiver for controlling UFO drone, adapted from akarsh98 and
	DzikuVx on GitHub. Before deploying, the parameters below must be defined
	according to your setup (you can likely use the defaults and just change
	IS_RECEIVER). Debugging output can be enabled, but you may want to take a
	look at the corresponding Hardware-Tests files when doing this.

	Please see the README in the 'Transceiver' folder for more.

Project URL
	https://github.com/derickson2402/UFO-Controller.git

##############################################################################*/

// You must set the following items before deploying. See README for help
// #define DEBUG_ON                      // Print debug output (turns off PPM)
#define IS_RECEIVER                      // Leave for RX, comment out for TX
#define PIPE_NUM         0xE8E8F0F0E1LL  // 40 bit 'Address' the radios use
#define CHANNEL_24G      120             // Channel 0-125 to use on 2.4GHz band
#define PIN_RADIO_CE     9               // Pin to radio CE
#define PIN_RADIO_CSN    10              // Pin to radio CSN

// Following values are receiver only, you can ignore for the transmitter
#define PIN_PPM          2               // Pin to use for PPM comm to MultiWii
#define PPM_FRAME_LEN    27000           // PPM Frame length of a packet in µs
#define PPM_PULSE_LEN    400             // PPM pulse length in µs

/*############################################################################*/

#include "Joystick.h"
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <util/atomic.h>
#if defined(DEBUG_ON)
	#include <printf.h>
#endif
#if !defined(IS_RECEIVER)
	#include "src/I2Cdev.h"
	#include "src/MPU6050.h"
	#include <Wire.h>
#endif


/* Translate 8-bit packet from Arduino to PPM array of [1000,2000]µs channels.
 * Updates PPM packet in memory so PPM pin will automatically send data to
 * MultiWii */
void UFOtoPPM(UFODataPacket& data, volatile int* ppm);


/* Configure serial communication to the Arduino running MultiWii controller.
 * Uses PPM over PIN_PPM */
void configPPM(volatile int* ppm);

/* Configure the radio for transmission. Settings must be the same for both
 * radios to communicate */
void configRadio(RF24& radio);

/*############################################################################*/

RF24 radio(PIN_RADIO_CE, PIN_RADIO_CSN); // Reference to nRF24 radio
UFODataPacket data;       // Datapacket sent Arduino -> Arduino over nRF24 radio
unsigned long lastComm;   // Time in ms of last packet received from transmitter
// Note we need lastComm since receiver will continue broadcasting PPM data to
// MultiWii forever, whether new packets are received or not, so MultiWii
// timeouts will not do anything. Therefore receiver handles timeouts.
// TODO: See #4
#if defined(IS_RECEIVER)
	volatile int ppm[12]; // Globally readable PPM packet to MultiWii
#else
	Joystick * joystick = new SimpleFLR(data);  // Joystick to pull values from
#endif

void setup() {
	#if defined(DEBUG_ON)
		Serial.begin(115200);
		while (!Serial) { /* Make sure serial comms are up */}
		printf_begin();
	#endif
	configRadio(radio);
	#if defined(IS_RECEIVER) && !defined(DEBUG_ON)
		configPPM(ppm);
	#elif !defined(IS_RECEIVER)
		Wire.begin();
	#endif
}

void loop() {
	#if defined(IS_RECEIVER)
		// Keep polling radio, timer interrupt will handle PPM to MultiWii
		while (radio.available()) {
			radio.read(&data,sizeof(UFODataPacket));
			// lastComm = millis();
		}
		#if !defined(DEBUG_ON)
			// Check if we possibly lost connection after ~1 second
			// TODO: See #4, we need a safer way to handle comm loss
			if (millis() - lastComm > 1000) {
				data.clear();
				UFOtoPPM(data, ppm);
			}
		#endif
	#else
		// Get joystick values and transmit them to receiver
		joystick->update();
		radio.stopListening();
		radio.write(&data, sizeof(UFODataPacket));
	#endif
	#if defined(DEBUG_ON)
		// Print out current data and slow down printing rate
		Serial.print(data.throttle, DEC); Serial.print(' ');
		Serial.print(data.yaw,      DEC); Serial.print(' ');
		Serial.print(data.pitch,    DEC); Serial.print(' ');
		Serial.print(data.roll,     DEC); Serial.print(' ');
		Serial.print(data.AUX1,     DEC); Serial.print(' ');
		Serial.println(data.AUX2,   DEC);
		delay(1000);
	#endif
}

/*############################################################################*/

void configRadio(RF24& radio) {
	if (!radio.begin()) {
		while (true) {
			Serial.println(F("radio hardware not responding"));
			delay(2000);
		}
	}
	// We don't want ACK since we are broadcasting packets quickly and won't
	// re-send them (think UDP on IP networks)
	radio.setAutoAck(false);
	radio.setDataRate(RF24_250KBPS);
	radio.setPayloadSize(sizeof(UFODataPacket));
	radio.setPALevel(RF24_PA_MAX);
	radio.setChannel(CHANNEL_24G);
	radio.setAddressWidth(5);

	// Receiver listens, transmitter talks. Also receiver will try to recognize
	// lost signal
	#if defined(IS_RECEIVER)
		radio.openReadingPipe(1, PIPE_NUM);
		radio.startListening();
		lastComm = millis();
	#else
		radio.openWritingPipe(PIPE_NUM);
		radio.stopListening();
	#endif
	#if defined(DEBUG_ON)
		// Run after Serial and radio are up to confirm mode
		radio.printPrettyDetails();
	#endif
}

void UFOtoPPM(UFODataPacket& data, volatile int* ppm) {
	// UFODataPacket uses 8-bit data but PPM needs pulses of duration in µs.
	// Store results to temp location in memory, moves ppm to point to it, then
	// copies over data. This means we block interrupts from as few CPU cycles
	// as possible when updating ppm, while ensuring we don't send bogus data.
	int ppmAux[6];     // Temporary data block for new PPM packet
	volatile int* ppmPtr = ppm; // Ptr back to original PPM packet
	ppmAux[0] = map(data.throttle, 0, 255, 1000, 2000);
	ppmAux[1] = map(data.yaw,      0, 255, 1000, 2000);
	ppmAux[2] = map(data.pitch,    0, 255, 1000, 2000);
	ppmAux[3] = map(data.roll,     0, 255, 1000, 2000);
	ppmAux[4] = map(data.AUX1,     0, 1,   1000, 2000);
	ppmAux[5] = map(data.AUX2,     0, 1,   1000, 2000);
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		ppm = ppmAux; // Look at new memory while we copy over
	}
	// Copy over actual data
	for (uint8_t i = 0; i < 6; ++i) {
		ppm[i] = ppmAux[i];
	}
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		ppm = ppmPtr; // All done, ppmAux will go out of scope now
	}
}

void configPPM(volatile int* ppm) {
	// Set 'safe' values on boot since we use interrupts for PPM comm
	data.clear();
	UFOtoPPM(data, ppm);

	// Configure pin connecting to MultiWii
	pinMode(PIN_PPM, OUTPUT);
	digitalWrite(PIN_PPM, 0);

	// Taken from this guide on Arduino PPM implementation:
	// https://quadmeup.com/generate-ppm-signal-with-arduino/
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		TCCR1A = 0;              // set entire TCCR1 register to 0
		TCCR1B = 0;
		OCR1A = 100;             // compare match register
		TCCR1B |= (1 << WGM12);  // turn on CTC mode
		TCCR1B |= (1 << CS11);   // 8 prescaler: 0,5 microseconds at 16mhz
		TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
	}
}

#if defined(IS_RECEIVER) && !defined(DEBUG_ON)
/* Following method is adapted from this guide on Arduino PPM implementation:
 * 
 * https://quadmeup.com/generate-ppm-signal-with-arduino/
 * 
 * It uses Arduino timer interrupts to send PPM signal to MultiWii Arduino.
 * To not interfere with the transmitter, this method should only be defined
 * for the receiver. */
ISR(TIMER1_COMPA_vect) {
	static boolean state = true;

	TCNT1 = 0;

	if (state) { //start pulse
		digitalWrite(PIN_PPM, HIGH);
		OCR1A = PPM_PULSE_LEN * 2;
		state = false;
	} else{ //end pulse and calculate when to start the next pulse
		static byte cur_chan_numb;
		static unsigned int calc_rest;
	
		digitalWrite(PIN_PPM, LOW);
		state = true;

		if(cur_chan_numb >= 6){
			cur_chan_numb = 0;
			calc_rest = calc_rest + PPM_PULSE_LEN;
			OCR1A = (PPM_FRAME_LEN - calc_rest) * 2;
			calc_rest = 0;
		}
		else{
			OCR1A = (ppm[cur_chan_numb] - PPM_PULSE_LEN) * 2;
			calc_rest = calc_rest + ppm[cur_chan_numb];
			cur_chan_numb++;
		}
	}
}
#endif
