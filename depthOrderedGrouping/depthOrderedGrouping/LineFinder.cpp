/*
 *  LineFinder.cpp
 *  
 *
 *  Created by Lars Jangaard on 11/29/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "LineFinder.h"

LineFinder::LineFinder(ImageDetails* img) {
    imageDetails = img;

	cannyThresh1 = 18;
	cannyThresh2 = 65;
	cannyAperature = 3;

	houghAccumulator = 40;
	houghMinLen = 10;
	houghMaxGap = 2;

    proximity = 80;
    vertical = 4;
}

void LineFinder::greyImage() {
	Mat greyed;
	string matName = "greyScale";
	
	cvtColor(image, greyed, CV_BGR2GRAY);
	
	namedWindow(matName);
	imshow(matName, greyed);
	
	cvWaitKey(0);
}

void LineFinder::blurImage() {
	Mat blurred;
	string matName = "blurred";
	int kernelSize = 5;
	double sigma1 = 2.0;
	double sigma2 = 2.0;
	
	GaussianBlur(greyed, blurred, Size(kernelSize, kernelSize), sigma1, sigma2);

	imageDetails.insertMat("blurred", blurred);
	imshow("blurred", blurred);
	
	cvWaitKey(0);
}

void LineFinder::detectEdges() {
	Mat edged;
    Canny(imageDetails.getMat("grey"), edged, cannyThresh1, cannyThresh2, cannyAperature);

    createTrackbar("Thresh1", "Output1", &cannyThresh1, 100, cannyThresholdOneTrackbar);
    createTrackbar("Thresh2", "Output1", &cannyThresh2, 300, cannyThresholdTwoTrackbar);
    createTrackbar("Aperture", "Output1", &cannyAperature, 7, cannyAperatureTrackbar);

    imageDetails.insertMat("edged", edged);
    imshow("Grey Scale", edged);	

    cvWaitKey(0);
}

void LineFinder::detectLines() {
        
    Mat houghed;    
    vector<Vec4i> houghpResult;
        
    HoughLinesP(myImg->edged, houghpResult, 1, CV_PI/180, houghAccumulator, houghMinLen, houghMaxGap);

    for( int i = 0; i < myImg->allLines.size(); i++ ) {
        Vec4i l = myImg->allLines[i];
        line(myImg->houghed, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255));
    }

    createTrackbar("Accumulator", "Output2", &houghAccumulator, 150, houghAccumulatorTrackbar);
    createTrackbar("Min Length", "Output2", &houghMinLen, 100, houghMinLenTrackbar);
    createTrackbar("Max Gap", "Output2", &houghMaxGap, 100, houghMaxGapTrackbar);

    imshow("Output2", houghed);
}

// cannyThresholdOneTrackbar - cannyThresh1
// the lower the threshold the more lines you get in non-edgy places
void LineFinder::cannyThresholdOneTrackbar(int slider) {
        cannyThresh1 = slider;
        detectEdges();
}

// cannyThresholdTwoTrackbar - cannyThresh2
// seems to have a relationship with threshold 1 but mostly does the same thing
void LineFinder::cannyThresholdTwoTrackbar(int slider) {
        cannyThresh2 = slider;
        detectEdges();
}

// cannyAperatureTrackbar - aperture
// compounds the edge detection.
// can only be 1, 3, 5, 7, but crashes on 1 so i don't allow it
void LineFinder::cannyAperatureTrackbar(int slider) {
    int aperSize = slider;
        
    if (slider < 5 ) slider = 3;
    else if (aperSize == 6) aperSize = 7;

    cannyAperature = slider;
}

// houghAccumulatorTrackbar - hough threshold
void LineFinder::houghAccumulatorTrackbar(int slider) {
    if (slider == 0) slider = 1;

    houghAccumulator = slider;
    detectLines();
}

// houghMinLenTrackbar - min line length
void LineFinder::houghMinLenTrackbar(int slider) {
    if (slider == 0) slider = 1;

    houghMinLen = slider;
    detectLines();
}

// houghMaxGapTrackbar - hough threshold
void LineFinder::houghMaxGapTrackbar(int slider) {
    if (slider == 0) slider = 1;
        
    houghMaxGap = slider;
    detectLine()
}