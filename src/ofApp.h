#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include "ofxOsc.h"

#define USE_CAMERA_ID 1
#define MOVIE_DEBUG_MODE false

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    //WebCam
    ofVideoGrabber cam;
    float videoWidth;
    float videoHeight;
    ofImage image;
    
    
    //Tracking
    float threshold;
    ofVideoPlayer movie;
    ofxCv::ContourFinder contourFinder;
    bool showLabels;

    
    ofxPanel gui;
    ofxIntSlider radius;
    ofxIntSlider area;
    ofxFloatSlider mThreshold;
};
