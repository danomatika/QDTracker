HeadOSC
=======

<p align="center">
	<img src="https://raw.github.com/danomatika/QDTracker/master/HeadOSC/screenshot.png"/>
</p>

computes simple kinect head approximation & sends position over OSC

*kinect 1 / xbox 360 kinect only*

2014 Dan Wilcox <danomatika@gmail.com> GPL v3

See <https://github.com/danomatika/QDTracker> for documentation


Algorithm
---------

<p align="center">
	<img src="https://raw.github.com/danomatika/QDTracker/master/HeadOSC/sketch.jpg"/>
</p>

* find person
* find highest point in person contour, ignore positions outside of person centroid += highestPointThreshold
* compute approximate head position by interpolating along line between person centroid & highest point 

Pros & Cons
-----------

pros:

* simple & fast

cons:

* only tracks 1 "person" aka sufficiently large thing
* requires empty space, distracted by other sufficiently large things
* not truely 3d, more like 2.5 since it's only from 1 perspective
* no orientation data (aka looking up, looking down, etc)

Build Requirements
------------------

* OpenFrameworks 0.8+
* addons (all included with the OF download):
  * ofxKinect 
  * ofxOpenCv
  * ofxOsc
  * ofxXmlSettigs

Settings
--------

see settings file comments for info: `data/settings.xml`

Key Commands
------------

* d: toggle display image type: threshold, RGB, depth, none
* =/-: increase/decrease kinect depth threshold
* s: save settings
* l: load settings
* R (shift+r): reset settings to defaults
* x: toggle x pos normalization
* y: toggle y pos normalization
* z: toggle z pos normalization

OSC
---

Sends a single OSC (Open Sound Control) message on every frame when a head position is approximated:

    /head x y z
    
x, y, & z are floats and can be normalized/scaled based on your chosen settings.
