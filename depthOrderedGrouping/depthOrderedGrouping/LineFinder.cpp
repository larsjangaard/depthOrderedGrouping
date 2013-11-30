/*
 *  LineFinder.cpp
 *  
 *
 *  Created by Lars Jangaard on 11/29/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "LineFinder.h"

int LineFinder::cannyThresh1;
int LineFinder::cannyThresh2;
int LineFinder::cannyAperture;
int LineFinder::houghAccumulator, LineFinder::houghMinLen, LineFinder::houghMaxGap;
ImageDetails* LineFinder::imageDetails;

LineFinder::LineFinder(ImageDetails* img) {
    imageDetails = img;

	cannyThresh1 = 18;
	cannyThresh2 = 65;
	cannyAperture = 3;

	houghAccumulator = 40;
	houghMinLen = 10;
	houghMaxGap = 2;
}

void LineFinder::greyImage() {
	Mat greyed;
	string matName = "greyScale";
	
	cvtColor(*imageDetails->getMat("original"), greyed, CV_BGR2GRAY);
	
	imageDetails->insertMat(matName, greyed);

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
	
	GaussianBlur(*imageDetails->getMat("greyScale"), blurred, Size(kernelSize, kernelSize), sigma1, sigma2);

	imageDetails->insertMat("blurred", blurred);
	imshow("blurred", blurred);
	
	cvWaitKey(0);
}

void LineFinder::detectEdges() {
	Mat edged;
    Canny(*imageDetails->getMat("blurred"), edged, cannyThresh1, cannyThresh2, cannyAperture);

	namedWindow("Output1");

    createTrackbar("Thresh1", "Output1", &cannyThresh1, 100, cannyThresholdOneTrackbar);
    createTrackbar("Thresh2", "Output1", &cannyThresh2, 300, cannyThresholdTwoTrackbar);
    createTrackbar("Aperture", "Output1", &cannyAperture, 7, cannyAperatureTrackbar);

    imageDetails->insertMat("edged", edged);
    imshow("Output1", edged);	
}

void LineFinder::detectLines() {
        

	Mat houghed = *imageDetails->getMat("original");
    vector<Vec4i> houghpResult;
	imageDetails->insertMat("houghed", houghed.clone());
	namedWindow("Output2");
        
    HoughLinesP(*imageDetails->getMat("edged"), houghpResult, 1, CV_PI/180, houghAccumulator, houghMinLen, houghMaxGap);

    for( int i = 0; i < houghpResult.size(); i++ ) {
        Vec4i l = houghpResult[i];
        line(*imageDetails->getMat("houghed"), Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255));
    }

    createTrackbar("Accumulator", "Output2", &houghAccumulator, 150, houghAccumulatorTrackbar);
    createTrackbar("Min Length", "Output2", &houghMinLen, 100, houghMinLenTrackbar);
    createTrackbar("Max Gap", "Output2", &houghMaxGap, 100, houghMaxGapTrackbar);

	//imageDetails->insertMat("houghed",houghed);
	imshow("Output2", *imageDetails->getMat("houghed"));
}

// cannyThresholdOneTrackbar - cannyThresh1
// the lower the threshold the more lines you get in non-edgy places
void LineFinder::cannyThresholdOneTrackbar(int slider, void *src) {
    cannyThresh1 = slider;
    detectEdges();
}

// cannyThresholdTwoTrackbar - cannyThresh2
// seems to have a relationship with threshold 1 but mostly does the same thing
void LineFinder::cannyThresholdTwoTrackbar(int slider, void *src) {
    cannyThresh2 = slider;
    detectEdges();
}

// cannyAperatureTrackbar - aperture
// compounds the edge detection.
// can only be 1, 3, 5, 7, but crashes on 1 so i don't allow it
void LineFinder::cannyAperatureTrackbar(int slider, void *src) {
    int aperSize = slider;

	if(slider <= 1) slider = 2;

	if(slider%2 == 0) slider = slider+1;

	cannyAperture = slider;

	detectEdges();
}

// houghAccumulatorTrackbar - hough threshold
void LineFinder::houghAccumulatorTrackbar(int slider, void*) {
    if (slider == 0) slider = 1;

    houghAccumulator = slider;
    detectLines();
}

// houghMinLenTrackbar - min line length
void LineFinder::houghMinLenTrackbar(int slider, void*) {
    if (slider == 0) slider = 1;

    houghMinLen = slider;
    detectLines();
}

// houghMaxGapTrackbar - hough threshold
void LineFinder::houghMaxGapTrackbar(int slider, void*) {
    if (slider == 0) slider = 1;
        
    houghMaxGap = slider;
    detectLines();
}