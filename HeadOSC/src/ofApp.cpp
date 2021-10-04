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
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofSetVerticalSync(true);
	
	// settings
	resetSettings();
	loadSettings();
	
	// setup kinect
	kinect.init(false); // no IR image
	kinect.setRegistration(true);
	kinect.setDepthClipping(nearClipping, farClipping);
	kinect.open(kinectID);
	
	// setup cv
	depthImage.allocate(kinect.width, kinect.height);
	depthDiff.allocate(kinect.width, kinect.height);
}

//--------------------------------------------------------------
void ofApp::update(){
	ofBackground(0, 0, 0);

	kinect.update();
	if(kinect.isFrameNew()) { // dont bother if the frames aren't new
	
		// find person-sized blobs
		depthImage.setFromPixels(kinect.getDepthPixels());
		depthDiff = depthImage;
		depthDiff.threshold(threshold);
		depthDiff.updateTexture();
		personFinder.findContours(depthDiff, personMinArea, personMaxArea, 1, false);
		
		// found person-sized blob?
		if(personFinder.blobs.size() > 0) {
			ofxCvBlob &blob = personFinder.blobs[0];
			person.position = blob.centroid;
			person.width = blob.boundingRect.width;
			person.height = blob.boundingRect.height;
			
			// find highest contour point (actually the lowest value since top is 0)
			int height = INT_MAX;
			for(int i = 0; i < blob.pts.size(); ++i) {
				auto &p = blob.pts[i];
				if(p.y < height &&
				  (p.x > person.x-highestPointThreshold && p.x < person.x+highestPointThreshold)) {
					highestPoint = blob.pts[i];
					height = blob.pts[i].y;
				}
			}
			
			// compute rough head position between centroid and highest point
			head = glm::vec3(person.position.x*(1-headInterpolation) + highestPoint.x*headInterpolation,
							 person.position.y*(1-headInterpolation) + highestPoint.y*headInterpolation,
							 kinect.getDistanceAt(head));
			//head = person.position.getInterpolated(highestPoint, headInterpolation);
			//head.z = kinect.getDistanceAt(head);
			headAdj = head;
			
			// normalize values
			if(bNormalizeX) headAdj.x = ofMap(head.x, 0, kinect.width, 0, 1);
			if(bNormalizeY) headAdj.y = ofMap(head.y, 0, kinect.height, 0, 1);
			if(bNormalizeZ) headAdj.z = ofMap(head.z, kinect.getNearClipping(), kinect.getFarClipping(), 0, 1);
			
			// scale values
			if(bScaleX) headAdj.x *= scaleXAmt;
			if(bScaleY) headAdj.y *= scaleYAmt;
			if(bScaleZ) headAdj.z *= scaleZAmt;
			
			// send head position
			ofxOscMessage message;
			message.setAddress("/head");
			message.addFloatArg(headAdj.x);
			message.addFloatArg(headAdj.y);
			message.addFloatArg(headAdj.z);
			sender.sendMessage(message);
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw(){

	// draw display image
	ofSetColor(255);
	switch(displayImage) {
		case THRESHOLD:
			depthDiff.draw(0, 0);
			break;
		case RGB:
			kinect.draw(0, 0);
			break;
		case DEPTH:
			kinect.drawDepth(0, 0);
			break;
		default: // NONE
			break;
	}

	if(personFinder.blobs.size() > 0) {

		// draw person finder
		ofSetLineWidth(2.0);
		personFinder.draw(0, 0, 640, 480);
	
		// purple - found person centroid
		ofFill();
		ofSetColor(255, 0, 255);
		ofDrawRectangle(person.position, 10, 10);
		
		// gold - highest point
		ofFill();
		ofSetColor(255, 255, 0);
		ofDrawRectangle(highestPoint, 10, 10);
		
		// light blue - "head" position
		ofFill();
		ofSetColor(0, 255, 255);
		ofDrawRectangle(head.x, head.y, 10, 10);
		
		// draw current position
		ofSetColor(255);
		ofDrawBitmapString(ofToString(headAdj.x, 2)+" "+ofToString(headAdj.y, 2)+" "+ofToString(headAdj.z, 2), 12, 12);
	}
	
	ofSetColor(255);
	ofDrawBitmapString("threshold " + ofToString(threshold), 12, 24);
}

//--------------------------------------------------------------
void ofApp::exit(){
	kinect.close();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch(key) {
		
		case '-':
			threshold--;
			if(threshold < 0) threshold = 0;
			break;
			
		case '=':
			threshold++;
			if(threshold > 255) threshold = 255;
			break;
			
		case 'x':
			bNormalizeX = !bNormalizeX;
			break;
			
		case 'y':
			bNormalizeY = !bNormalizeY;
			break;
			
		case 'z':
			bNormalizeZ = !bNormalizeZ;
			break;
			
		case 'd': {
			// increment enum
			int d = (int)displayImage;
			d++;
			if(d > DEPTH) {
				d = (int)NONE;
			}
			displayImage = (DisplayImage)d;
			break;
		}
			
		case 's':
			saveSettings();
			break;
			
		case 'l':
			loadSettings();
			break;
			
		case 'R':
			resetSettings();
			break;
	}
}

//--------------------------------------------------------------
void ofApp::resetSettings() {
	
	threshold = 160;
	nearClipping = 500;
	farClipping = 4000;
	personMinArea = 3000;
	personMaxArea = 640*480*0.5;
	highestPointThreshold = 50;
	headInterpolation = 0.6;
	
	bNormalizeX = false;
	bNormalizeY = false;
	bNormalizeZ = false;
	
	bScaleX = false;
	bScaleY = false;
	bScaleZ = false;
	
	scaleXAmt = 1.0;
	scaleYAmt = 1.0;
	scaleZAmt = 1.0;
	
	displayImage = THRESHOLD;
	kinectID = 0;
	
	sendAddress = "127.0.0.1";
	sendPort = 9000;

	// setup osc
	sender.setup(sendAddress, sendPort);
}

//--------------------------------------------------------------
bool ofApp::loadSettings(const std::string xmlFile) {

	ofXml xml;
	if(!xml.load(xmlFile)) {
		ofLogWarning() << "Couldn't load settings ";
		return false;
	}

	ofXml root = xml.getChild("settings");
	if(!root) {
		ofLogWarning() << "Couldn't load settings, missing root \"settings\" tag";
		return false;
	}

	kinectID = root.getChild("kinectID").getUintValue();
	displayImage = (DisplayImage)root.getChild("displayImage").getUintValue();

	ofXml tracking = root.getChild("tracking");
	if(tracking) {
		threshold = tracking.getChild("threshold").getIntValue();
		nearClipping = tracking.getChild("nearClipping").getUintValue();
		farClipping = tracking.getChild("farClipping").getUintValue();
		personMinArea = tracking.getChild("personMinArea").getUintValue();
		personMaxArea = tracking.getChild("personMaxArea").getUintValue();
		highestPointThreshold = tracking.getChild("highestPointThreshold").getUintValue();
		headInterpolation = tracking.getChild("headInterpolation").getFloatValue();
	}

	ofXml normalize = root.getChild("normalize");
	if(normalize) {
		bNormalizeX = normalize.getChild("bNormalizeX").getBoolValue();
		bNormalizeY = normalize.getChild("bNormalizeY").getBoolValue();
		bNormalizeZ = normalize.getChild("bNormalizeZ").getBoolValue();
	}

	ofXml scale = root.getChild("scale");
	if(scale) {
		bScaleX = scale.getChild("bScaleX").getBoolValue();
		bScaleY = scale.getChild("bScaleY").getBoolValue();
		bScaleZ = scale.getChild("bScaleZ").getBoolValue();
		scaleXAmt = scale.getChild("scaleXAmt").getFloatValue();
		scaleYAmt = scale.getChild("scaleYAmt").getFloatValue();
		scaleZAmt = scale.getChild("scaleZAmt").getFloatValue();
	}

	ofXml osc = root.getChild("osc");
	if(osc) {
		sendAddress = osc.getChild("sendAddress").getValue();
		sendPort = osc.getChild("sendPort").getUintValue();
	}
	
	// setup kinect
	kinect.setDepthClipping(nearClipping, farClipping);
	
	// setup osc
	sender.setup(sendAddress, sendPort);
	
	return true;
}

//--------------------------------------------------------------
bool ofApp::saveSettings(const std::string xmlFile) {
	
	ofXml xml;

	ofXml root = xml.appendChild("settings");
	root.appendChild("kinectID").set(kinectID);
	root.appendChild("displayImage").set(displayImage);

	ofXml tracking = root.appendChild("tracking");
	tracking.appendChild("threshold").set(threshold);
	tracking.appendChild("nearClipping").set(nearClipping);
	tracking.appendChild("farClipping").set(farClipping);
	tracking.appendChild("personMinArea").set(personMinArea);
	tracking.appendChild("personMaxArea").set(personMaxArea);
	tracking.appendChild("highestPointThreshold").set(highestPointThreshold);
	tracking.appendChild("headInterpolation").set(headInterpolation);

	ofXml normalize = root.appendChild("normalize");
	normalize.appendChild("bNormalizeX").set(bNormalizeX);
	normalize.appendChild("bNormalizeY").set(bNormalizeY);
	normalize.appendChild("bNormalizeZ").set(bNormalizeZ);

	ofXml scale = root.appendChild("scale");
	scale.appendChild("bScaleX").set(bScaleX);
	scale.appendChild("bScaleY").set(bScaleY);
	scale.appendChild("bScaleZ").set(bScaleZ);
	scale.appendChild("scaleXAmt").set(scaleXAmt);
	scale.appendChild("scaleYAmt").set(scaleYAmt);
	scale.appendChild("scaleZAmt").set(scaleZAmt);

	ofXml osc = root.appendChild("osc");
	osc.appendChild("sendAddress").set(sendAddress);
	osc.appendChild("sendPort").set(sendPort);

	if(!xml.save(xmlFile)) {
		ofLogWarning() << "Couldn't save settings";
			return false;
	}
	return true;
}
