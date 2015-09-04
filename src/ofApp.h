#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include "ofxOsc.h"
#include "ofxOsc.h"

/*カメラID*/
#define USE_CAMERA_ID 1
/*MOVIE DEBUG MODE*/
#define MOVIE_DEBUG_MODE false

/*MOVIE RATIO*/
#define MOVIE_WIDTH_RATIO 640
#define MOVIE_HEIGHT_RATIO 480

/*OSC*/
#define HOST "localhost" //送信先ホストのIPを設定
#define PORT 8000 //送信先のポート番号を設定

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
        void windowResized();
//		void mouseMoved(int x, int y );
//		void mouseDragged(int x, int y, int button);
//		void mousePressed(int x, int y, int button);
//		void mouseReleased(int x, int y, int button);
//		void windowResized(int w, int h);
//		void dragEvent(ofDragInfo dragInfo);
//		void gotMessage(ofMessage msg);

    // Application method
    //ビデオサイズを画面幅に合わせるて調整
    void setVideoSize();
    
    void switchFullScreenMode(int key);
    
    
    //Applicaiton Valiable
    
    //WebCam
    ofVideoGrabber cam;
    float videoWidth;
    float videoHeight;
    float actVideoWidth;
    float actVideoHeight;
    ofImage image;
    
    
    
    //Tracking
    float threshold;
    ofVideoPlayer movie;
    ofxCv::ContourFinder contourFinder;
    bool showLabels;

    
    ofxPanel gui;
    ofxIntSlider minRadius;
    ofxIntSlider maxRadius;
    ofxIntSlider minArea;
    ofxIntSlider maxArea;
    ofxFloatSlider mThreshold;
    
    //Collision
    float collisionDist = 0;

    //OSCメッセージの送信者
    ofxOscSender sender;
};
