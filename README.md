# UFO-Controller

This summer, several of us wanted to build some quadcopter drones (what can I say, we're a house full of bored engineers :smiley:).
Eventually, we'd like to hook up FPV cameras, GPS units, and some other fun gizmos so that we can do cool stuff like FPV racing, autonomous path following with GPS, and smart object detection and wayfinding using computer vision.
Currently the project is in early stages, but I am documenting all the software as I go and when we have everything working I will make sure to include assembly instructions as well.

Much of the transmitter code is extended from [akarsh98](https://github.com/akarsh98) on GitHub and [this guide on PPM via Arduino](https://quadmeup.com/generate-ppm-signal-with-arduino/). Thanks akarsh and Pawel!

## Included Files

Some of the code used here is from projects from almost a decade ago, so I made sure to grab a copy of all the libraries and such that are used here.
Each folder contains some component of either testing/building the drone, and each comes with its own README and any dependencies.

Folder | Description
---|---|
```Arduino-ISP```     | Copied from Arduino IDE built-in example 11. Allows an Uno to program a Nano over ISP
```Wiring-Diagrams``` | PCB designs and board pinouts for transceiver, flight controller, etc for KiCad 6.0. Includes KiCad files for MPU breakout board.
```Hardware-Tests```  | Various programs for verifying that individual electronic components are working
```MultiWii```        | Flight control software from [MultiWii.com](http://www.multiwii.com/)
```MultiWii-GUI```    | Configuration GUI for ```MultiWii```
```Transceiver```     | Instructions for using pair of Arduino as radio transmitter to drone

## Testing/Verifying Hardware

In the ```Hardware-Tests``` folder are some basic programs for testing that various components are working.
There is another README in the folder which will walk you through everything.
I highly recommend you verify all of your hardware is working correctly BEFORE trying everything together, since there is nothing worse than spending hours on a "software" bug that doesn't actually exist.
Plus if you're like me you bought the cheapest stuff you could find and there's a good chance it won't all work right :smiley:.

## Configuring VS Code IDE

If you are like me, you really hate the default Arduino IDE and you would rather use VS Code.
To do this:

1. Install the Arduino VS Code extension
2. Choose AVR programmer and Arduino Nano board
3. In ```c_cpp_properties.json```, update ```compilerPath```, ```includePath```, and ```forcedInclude``` so your machine's section matches that of the Arduino (basically tell intellisense to use the Arduino's compiler)
4. Add ```USBCON``` to ```defines``` in ```c_cpp_properties.json```
5. If you need 3rd-party libraries, put them in ```src``` in the root of this repo and symlink ```src``` in your code root folder to it (Arduino compiler should recognize the ```src``` folder automatically)

## Resources

- https://create.arduino.cc/projecthub/akarsh98/diy-arduino-based-quadcopter-drone-948153
- https://www.hackster.io/robocircuits/arduino-quadcopter-e618c6
- https://howtomechatronics.com/projects/diy-arduino-rc-transmitter/
- https://www.modelflight.com.au/blog/a-guide-to-understanding-lipo-batteries
- https://www.instructables.com/DJi-F450-Quadcopter-How-to-Build-Home-Built/
- http://www.rctutor.org/the-parts-list.html
- [Read-only Amazon link for parts](https://www.amazon.com/hz/wishlist/ls/1BKLT1DHMK0VX?ref_=wl_share)
- [PPM communication via Arduino](https://quadmeup.com/generate-ppm-signal-with-arduino/)
