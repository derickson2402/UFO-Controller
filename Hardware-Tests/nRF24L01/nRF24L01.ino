/*##############################################################################

Date
	26 April 2022

Written By
	Dan Erickson (dan@danerick.com)

Description
	Radio transmitter for controlling UFO drone, adapted from akarsh98 on
	GitHub. Used in conjunction with 'receiver.ino' which connects to another
	Arduino running 'MultiWii'. Please see the README in the 'Transceiver'
	folder for more.

Project URL
	https://github.com/derickson2402/UFO-Controller.git

##############################################################################*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Create a unique pipe in. The transmitter must use the same unique code
const uint64_t pipeIn = 0xE8E8F0F0E1LL;

RF24 radio(9, 10);

// The sizeof this struct should not exceed 32 bytes
struct UFODataPacket {
	byte throttle;
	byte yaw;
	byte pitch;
	byte roll;
	byte AUX1;
	byte AUX2;
};

UFODataPacket data;

void resetData() {
	// 'safe' values to use when no radio input is detected
	data.throttle = 0;
	data.yaw = 127;
	data.pitch = 127;
	data.roll = 127;
	data.AUX1 = 0;
	data.AUX2= 0;
}

void setup() {
	Serial.begin(250000); //Set the speed to 9600 bauds if you want.
	Serial.println("Starting up...");
	resetData();
	radio.begin();
	radio.setAutoAck(false);
	radio.setDataRate(RF24_250KBPS);

	radio.openReadingPipe(1,pipeIn);
	//we start the radio comunication
	radio.startListening();
  Serial.println("Finished setup!");
}

unsigned long lastRecvTime = 0;

void recvData() {
	while ( radio.available() ) {
		radio.read(&data, sizeof(UFODataPacket));
		lastRecvTime = millis(); // here we receive the data
	}
}

/**************************************************/

void loop() {
	recvData();
	unsigned long now = millis();
	//Here we check if we've lost signal, if we did we reset the values
	if ( now - lastRecvTime > 1000 ) {
		// Signal lost?
		resetData();
	}

	Serial.print("Throttle: "); Serial.print(data.throttle); Serial.print("\t");
	Serial.print("Yaw:      "); Serial.print(data.yaw);      Serial.print("\t");
	Serial.print("Pitch:    "); Serial.print(data.pitch);    Serial.print("\t");
	Serial.print("Roll:     "); Serial.print(data.roll);     Serial.print("\t");
	Serial.print("Aux1:     "); Serial.print(data.AUX1);     Serial.print("\t");
	Serial.print("Aux2:     "); Serial.print(data.AUX2);     Serial.print("\n");
}
