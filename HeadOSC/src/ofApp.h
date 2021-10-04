/*
 * HeadOSC, part of the Quick N Dirty Tracking system
 *
 * Copyright (c) 2014 Dan Wilcox <danomatika@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * See https://github.com/danomatika/QDTracker for documentation
 *
 * Largely adapted from the Kinect Titty Tracker:
 * http://danomatika.com/projects/kinect-titty-tracker
 *
 */
#pragma once

#include "ofMain.h"

#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxOsc.h"

#define SETTINGS "settings.xml"

class ofApp : public ofBaseApp {

	public:
		void setup();
		void update();
		void draw();
		void exit();
		void keyPressed(int key);
		
		void resetSettings();
		bool loadSettings(const std::string xmlFile=SETTINGS);
		bool saveSettings(const std::string xmlFile=SETTINGS);

		ofxKinect kinect;    // our RGB/depth camera of course
		ofxOscSender sender; // for sending head position

		// search images
		ofxCvGrayscaleImage depthImage; // grayscale depth image
		ofxCvGrayscaleImage depthDiff;  // thresholded person finder image

		// blob trackers
		ofxCvContourFinder 	personFinder;
		
		// positions
		ofRectangle person;     // found person centroid & size
		glm::vec3 highestPoint; // highest point in the person contour
		glm::vec3 head;         // found head position
		glm::vec3 headAdj;      // adjust head position after normalize & scale
		
		// settings
		int threshold; // person finder depth clipping threshold (0-255)
		unsigned int nearClipping, farClipping; // kinect clipping planes in cm
		unsigned int personMinArea, personMaxArea; // min and max area for the person finder
		unsigned int highestPointThreshold; // only consider highest points +- this & the person centroid
		float headInterpolation; // percentage to interpolate between person centroid & highest point (0-1)
		
		// normalize the head coordinates?
		bool bNormalizeX; // 0-kinect.width
		bool bNormalizeY; // 0-kinect.height
		bool bNormalizeZ; // nearClipping-farClipping
		
		// scale head coordinates?
		bool bScaleX, bScaleY, bScaleZ;
		float scaleXAmt, scaleYAmt, scaleZAmt; // how much to scale
		
		// live image to display
		enum DisplayImage {
			NONE = 0,
			THRESHOLD = 1,
			RGB = 2,
			DEPTH = 3
		} displayImage;
		
		// osc send destination
		std::string sendAddress;
		unsigned int sendPort;
		
		unsigned int kinectID; // which kinect to use
};
