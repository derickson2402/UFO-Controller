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
import processing.serial.*;
import controlP5.*;

Serial serialPort;
ControlP5 cp5;
int speed;

void setup() {
	// Must be same baud as in Motors.ino, find portName in Arduino IDE
	String portName = Serial.list()[0];
	serialPort = new Serial(this, portName 9600);

	// Configure basic window with a big slider for speed
	size(400, 800);
	cp5 = new ControlP5(this);
	speed = 0;
	cp5.addSlider("speed")
		.setPosition(50, 100)
		.setSize(300, 600)
		.setRange(0, 10)
		.setValue(0);
}

void draw() {
	fill(speed);
	serialPort.write(speed);
}