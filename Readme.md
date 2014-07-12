QDTracker
=========

Quick N Dirty Tracker "head" positional tracker for OpenFrameworks using kinect 1 / xbox kinect & OSC (Open Sound Control)

2014 Dan Wilcox <danomatika@gmail.com> GPL v3

See <https://github.com/danomatika/QDTracker> for documentation

Components
----------

### HeadOSC

<p align="center">
	<img src="https://raw.githubusercontent.com/danomatika/QDTracker/master/HeadOSC/screenshot.png"/>
</p>

**front facing head approximation**

Sends the OSC message: /head x y z

### OverHeadOSC

<p align="center">
	<img src="https://raw.githubusercontent.com/danomatika/QDTracker/master/OverHeadOSC/screenshot.png"/>
</p>

**overhead blob & highest point finding**

Sends the OSC message: /overhead x y z

Coordinate Data
---------------

Values, unless normalized and/or scaled (see settings):

* x: 0 - 640 (kinect depth image width)
* y: 0 - 480 (kinect depth image height)
* z: distance in centimeters (kinect nearClipping - farClipping)


Downloading
-----------

Git clone/download this repo into your OpenFrameworks `/apps` directory.


