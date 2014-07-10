#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofSetVerticalSync(true);
	
	// settings
	threshold = 160;
	nearClipping = 500;
	farClipping = 4000;
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
	
	bShowRGB = false;
	
	// setup kinect
	kinect.init(false); // no IR image
	kinect.setRegistration(true);
	kinect.setDepthClipping(nearClipping, farClipping);
	kinect.open(0);
	
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
		personFinder.findContours(depthDiff, 3000, (kinect.width*kinect.height)/2, 1, false);
		
		// found person-sized blob?
		if(personFinder.blobs.size() > 0) {
			ofxCvBlob &blob = personFinder.blobs[0];
			person.position = blob.centroid;
			person.width = blob.boundingRect.width;
			person.height = blob.boundingRect.height;
			
			// find highest contour point (actually the lowest value since top is 0)
			int height = INT_MAX;
			for(int i = 0; i < blob.pts.size(); ++i) {
				ofPoint &p = blob.pts[i];
				if(p.y < height &&
				  (p.x > person.x-highestPointThreshold && p.x < person.x+highestPointThreshold)) {
					highestPoint = blob.pts[i];
					height = blob.pts[i].y;
				}
			}
			
			// compute rough head position between centroid and highest point
			head = person.position.getInterpolated(highestPoint, headInterpolation);
			head.z = kinect.getDistanceAt(head);
			
			if(bNormalizeX) head.x = ofMap(head.x, 0, kinect.width, 0, 1);
			if(bNormalizeY) head.y = ofMap(head.y, 0, kinect.height, 0, 1);
			if(bNormalizeZ) head.z = ofMap(head.z, kinect.getNearClipping(), kinect.getFarClipping(), 0, 1);
			
			if(bScaleX) head.x *= scaleXAmt;
			if(bScaleY) head.y *= scaleYAmt;
			if(bScaleZ) head.z *= scaleZAmt;
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw(){

	// draw RGB or IR image
	ofSetColor(255);
	if(bShowRGB) {
		kinect.draw(0, 0);
	}
	else {
		kinect.drawDepth(0, 0);
	}

	if(personFinder.blobs.size() > 0) {

		// draw person finder
		ofSetLineWidth(2.0);
		personFinder.draw(0, 0, 640, 480);
	
		// purple - found person centroid
		ofFill();
		ofSetColor(255, 0, 255);
		ofRect(person.position, 10, 10);
		
		// gold - highest point
		ofFill();
		ofSetColor(255, 255, 0);
		ofRect(highestPoint, 10, 10);
		
		// light blue - "head" position
		ofFill();
		ofSetColor(0, 255, 255);
		ofRect(head.x, head.y, 10, 10);
		
		// draw current position
		ofSetColor(255);
		ofDrawBitmapString(ofToString(head.x, 2)+" "+ofToString(head.y, 2)+" "+ofToString(head.z, 2), 12, 12);
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
			
		case 'r':
			bShowRGB = !bShowRGB;
			break;
	}
}
