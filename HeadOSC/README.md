HeadOSC
=======

<p align="center">
	<img src="https://raw.github.com/danomatika/QDTracker/master/HeadOSC/screenshot.png"/>
</p>

computes simple kinect head approximation & sends position over OSC

*kinect 1 / xbox 360 kinect only*

2014-2021 Dan Wilcox <danomatika@gmail.com> GPL v3

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

* OpenFrameworks
* addons (all included with the OF download):
  * ofxKinect 
  * ofxOpenCv
  * ofxOsc

Settings
--------

XML settings file tags and sections, ex. `data/settings.xml`

general
* kinectID: which kinect ID to open (note: doesn't change when reloading); int 
* displayImage: display image: 0 - none, 1 - threshold, 2 - RGB, 3 - depth

tracking
* threshold: person finder depth clipping threshold; int 0 - 255
* nearClipping: kinect near clipping plane in cm; int
* farClipping: kinect far clipping plane in cm; int
* personMinArea: minimum area to consider when looking for person blobs; int
* personFarArea: maximum area to consider when looking for person blobs; int
* highestPointThreshold: only consider highest points +- this & the person centroid; int
* headInterpolation: percentage to interpolate between person centroid & highest point; float 0 - 1

normalize
* bNormalizeX: normalize head position X coord, enable/disable; bool 0 or 1
* bNormalizeY: normalize head position Y coord, enable/disable; bool 0 or 1
* bNormalizeZ: normalize head position Z coord, enable/disable; bool 0 or 1

scale
* bScaleX: scale head position X coord, performed after normalization, enable/disable; bool 0 or 1
* bScaleY: scale head position Y coord, performed after normalization, enable/disable; bool 0 or 1
* bScaleZ: scale head position Z coord, performed after normalization, enable/disable; bool 0 or 1
* scaleXAmt: scale amount for X coord
* scaleYAmt: scale amount for Y coord
* scaleZAmt: scale amount for Z coord

osc
* sendAddress: host destination address
* sendPort: host destination port

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
