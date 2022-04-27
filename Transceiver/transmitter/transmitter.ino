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

// Create a unique pipe out. The receiver must use the same unique code
const uint64_t pipeOut = 0xE8E8F0F0E1LL;

RF24 radio(9, 10); // select CSN pin

// The sizeof this struct should not exceed 32 bytes
// This gives us up to 32 8 bits channels
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
	// This are the start values of each channel
	// Throttle is 0 in order to stop the motors

	data.throttle = 0;
	data.yaw = 127;
	data.pitch = 127;
	data.roll = 127;
	data.AUX1 = 0;
	data.AUX2 = 0;
}

void setup() {
	//Start everything up
  Serial.begin(250000);
  Serial.print("Starting up...");
	radio.begin();
	radio.setAutoAck(false);
	// This value must match the receiver or they can't communicate
	radio.setDataRate(RF24_250KBPS);
	radio.openWritingPipe(pipeOut);
	resetData();
  Serial.println(" Done with setup!");
}

/**************************************************/

// Returns a corrected value for a joystick position that takes into account
// the values of the outer extents and the middle of the joystick range.
int mapJoystickValues(int val, int lower, int middle, int upper, bool reverse) {
	val = constrain(val, lower, upper);
	if ( val < middle )
		val = map(val, lower, middle, 0, 128);
	else
		val = map(val, middle, upper, 128, 255);
	return ( reverse ? 255 - val : val );
}

void loop() {
	// The calibration numbers used here should be measured 
	// for your joysticks till they send the correct values.
	data.throttle = mapJoystickValues( analogRead(A0), 13, 524, 1015, true );
	data.yaw      = mapJoystickValues( analogRead(A1), 1, 505, 1020, true );
	data.pitch    = mapJoystickValues( analogRead(A2), 12, 544, 1021, true );
	data.roll     = mapJoystickValues( analogRead(A3), 34, 522, 1020, true );
	data.AUX1     = digitalRead(4); //The 2 toggle switches
	data.AUX2     = digitalRead(5);

  Serial.print("Throttle: "); Serial.print(data.throttle); Serial.print("\t");
  Serial.print("Yaw:      "); Serial.print(data.yaw);      Serial.print("\t");
  Serial.print("Pitch:    "); Serial.print(data.pitch);    Serial.print("\t");
  Serial.print("Roll:     "); Serial.print(data.roll);     Serial.print("\t");
  Serial.print("Aux1:     "); Serial.print(data.AUX1);     Serial.print("\t");
  Serial.print("Aux2:     "); Serial.print(data.AUX2);     Serial.print("\n");
  radio.write(&data, sizeof(UFODataPacket));
}
