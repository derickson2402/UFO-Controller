/*##############################################################################

Date
	05 May 2022

Written By
	Dan Erickson (dan@danerick.com)

Description
	Radio transceiver for controlling UFO drone, adapted from akarsh98 and
	DzikuVx on GitHub. Before deploying, the parameters below must be defined
	according to your setup (you can likely use the defaults and just change
	IS_RECEIVER). Also, debugging output can be enabled, but you may want to
	take a look at the corresponding Hardware-Tests files also.

	Please see the README in the 'Transceiver' folder for more.

Project URL
	https://github.com/derickson2402/UFO-Controller.git

##############################################################################*/

// You must set the following items before deploying. See README for help
// #define DEBUG_ON                         // Print debug output (turns off PPM)
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

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <util/atomic.h>
#if defined(DEBUG_ON)
	#include <printf.h>
#endif

/* Data packet to be transmitted. nRF24L01 chip can only send 32 bytes at a
 * time, so we get up to 32 8 bit channels. Currently 6 are used, corresponding
 * to first 6 channels in MultiWii/config.h */
struct UFODataPacket {
	byte throttle, yaw, pitch, roll, AUX1, AUX2;
	/* Reset the values to neutral defaults */
	void clear() {
		throttle = AUX1 = AUX2 = 0;
		yaw = pitch = roll = 127;
	}
};

/* Translate 8-bit packet from Arduino to PPM array of [1000,2000]µs channels */
void UFOtoPPM(UFODataPacket& data, int* ppm);

/* Configure serial communication to the Arduino running MultiWii controller.
 * Uses PPM over PIN_PPM */
void configPPM();

/* Configure the radio for transmission. Settings must be the same for both
 * radios to communicate */
void configRadio(RF24& radio);

/*############################################################################*/

RF24 radio(PIN_RADIO_CE, PIN_RADIO_CSN); // Reference to nRF24 radio
int ppm[12];     // Globally readable PPM packet to MultiWii
UFODataPacket data;       // Datapacket sent Arduino -> Arduino over nRF24 radio
unsigned long lastComm;   // Time in ms of last packet received from transmitter

void setup() {
	configRadio(radio);
	configPPM();
	#if defined(DEBUG_ON)
		Serial.begin(115200);
		printf_begin();
		radio.printPrettyDetails();
	#endif
}

void loop() {
	#if defined(IS_RECEIVER)
		// Keep polling radio, timer interrupt will handle PPM to MultiWii
		while (radio.available()) {
			radio.read(&data,sizeof(UFODataPacket));
			lastComm = millis();
		}
		// Check if we possibly lost connection after ~1 second
		if (millis() - lastComm > 1000) {
			data.clear();
			UFOtoPPM(data, ppm);
		}
	#else
		// Get current gyro values and transmit them to receiver
		// TODO: implement gyroscope reading. See #1
		data.clear();
		UFOtoPPM(data, ppm);
		radio.write(&data, sizeof(UFODataPacket));
	#endif
	#if defined(DEBUG_ON)
		delay(1000);
	#endif
}

/*############################################################################*/

void configRadio(RF24& radio) {
	radio.begin();
	// We don't want ACK since we are broadcasting packets quickly and won't
	// re-send them (think UDP)
	radio.setAutoAck(false);
	radio.setDataRate(RF24_250KBPS);
	radio.setPayloadSize(sizeof(UFODataPacket));
	radio.setPALevel(RF24_PA_MAX);

	#if defined(IS_RECEIVER)
		radio.openReadingPipe(1, PIPE_NUM);
		radio.startListening();
		lastComm = millis();
	#else
		radio.openWritingPipe(PIPE_NUM);
		radio.stopListening();
	#endif
}

void UFOtoPPM(UFODataPacket& data, int* ppm) {
	// UFODataPacket uses 8-bit data but PPM needs pulses of duration in µs.
	// Store results to temp location so we can use as few CPU cycles as
	// possible when updating ppm, since we must temporarily block interrupts
	// or risk sending bogus data. This could be made faster by 
	int temp[6];
	temp[0] = map(data.throttle, 0, 255, 1000, 2000);
	temp[1] = map(data.yaw,      0, 255, 1000, 2000);
	temp[2] = map(data.pitch,    0, 255, 1000, 2000);
	temp[3] = map(data.roll,     0, 255, 1000, 2000);
	temp[4] = map(data.AUX1,     0, 1,   1000, 2000);
	temp[5] = map(data.AUX2,     0, 1,   1000, 2000);
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		// TODO: implement this using pointers to improve speed (would take 2 CPU cycles instead of ~26). See #3
		memcpy(ppm, temp, sizeof(temp[0])*6);
	}
}

void configPPM() {
	#if !defined(IS_RECEIVER)
		return;
	#endif

	// Set 'safe' values on boot since we use interrupts for PPM comm
	data.throttle = data.AUX1 = data.AUX2 = 0;
	data.yaw = data.pitch = data.roll = 127;
	UFOtoPPM(data, ppm);

	// Configure pin connecting to MultiWii
	pinMode(PIN_PPM, OUTPUT);
	digitalWrite(PIN_PPM, 0);

	// Taken from this guide on Arduino PPM implementation:
	// https://quadmeup.com/generate-ppm-signal-with-arduino/
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		TCCR1A = 0; // set entire TCCR1 register to 0
		TCCR1B = 0;
		OCR1A = 100; // compare match register
		TCCR1B |= (1 << WGM12);  // turn on CTC mode
		TCCR1B |= (1 << CS11);  // 8 prescaler: 0,5 microseconds at 16mhz
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
