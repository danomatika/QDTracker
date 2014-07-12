/*
 * OverHeadOSC, part of the Quick N Dirty Tracking system
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

#include "XmlSettings.h"

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
		depthImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
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

			// find the closest point in the person blob
			overhead = findNearestPoint(kinect.getDepthPixelsRef(), person);
			overhead.z = kinect.getDistanceAt(overhead.x, overhead.y);
			overheadAdj = overhead;
			
			// normalize values
			if(bNormalizeX) overheadAdj.x = ofMap(overhead.x, 0, kinect.width, 0, 1);
			if(bNormalizeY) overheadAdj.y = ofMap(overhead.y, 0, kinect.height, 0, 1);
			if(bNormalizeZ) overheadAdj.z = ofMap(overhead.z, kinect.getNearClipping(), kinect.getFarClipping(), 0, 1);
			
			// scale values
			if(bScaleX) overheadAdj.x *= scaleXAmt;
			if(bScaleY) overheadAdj.y *= scaleYAmt;
			if(bScaleZ) overheadAdj.z *= scaleZAmt;
			
			// send head position
			ofxOscMessage message;
			message.setAddress("/overhead");
			message.addFloatArg(overheadAdj.x);
			message.addFloatArg(overheadAdj.y);
			message.addFloatArg(overheadAdj.z);
			sender.sendMessage(message);
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw(){

	// draw RGB or IR image
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
		ofRect(person.position, 10, 10);
		
		// light blue - overhead "head" position
		ofFill();
		ofSetColor(0, 255, 255);
		ofRect(overhead.x, overhead.y, 10, 10);
		
		// draw current position
		ofSetColor(255);
		ofDrawBitmapString(ofToString(overheadAdj.x, 2)+" "+ofToString(overheadAdj.y, 2)+" "+ofToString(overheadAdj.z, 2), 12, 12);
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
	personMinArea = 5;
	personMaxArea = 3000;
	
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
bool ofApp::loadSettings(const string xmlFile) {
	ofxXmlSettings xml;
	if(!xml.loadFile(xmlFile)) {
		ofLogWarning() << "Couldn't load settings: "
			<< xml.doc.ErrorRow() << ", " << xml.doc.ErrorCol()
			<< " " << xml.doc.ErrorDesc();
			return false;
	}
	
	xml.pushTag("settings");
		
		displayImage = (DisplayImage)xml.getValue("displayImage", displayImage);
		kinectID = xml.getValue("kinectID", (int)kinectID);
		
		xml.pushTag("tracking");
			threshold = xml.getValue("threshold", threshold);
			nearClipping = xml.getValue("nearClipping", (int)nearClipping);
			farClipping = xml.getValue("farClipping", (int)farClipping);
			personMinArea = xml.getValue("personMinArea", (int)personMinArea);
			personMaxArea = xml.getValue("personMaxArea", (int)personMaxArea);
		xml.popTag();
		
		xml.pushTag("normalize");
			bNormalizeX = xml.getValue("bNormalizeX", bNormalizeX);
			bNormalizeY = xml.getValue("bNormalizeY", bNormalizeY);
			bNormalizeZ = xml.getValue("bNormalizeZ", bNormalizeZ);
		xml.popTag();
		
		xml.pushTag("scale");
			bScaleX = xml.getValue("bScaleX", bScaleX);
			bScaleY = xml.getValue("bScaleY", bScaleY);
			bScaleZ = xml.getValue("bScaleZ", bScaleZ);
			scaleXAmt = xml.getValue("scaleXAmt", scaleXAmt);
			scaleYAmt = xml.getValue("scaleYAmt", scaleYAmt);
			scaleZAmt = xml.getValue("scaleZAmt", scaleZAmt);
		xml.popTag();
		
		xml.pushTag("osc");
			sendAddress = xml.getValue("sendAddress", sendAddress);
			sendPort = xml.getValue("sendPort", (int)sendPort);
		xml.popTag();
		
	xml.popTag();
	
	// setup kinect
	kinect.setDepthClipping(nearClipping, farClipping);
	
	// setup osc
	sender.setup(sendAddress, sendPort);
	
	return true;
}

//--------------------------------------------------------------
bool ofApp::saveSettings(const string xmlFile) {
	
	XmlSettings xml;
	
	xml.addTag("settings");
	xml.pushTag("settings");
		
		xml.addComment(" general settings ");
		xml.addComment(" which kinect ID to open (note: doesn't change when reloading); int ");
		xml.addValue("kinectID", (int)kinectID);
		xml.addComment(" display image: 0 - none, 1 - threshold, 2 - RGB, 3 - depth");
		xml.addValue("displayImage", displayImage);
		
		xml.addComment(" tracking settings ");
		xml.addTag("tracking");
		xml.pushTag("tracking");
			xml.addComment(" person finder depth clipping threshold; int 0 - 255 ");
			xml.addValue("threshold", threshold);
			xml.addComment(" kinect near clipping plane in cm; int ");
			xml.addValue("nearClipping", (int)nearClipping);
			xml.addComment(" kinect far clipping plane in cm; int ");
			xml.addValue("farClipping", (int)farClipping);
			xml.addComment(" minimum area to consider when looking for person blobs; int ");
			xml.addValue("personMinArea", (int)personMinArea);
			xml.addComment(" maximum area to consider when looking for person blobs; int ");
			xml.addValue("personMaxArea", (int)personMaxArea);
		xml.popTag();
		
		xml.addComment(" normalize overhead position coords, enable/disable; bool 0 or 1 ");
		xml.addTag("normalize");
		xml.pushTag("normalize");
			xml.addComment(" normalized; 0 - width ");
			xml.addValue("bNormalizeX", bNormalizeX);
			xml.addComment(" normalized; 0 - height ");
			xml.addValue("bNormalizeY", bNormalizeY);
			xml.addComment(" normalized ; nearClipping - farClipping ");
			xml.addValue("bNormalizeZ", bNormalizeZ);
		xml.popTag();
		
		xml.addComment(" scale overhead position coords, performed after normalization ");
		xml.addTag("scale");
		xml.pushTag("scale");
			xml.addComment(" enable/disable; bool 0 or 1 ");
			xml.addValue("bScaleX", bScaleX);
			xml.addValue("bScaleY", bScaleY);
			xml.addValue("bScaleZ", bScaleZ);
			xml.addComment(" scale amounts ");
			xml.addValue("scaleXAmt", scaleXAmt);
			xml.addValue("scaleYAmt", scaleXAmt);
			xml.addValue("scaleZAmt", scaleZAmt);
		xml.popTag();
		
		xml.addComment(" osc settings ");
		xml.addTag("osc");
		xml.pushTag("osc");
			xml.addComment(" host destination address ");
			xml.addValue("sendAddress", sendAddress);
			xml.addComment(" host destination port ");
			xml.addValue("sendPort", (int)sendPort);
		xml.popTag();
		
	xml.popTag();
	
	if(!xml.saveFile(xmlFile)) {
		ofLogWarning() << "Couldn't save settings: " << xml.doc.ErrorDesc();
			return false;
	}
	return true;
}

//--------------------------------------------------------------
ofPoint ofApp::findNearestPoint(ofPixels& pixels, ofRectangle searchBox, int maxValue) {

	int minX = MAX(searchBox.getLeft(), 0);
	int minY = MAX(searchBox.getTop(), 0);
	int maxX = MIN(searchBox.getRight(), pixels.getWidth());
	int maxY = MIN(searchBox.getBottom(), pixels.getHeight());

	ofPoint nearest;
	unsigned char brightest = 0;
	for(int y = minY; y < maxY; ++y) {
		for(int x = minX; x < maxX; ++x) {
			unsigned char val = pixels[y*pixels.getWidth() + x];
			if(val < maxValue && val > brightest) {
				brightest = val;
				nearest.x = x;
				nearest.y = y;
				nearest.z = val;
			}
		}
	}
	
	return nearest;
}
