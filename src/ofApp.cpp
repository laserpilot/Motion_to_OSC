#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    //Camera settings
    camWidth=640;
    camHeight = 480;
    // testCapture.allocate(camWidth, camHeight, GL_RGB);
    //vidGrabber.setDeviceID(3); //dont set this explicitly
    vidGrabber.initGrabber(camWidth, camHeight);
    
    //CV motion settings
	cameraColorImage.allocate( camWidth, camHeight );
	cameraGrayImage.allocate( camWidth, camHeight );
	cameraGrayPrevImage.allocate( camWidth, camHeight );
	cameraGrayDiffImage.allocate( camWidth, camHeight );
	cameraDiffFloatImage.allocate( camWidth, camHeight );
	cameraMotionFloatImage.allocate( camWidth, camHeight );
    
    
    numZones = 5;
    
    zone.assign(numZones,0.0);
    
    for (int i=0; i<numZones; i++){
        vector<float> temp;
        temp.assign(200,0.0);
        motionGraphs.push_back(temp);
    }
    
    trailsOn = true;
    
    trailBlur = 3;
    
    graphYScale = 10000;
    
    cameraMotionFadeAmount = 0.7;
    
    
    oscSend.setup("localhost", 6767);

}

//--------------------------------------------------------------
void ofApp::update(){
    
    bool bNewFrame = false;
    
    vidGrabber.update();
    bNewFrame = vidGrabber.isFrameNew();
    
    if( bNewFrame )
	{
		updateMotion( vidGrabber.getPixels() );
    }

}

void ofApp::updateMotion(unsigned char *pixels){
    //--------------------------------------------------------------

        cameraColorImage.setFromPixels( pixels, camWidth, camHeight );
        
        cameraGrayPrevImage	= cameraGrayImage;
        cameraGrayImage		= cameraColorImage;
        
        cameraGrayDiffImage.absDiff( cameraGrayImage, cameraGrayPrevImage );
        cameraGrayDiffImage.threshold( 30 );
        
    if(trailsOn){ //draw motion trails
        cameraDiffFloatImage	= cameraGrayDiffImage;
        cameraMotionFloatImage	*= cameraMotionFadeAmount;
        cameraMotionFloatImage	+= cameraDiffFloatImage;
        cameraMotionFloatImage.blurGaussian( trailBlur );
    }
    
    for (int i=0; i<numZones; i++){
        zone[i] = cameraGrayDiffImage.countNonZeroInRegion(i*(camWidth/numZones), 0,(camWidth/numZones), camHeight);
        //cout<< zone[i] <<endl;
        float scaledVal = ofMap(zone[i], 0, graphYScale, 0.0, 1.0, true);
        motionGraphs[i].push_back(scaledVal);
        if(motionGraphs[i].size()>=200){
            motionGraphs[i].erase(motionGraphs[i].begin(),motionGraphs[i].begin()+1);
        }
        
        //Here is where we send the OSC values of each zone
        
        ofxOscMessage m;
        m.setAddress("/zone" + ofToString(i)); //zone1,zone2
        m.addFloatArg(ofMap(scaledVal, 0, 1, 0, 127, true)); //remap to midi values for OSCulator
        
        oscSend.sendMessage(m);
    }
    
    
        
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackground(0); //clear the buffer!
    
    
    if(trailsOn){ //draw trails
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        ofSetColor(255, 255, 255, 200);
        cameraMotionFloatImage.draw(0,0, camWidth, camHeight);
    }
    
    ofSetColor(255, 255, 255,255);
    vidGrabber.draw(0, 0, camWidth, camHeight); //Draw camera feed
    ofDisableBlendMode();
    
    for(int i=0; i<numZones; i++){
        
        if(i==0){
            ofSetColor(200, 0, 255);
        }
        if(i==1){
            ofSetColor(100, 200, 200);
        }
        if(i==2){
            ofSetColor(100, 200, 100);
        }
        if(i==3){
            ofSetColor(50, 200, 200);
        }
        if(i==3){
            ofSetColor(50, 50, 200);
        }
        
        ofPushMatrix();
        ofTranslate(i*(camWidth/numZones), 550);
        ofScale(0.5,0.2);
        ofBeginShape();
        for (int j = 0; j < motionGraphs[i].size(); j++){
            if( j == 0 ) ofVertex(j, 200);
            
            ofVertex(j, 200 - motionGraphs[i][j] * 200);
            
            if( j == motionGraphs[i].size() -1 ) ofVertex(j, 200);
        }
        ofEndShape(false);
        

        ofSetColor(255);
        ofPopMatrix();
        ofDrawBitmapString("Zone " + ofToString(i), i*(camWidth/numZones), camHeight+20);
        ofDrawBitmapString(ofToString(zone[i]), i*(camWidth/numZones), camHeight+42);

        
        ofLine(i*(camWidth/numZones), 0, i*(camWidth/numZones), camHeight);
        
        //ofSetColor(0, 0, 255);
    }

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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