#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0,0,0);
    ofEnableSmoothing();
    
    ofSetFrameRate(60);
    
    if(MOVIE_DEBUG_MODE){
        //movie
        movie.loadMovie("video.mov");
        movie.play();
    }else {
        //Setting of WebCam
        cam.setVerbose(true);
        cam.setDeviceID(USE_CAMERA_ID);
        
        videoWidth =  640*0.8;
        float ratio = 1024.0/640.0;
        videoHeight = 480*0.8;
        cam.initGrabber(videoWidth, videoHeight);
    }
    
    //setup of gui
    int guiMargin = 10;
    float guiW = 300;
    float guiH = ofGetHeight() - guiMargin*2;
    ofxGuiSetDefaultWidth(guiW);
    ofxGuiSetDefaultHeight(18);
    
    gui.setup();
    gui.add(radius.setup("Radius", 7, 1, 50));
    gui.add(area.setup("Area",1200,1,1500));
    gui.add(mThreshold.setup("Threshold", 150, 0, 500));
    gui.setPosition(guiMargin, guiMargin + videoHeight);
    
    
    contourFinder.setMinAreaRadius(radius);
    contourFinder.setMaxArea(area);
    contourFinder.setThreshold(mThreshold);
    // wait for half a frame before forgetting something
    contourFinder.getTracker().setPersistence(1055);
    // an object can move up to 32 pixels per frame
    contourFinder.getTracker().setMaximumDistance(32);
    
    showLabels = true;
}

//--------------------------------------------------------------
void ofApp::update(){
    contourFinder.setMinAreaRadius(radius);
    contourFinder.setMaxArea(area);
    contourFinder.setThreshold(mThreshold);
    
    if(MOVIE_DEBUG_MODE){
        movie.update();
        if(movie.isFrameNew()){
            ofxCv::blur(movie,10);
            contourFinder.findContours(movie);
        }
    }else {
        cam.update();
        if(cam.isFrameNew()){
            ofxCv::blur(cam,10);
            contourFinder.findContours(cam);
        }
    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetBackgroundAuto(showLabels);
    ofxCv::RectTracker& tracker = contourFinder.getTracker();

    ofSetColor(120, 120, 120);
    ofRect(0,0,videoWidth,videoHeight);
    
    if(showLabels){
        ofSetColor(255, 255, 255);
        
        int x,y;
        if(MOVIE_DEBUG_MODE){
            x = (int)(ofGetWidth()/2 - movie.width/2);
            y = (int)(ofGetHeight()/2 - movie.height/2);
            movie.draw(x,y);
        }else {
            x = (int)(ofGetWidth()/2 - cam.width/2);
            y = (int)(ofGetHeight()/2 - cam.height/2);
            cam.draw(videoWidth, 0);
        }


        contourFinder.draw();

        for(int i = 0;i < contourFinder.size();i++){
            ofPoint center = ofxCv::toOf(contourFinder.getCenter(i));
            ofPushMatrix();
            ofTranslate(center.x, center.y);
            int label = contourFinder.getLabel(i);
            string msg = ofToString(label) + ":" + ofToString(tracker.getAge(label));
            ofDrawBitmapString(msg, 10,-10);
            ofVec2f velocity = ofxCv::toOf(contourFinder.getVelocity(i));
            ofScale(5, 5);
            ofLine(0, 0, velocity.x, velocity.y);
            ofPopMatrix();
        }
    }else {
        for (int i = 0; i < contourFinder.size(); i++) {
            unsigned int label = contourFinder.getLabel(i);
            // only draw a line if this is not a new label
            if(tracker.existsPrevious(label)){
                //use the label to pick a random color
                ofSeedRandom(label << 24);
                ofSetColor(ofColor::fromHsb(ofRandom(255), 255, 255));
                //get the tracked object(cv::Rect) at current and previous position
                const cv::Rect& previous = tracker.getPrevious(label);
                const cv::Rect& current = tracker.getCurrent(label);
                // get the centers of the rectangles
                ofVec2f previousPosition(previous.x + previous.width / 2, previous.y + previous.height / 2);
                ofVec2f currentPosition(current.x + current.width / 2, current.y + current.height / 2);
                ofLine(previousPosition, currentPosition);
            }
        }
    }
    
    
    // this chunk of code visualizes the creation and destruction of labels
    const vector<unsigned int>& currentLabels = tracker.getCurrentLabels();
    const vector<unsigned int>& previousLabels = tracker.getPreviousLabels();
    const vector<unsigned int>& newLabels = tracker.getNewLabels();
    const vector<unsigned int>& deadLabels = tracker.getDeadLabels();
    
    ofSetColor(ofxCv::cyanPrint);
    for(int i = 0;i < currentLabels.size();i++){
        int j = currentLabels[i];
        ofLine(j, 0, j, 4);
    }
    
    ofSetColor(ofxCv::magentaPrint);
    for(int i = 0; i < previousLabels.size(); i++) {
        int j = previousLabels[i];
        ofLine(j, 4, j, 8);
    }
    ofSetColor(ofxCv::yellowPrint);
    for(int i = 0; i < newLabels.size(); i++) {
        int j = newLabels[i];
        ofLine(j, 8, j, 12);
    }
    ofSetColor(ofColor::white);
    for(int i = 0; i < deadLabels.size(); i++) {
        int j = deadLabels[i];
        ofLine(j, 12, j, 16);
    }
    
    
    
    
    
    gui.draw();
    ofDrawBitmapString(ofToString(ofGetFrameRate())+"fps", 10, videoHeight - 15);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == ' ') {
        showLabels = !showLabels;
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
