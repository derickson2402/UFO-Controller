/*##############################################################################

Date
	20 July 2022

Written By
	Dan Erickson (dan@danerick.com)

Description
	Hardware testing for drone motors. Tested with Readytosky 2212 920KV
	Brushless motors, connected with Readytosky 2-4S 40A Brushless ESC. Should
	work with any 5V servo-style speed controller.

	E-Stop button will override and shut down controllers. To resume
	functionality, press button again. Once calibration is finished, use
	potentiometer to control motor speed.

Project URL
	https://github.com/derickson2402/UFO-Controller.git

##############################################################################*/
#include <Servo.h>

// Select here the pins to use for the motor controllers and the e-stop button
#define ESTOP_PIN   2  // Must be an interrupt pin, either 2 or 3 on UNO boards
#define SPEED_PIN   A0 // Must be analog read pin with 1024 bit resolution
#define LED_PIN     7
#define MOTOR_PIN_A 8
#define MOTOR_PIN_B 9
#define MOTOR_PIN_C 10
#define MOTOR_PIN_D 11

// You probably should leave these values alone. If you do need to change them,
// you will also have to alter the Multiwii PWM code
#define PWM_MIN 1000
#define PWM_MAX 2000

/*############################################################################*/

// Run a calibration sequence which sets the ESC high then low. Takes 10 seconds
void calibrate();

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
volatile bool estop;  // Is the e-stop triggered?

void setup() {
	// Set up E-Stop
	pinMode(ESTOP_PIN, INPUT_PULLUP);
	estop = false;
	attachInterrupt(digitalPinToInterrupt(ESTOP_PIN), eStopInterrupt, FALLING);
	pinMode(LED_PIN, OUTPUT);
	digitalWrite(LED_PIN, HIGH);

	// Connect to the ESC then run calibration
	Serial.begin(9600);
	motorA.attach(MOTOR_PIN_A);
	motorB.attach(MOTOR_PIN_B);
	motorC.attach(MOTOR_PIN_C);
	motorD.attach(MOTOR_PIN_D);
	delay(5000);
	calibrate();
}

void loop() {
	// Calculate set speed of motor controllers from potentiometer
	int speed = analogRead(SPEED_PIN);
	int speedSetPoint = map(speed, 0, 1024, PWM_MIN, PWM_MAX);

	// Set the speed of the motor controllers
	setMotors(speedSetPoint);
	delay(500);
}

void calibrate() {
	Serial.println("Starting calibration routine, will take 10 seconds...");
	// Use for loops like this so that E-Stop will work
	for (int i = 0; i < 10; ++i) {
		setMotors(PWM_MAX);
		delay(500);
	}
	for (int i = 0; i < 10; ++i) {
		setMotors(PWM_MIN);
		delay(500);
	}
	Serial.println("Finished calibration, now in manual control mode.");
}

void setMotors(int speed) {
	if (estop) {
		speed = PWM_MIN;
		Serial.println("Warning: E-Stop engaged");
	}
	Serial.print("Setting motors to: "); Serial.println(speed);
	motorA.writeMicroseconds(speed);
	motorB.writeMicroseconds(speed);
	motorC.writeMicroseconds(speed);
	motorD.writeMicroseconds(speed);
}

void eStopInterrupt() {
	estop = !estop;
	digitalWrite(LED_PIN, (estop ? LOW : HIGH));
}
