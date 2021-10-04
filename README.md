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

Sends the OSC message: `/head x y z`

### OverHeadOSC

<p align="center">
	<img src="https://raw.githubusercontent.com/danomatika/QDTracker/master/OverHeadOSC/screenshot.png"/>
</p>

**overhead blob & highest point finding**

Sends the OSC message: `/overhead x y z`

Coordinate Data
---------------

Values, unless normalized and/or scaled (see settings):

* x: 0 - 640 (kinect depth image width)
* y: 0 - 480 (kinect depth image height)
* z: distance in centimeters (kinect nearClipping - farClipping)

Downloading
-----------

Git clone/download this repo into your OpenFrameworks `/apps` directory.

Installation & Build
--------------------

Overview:

1. Generate the project files for this folder using the OF ProjectGenerator
2. Build for your platform

### Generating Project Files

Project files are not included so you will need to generate the project files for your operating system and development environment using the OF ProjectGenerator which is included with the openFrameworks distribution.

To (re)generate project files for an existing project:

1. Click the "Import" button in the ProjectGenerator
2. Navigate to the project's parent folder ie. "apps/QDTracker", select the base folder for the example project ie. "HeadOSC", and click the Open button
3. Click the "Update" button

If everything went Ok, you should now be able to open the generated project and build/run the example.
