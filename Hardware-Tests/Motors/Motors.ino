/*##############################################################################

Date
	20 July 2022

Written By
	Dan Erickson (dan@danerick.com)

Description
	Hardware testing for drone motors. Tested with Readytosky 2212 920KV
	Brushless motors, connected with Readytosky 2-4S 40A Brushless ESC. Should
	work with any 5V servo-style speed controller.

	Turn motors on/off with button, change speed with potentiometer. Red LED on
	means that motors are armed. Before deploying, make sure to remove
	propellers and to modify pin assignments if needed.

Project URL
	https://github.com/derickson2402/UFO-Controller.git

##############################################################################*/

#include <Servo.h>

// Select here the pins to use for the motor controllers and the e-stop button
#define ESTOP_PIN   2  // Must be an interrupt pin, either 2 or 3 on UNO
                       // boards. Pullup resistor is configured in code, such
                       // that pushing should bring voltage to 0V
#define SPEED_PIN   A0 // Must be analog read pin with 1024 bit resolution for
                       // analog potentiometer
#define LED_PIN     7  // Red LED indicating if motors armed
#define MOTOR_PIN_A 8  // Each ESC should be connected to 5V and the signal wire
#define MOTOR_PIN_B 9  // should go to the specified pin
#define MOTOR_PIN_C 10
#define MOTOR_PIN_D 11

// You probably should leave these values alone. If you do need to change them,
// you will also have to alter the Multiwii PWM code. Both are represented as
// time lengths in microseconds
#define PWM_MIN 1000
#define PWM_MAX 2000

// Do not modify this value as it may break the E-Stop. This controls the
// update frequency of the motors and therefore how frequently the E-Stop is
// able to update motors
#define ESC_FRQ 100

/*############################################################################*/

// Sets the motor outputs to the specified value, unless e-stop is engaged
void setMotors(int speed);

// Immediately set motors to low. Disconnecting the signal won't necessarily
// stop them from moving, so this will hopefully be the safest option. Press
// again to re-enable the motors
void eStopInterrupt();

/*############################################################################*/

Servo motorA;
Servo motorB;
Servo motorC;
Servo motorD;
volatile bool estop; // Is the e-stop triggered?

void setup() {
	// Set up E-Stop
	pinMode(ESTOP_PIN, INPUT_PULLUP);
	estop = true;
	attachInterrupt(digitalPinToInterrupt(ESTOP_PIN), eStopInterrupt, FALLING);
	pinMode(LED_PIN, OUTPUT);
	digitalWrite(LED_PIN, LOW);

	// Connect to the ESC's
	Serial.begin(9600);
	motorA.attach(MOTOR_PIN_A);
	motorB.attach(MOTOR_PIN_B);
	motorC.attach(MOTOR_PIN_C);
	motorD.attach(MOTOR_PIN_D);
	delay(10000);
}

void loop() {
	// Check status of E-Stop and update warning light
	digitalWrite(LED_PIN, (estop ? LOW : HIGH));

	// Calculate set speed of motor controllers from potentiometer
	int speed = analogRead(SPEED_PIN);
	int speedSetPoint = map(speed, 0, 1024, PWM_MIN, PWM_MAX);

	// Set the speed of the motor controllers
	setMotors(speedSetPoint);
	delay(ESC_FRQ);
}

void setMotors(int speed) {
	if (estop) {
		speed = PWM_MIN;
		Serial.println("E-Stop");
	} else {
		Serial.println(speed);
	}
	motorA.writeMicroseconds(speed);
	motorB.writeMicroseconds(speed);
	motorC.writeMicroseconds(speed);
	motorD.writeMicroseconds(speed);
}

void eStopInterrupt() {
	estop = !estop;
}
