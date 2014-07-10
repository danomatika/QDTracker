#pragma once

#include "ofMain.h"

#include "ofxOpenCv.h"
#include "ofxKinect.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();
		void keyPressed(int key);

		ofxKinect kinect;	// our RGB/depth camera of course

		// search images
		ofxCvGrayscaleImage depthImage;	// grayscale depth image
		ofxCvGrayscaleImage depthDiff;	// thresholded person finder image

		// blob trackers
		ofxCvContourFinder 	personFinder;
		
		// positions
		ofRectangle person;		// found person centroid & size
		ofPoint highestPoint;	// highest point in the person contour
		ofPoint head;			// found head position
		
		// settings
		int threshold;	// person finder depth clipping threshold (0-255)
		unsigned int nearClipping, farClipping; // kinect clipping planes in cm
		unsigned int highestPointThreshold;	// only consider highest points +- this & the person centroid
		float headInterpolation; // ercentage to interpolate between person centroid & highest point (0-1)
		
		// normalize the head coordinates?
		bool bNormalizeX; // 0-kinect.width
		bool bNormalizeY; // 0-kinect.height
		bool bNormalizeZ; // nearClipping-farClipping
		
		// scale head coordinates?
		bool bScaleX, bScaleY, bScaleZ;
		float scaleXAmt, scaleYAmt, scaleZAmt; // how much to scale
		
		bool bShowRGB; // show RGB image (true) or show depth image (false)?
};
