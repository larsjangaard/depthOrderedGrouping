/*
 *  LineFinder.h
 *  
 *
 *  Created by Lars Jangaard on 11/29/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __depthOrderedGrouping__LineFinder__
#define __depthOrderedGrouping__LineFinder__

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <set>
#include "imageDetails.h"

using namespace cv;
using namespace std;

class LineFinder {
  public:
	LineFinder(ImageDetails*);
	
	void greyImage();
	void blurImage();

	static void detectEdges();
	static void detectLines();

	static void findValidLines();
	static void findValidLines(vector<Vec4i>*, string, string, string);
	
  private:

    static void cannyThresholdOneTrackbar(int, void*);
    static void cannyThresholdTwoTrackbar(int, void*);
    static void cannyAperatureTrackbar(int, void*);
	
	static void houghAccumulatorTrackbar(int, void*);
	static void houghMinLenTrackbar(int, void*);
	static void houghMaxGapTrackbar(int, void*);

	static void goodLineProxTrackbar(int, void*);

	static ImageDetails* imageDetails;

	static void getLineDetails();
	static void getLineDetails(string, string, string);

	static Point findMeanVanPts(vector<Vec4i>*);
	static Point findMeanVanPts(vector<Vec4i>*, string, string, string);

	static void trimLines(vector<Point>);
	static void trimLines(vector<Point>, string, string, string);
	
    // Canny parameters
    static int cannyThresh1;
	static int cannyThresh2;
	static int cannyAperture;

    // HoughlinesP parameters
    static int houghAccumulator, houghMinLen, houghMaxGap;

	// findValidLine parameters
	static int verticalThresh;
	static int validLineProx;
};

#endif /* defined(__depthOrderedGrouping__LineFinder__) */