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
	functionality, hit the Arduino reset button


Project URL
	https://github.com/derickson2402/UFO-Controller.git

##############################################################################*/
#include <Servo.h>

// Select here the pins to use for the motor controllers and the e-stop button
#define ESTOP_PIN   2  // This can only be 2 or 3 on UNO boards
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

// Sets the motor outputs to the specified value
void setMotors(int speed);

/*############################################################################*/

Servo motorA;
Servo motorB;
Servo motorC;
Servo motorD;
bool estop;

void setup() {
	Serial.begin(9600);
	delay(1000);
	Serial.println("Press enter to calibrate the controllers");
	while(!Serial.available());
	Serial.read();

	// Connect to the ESC then run calibration
	pinMode(LED_PIN, OUTPUT);
	estop = false;
	digitalWrite(LED_PIN, HIGH);
	motorA.attach(MOTOR_PIN_A);
	motorB.attach(MOTOR_PIN_B);
	motorC.attach(MOTOR_PIN_C);
	motorD.attach(MOTOR_PIN_D);
	calibrate();
}

void loop() {
	// Receiving int 0-10 from Motors.pde. Scale to a PWM speed and update ESC
	int speed = PWM_MIN;
	if (Serial.available() > 0) {
		speed = Serial.parseInt();
	}
	double speedAsPWM = (double)((PWM_MAX - PWM_MIN) * speed) / 10.0;
	setMotors(PWM_MIN + (int)(speedAsPWM));
}

void calibrate() {
	Serial.println("Starting calibration routine, will take 10 seconds...");
	setMotors(PWM_MAX);
	delay(5000);
	setMotors(PWM_MIN);
	delay(5000);
	Serial.println("Finished calibration, now in manual control mode.");
}

void setMotors(int speed) {
	if (estop) {
		speed = PWM_MIN;
	}
	motorA.writeMicroseconds(speed);
	motorB.writeMicroseconds(speed);
	motorC.writeMicroseconds(speed);
	motorD.writeMicroseconds(speed);
}

void eStopInterrupt() {
	estop = true;
	digitalWrite(LED_PIN, LOW);
}