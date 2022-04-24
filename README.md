# UFO-Controller

This summer, we want to build custom racing drones.
Eventually, we'd like to hook up FPV cameras, GPS units, and some other fun gizmos so that we can do cool stuff like FPV racing, autonomous path following with GPS, and smart object detection and wayfinding using computer vision.

Right now everything is in very early stages, so check back later for more!

## Included Files

Folder | Description
---|---|
```Arduino-ISP```     | Copied from Arduino built-in example 11. Allows an Uno to program a Nano over ISP
```Wiring-Diagrams``` | Sub-Directory of PCB designs, such as main-board for drone
```toxiclibs```       | Download of Toxiclibs ```geom``` and ```processing``` libraries
```Hardware-Tests```  | Various programs for verifying that electronic components are working

## Hardware Tests and Visualization with Processing

In the ```Hardware-Tests``` folder are some basic programs for testing that various components are working.
Some of these require Processing]() (and the included ```toxiclibs``` libraries).
Simply install the program and then open the relevant ```.pde``` file to run the test.

## Configuring VS Code IDE

If you are like me, you really hate the default Arduino IDE and you would rather use VS Code.
To do this:

1. Install the Arduino VS Code extension
2. Choose AVR programmer and Arduino Nano board
3. In ```c_cpp_properties.json```, update ```compilerPath```, ```includePath```, and ```forcedInclude``` so your machine's section matches that of the Arduino (basically tell intellisense to use the Arduino's compiler)
4. Add ```USBCON``` to ```defines``` in ```c_cpp_properties.json```
5. If you need 3rd-party libraries, put them in ```src``` and symlink ```src``` to the root folder for your code and update your includes accordingly

## Resources

- https://create.arduino.cc/projecthub/akarsh98/diy-arduino-based-quadcopter-drone-948153
- https://www.hackster.io/robocircuits/arduino-quadcopter-e618c6
- https://howtomechatronics.com/projects/diy-arduino-rc-transmitter/
- https://www.modelflight.com.au/blog/a-guide-to-understanding-lipo-batteries
- https://www.instructables.com/DJi-F450-Quadcopter-How-to-Build-Home-Built/
- http://www.rctutor.org/the-parts-list.html
- [Read-only Amazon link for parts](https://www.amazon.com/hz/wishlist/ls/1BKLT1DHMK0VX?ref_=wl_share)
