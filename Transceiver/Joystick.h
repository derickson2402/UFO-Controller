#ifndef JOYSTICK_H
#define JOYSTICK_H
/*##############################################################################

Date
	21 July 2022

Written By
	Dan Erickson (dan@danerick.com)

Description
	Joystick classes for plugging into Transceiver.ino. Each class is derived
	from the generic Joystick interface, so simply call update() to push fresh values to the UFODataPacket in Transceiver.ino.

	Only one Joystick should be defined at a time, and each should try to use
	digital pins 4-7 and analog pins 0-3 in order to avoid collisions with
	transmitter hardware.

	Please see the README in the 'Transceiver' folder for more.

Project URL
	https://github.com/derickson2402/UFO-Controller.git

##############################################################################*/

// Configure the following before including in any projects

// SimpleFLR:
#define RIGHT_PIN       4  // Digital pin for right button
#define FORWARD_PIN     5  // Digital pin for forward button
#define LEFT_PIN        6  // Digital pin for left button
#define THROTTLE_PIN    A0 // Analog pin for throttle potentiometer
#define ROTATE_SPEED    25 // Left/Right rotation speed from 0-127
#define MOVE_SPEED      25 // Forward movement speed from 0-127

// GyroMotion
// TODO: Implement GyroMotion controller #1

/*############################################################################*/

/* Data packet to be transmitted. Follows North East Down (NED) reference frame
 * where x is forward (north), y is right (east), and z is down. Therefore
 * roll, pitch, and yaw are defined as rotation about these axes in respective
 * order. 127 is no rotation, >127 is positive and <127 is negative rotation.
 * The mapping is:
 * 
 * North   ->   x   ->   forward   ->   roll
 * East    ->   y   ->   right     ->   pitch
 * Down    ->   z   ->   down      ->   yaw
 * 
 * The nRF24L01 chip can only send 32 bytes at a time, so we get up to 32 8 bit
 * channels. Currently 6 are used, corresponding to first 6 channels in
 * MultiWii/config.h. This means it is possible to send extra arbitrary data
 * over the air
 */
struct UFODataPacket {
	byte throttle; // Amount of power supplied to motor, higher is more
	byte yaw;      // Rotation about z axis (look left/right)
	byte pitch;    // Rotation about y axis (look up/down)
	byte roll;     // Rotation about x axis (roll to the sides)
	byte AUX1;     // Arbitrary byte 1 (currently unused)
	byte AUX2;     // Arbitrary byte 2 (currently unused)
	/* Reset the values to neutral defaults */
	void clear() {
		throttle = AUX1 = AUX2 = 0;
		yaw = pitch = roll = 127;
	}
};

/* Virtual interface for various Joystick control schemes */
class Joystick {
public:
	/* Updates the original data packet with values from the Joystick */
	virtual UFODataPacket update() = 0;

	/* Open a new Joystick */
	explicit Joystick(UFODataPacket & data) : data(data) { }

protected:
	volatile UFODataPacket & data; // Reference to data packet that caller uses
};

/* Simple Joystick with 3 buttons for forward, left, and right, and a
 * potentiometer for throttle */
class SimpleFLR : public Joystick {
public:
	explicit SimpleFLR(UFODataPacket & data) : Joystick(data) {
		pinMode(LEFT_PIN,    INPUT_PULLUP);
		pinMode(RIGHT_PIN,   INPUT_PULLUP);
		pinMode(FORWARD_PIN, INPUT_PULLUP);
	}
	UFODataPacket update() {
		// Calculate values from button presses
		byte newYaw = 127;
		byte left = digitalRead(LEFT_PIN);
		byte right = digitalRead(RIGHT_PIN);
		if (left == right) {
			newYaw = 127; // Don't move if none or both buttons pressed
		} else if (left == LOW) {
			newYaw = 127 - ROTATE_SPEED; // Left button pressed
		} else {
			newYaw = 127 + ROTATE_SPEED; // Right button pressed
		}
		byte newPitch = (digitalRead(FORWARD_PIN) == LOW)
				? (127 - MOVE_SPEED) : 127;

		// Update the data packet
		data.throttle = map(analogRead(THROTTLE_PIN), 0, 1024, 0, 255);
		data.yaw      = newYaw;
		data.pitch    = newPitch;
		data.roll     = 127;
		data.AUX1     = 0;
		data.AUX2     = 0;
	}
};

class GyroMotion : Joystick {
	// TODO: Implement GyroMotion #1
};
#endif
