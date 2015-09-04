#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0,0,0);
    ofEnableSmoothing();
    
    ofSetFrameRate(60);
    videoWidth =  640;
    videoHeight = 480;
    
    
    setVideoSize();
    if(MOVIE_DEBUG_MODE){
        //movie
        movie.loadMovie("debugmov1.mov");
        movie.play();
    }else {
        //Setting of WebCam
        cam.setVerbose(true);
        cam.setDeviceID(USE_CAMERA_ID);
        cam.initGrabber(actVideoWidth, actVideoHeight);
    }
    
    //setup of gui
    int guiMargin = 10;
    float guiW = 300;
    float guiH = ofGetHeight() - guiMargin*2;
    ofxGuiSetDefaultWidth(guiW);
    ofxGuiSetDefaultHeight(18);
    
    gui.setup();
    gui.add(minRadius.setup("MinRadius", 1, 10, 50));
    gui.add(maxRadius.setup("MaxRadius", 100, 10, 500));
    gui.add(minArea.setup("minArea",10,0,50));
    gui.add(maxArea.setup("maxArea",5500,40,5500));
    gui.add(mThreshold.setup("Threshold", 15, 0, 255));
    gui.setPosition(guiMargin, guiMargin);
    
    
    contourFinder.setMinAreaRadius(minRadius);
    contourFinder.setMaxAreaRadius(maxRadius);
    contourFinder.setThreshold(mThreshold);
    // wait for half a frame before forgetting something
    contourFinder.getTracker().setPersistence(15);
    // an object can move up to 32 pixels per frame
    contourFinder.getTracker().setMaximumDistance(32);
    
    showLabels = true;
    
    
    //指定したIPアドレスとポート番号でサーバーに接続
    sender.setup( HOST, PORT );
}

//--------------------------------------------------------------
void ofApp::update(){
    contourFinder.setMinAreaRadius(minRadius);
    contourFinder.setMaxAreaRadius(maxRadius);
    contourFinder.setMinArea(minArea);
    contourFinder.setMaxArea(maxArea);
    contourFinder.setThreshold(mThreshold);
    
    //ビデオのサイズ調整
    setVideoSize();
    
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
    
    //当たり判定
    if(showLabels){
        //接しているか判定
        float objRads[contourFinder.size()];
        float objPosX[contourFinder.size()];
        float objPosY[contourFinder.size()];
        
        for(int i = 0;i<contourFinder.size();i++){
            cv::Rect targetRect = contourFinder.getBoundingRect(i);
            float targetW = targetRect.width;
            float targetH = targetRect.height;
            float targetF = max(targetW,targetH);
            objRads[i] = targetF;
            objPosX[i] = contourFinder.getCenter(i).x;
            objPosY[i] = contourFinder.getCenter(i).y;
        }
        
        for(int i = 0;i < contourFinder.size();i++){
            for (int n = i+1; n<contourFinder.size(); n++) {
                if(objRads[n]){
                    float distance = ofDist(objPosX[i], objPosY[i], objPosX[n], objPosY[n]);
                    if(objRads[i] + objRads[n] + collisionDist > distance){
                        std::cout << "Collision!!" << endl;
                        std::cout << distance << endl;
                        
                        
                        //OSCメッセージの準備
                        ofxOscMessage m;
                        //OSCアドレスの指定
                        m.setAddress( "/collision/flag" );
                        m.addIntArg(0);
                        //メッセージを送信
                        sender.sendMessage( m );
                        
                    }
                }
            }
        }
            
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetBackgroundAuto(showLabels);
    
    ofxCv::RectTracker& tracker = contourFinder.getTracker();

    
    if(showLabels){
        ofSetColor(255, 255, 255);
        
        if(MOVIE_DEBUG_MODE){
            movie.draw(0,0,actVideoWidth,actVideoHeight);
        }else {
            cam.draw(0, 0,actVideoWidth,actVideoHeight);
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
    ofDrawBitmapString(ofToString(ofGetFrameRate())+"fps", 10, ofGetHeight() - 15);
}

//--------------------------------------------------------------
// EVENT
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    //フルスクリーンモードの切り替え
    switchFullScreenMode(key);
    
    if(key == ' ') {
        showLabels = !showLabels;
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

void ofApp::switchFullScreenMode(int key){
    //もしキー入力が[f]だったら
    if (key == 'f')
    {
        //フルスクリーン on/off の切り替え
        ofToggleFullscreen();
    }
}


//--------------------------------------------------------------
// Application
//--------------------------------------------------------------
void ofApp::setVideoSize() {
    float winW = ofGetWidth();
    float winH = ofGetHeight();
    
    float ratioW = winW/videoWidth;
    float ratioH = winH/videoHeight;
    
    float videoRatio = std::max(ratioW, ratioH);

    float tempRatioW = videoWidth*videoRatio;
    float tempRatioH = videoHeight*videoRatio;
    
    if(actVideoWidth != videoWidth*videoRatio){
        actVideoWidth = videoWidth*videoRatio;
    }
    
    if(actVideoHeight != videoHeight*videoRatio){
        actVideoHeight = videoHeight*videoRatio;
    }
    
}