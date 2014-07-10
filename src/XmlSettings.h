/*
 * Copyright (c) 2013 Dan Wilcox <danomatika@gmail.com>
 *
 * BSD Simplified License.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 *
 * See https://github.com/danomatika/ofxAppUtils for documentation
 *
 * This is a port of my Obj-C Moving Average Class on RosettaCode:
 * http://rosettacode.org/wiki/Moving_Average#Objective-C
 *
 */
#pragma once

#include "ofxXmlSettings.h"

// tiny wrapper to allow adding of xml comments
class XmlSettings : public ofxXmlSettings {
	public:
	
		void addComment(const std::string text) {
			TiXmlComment comment(text.c_str());
			storedHandle.ToNode()->InsertEndChild(comment);
		}
};
