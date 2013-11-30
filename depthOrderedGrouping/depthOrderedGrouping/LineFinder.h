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

#include <iostream>
#include <string.h>

using namespace std;

class LineFinder {
  public:
	LineFinder(ImageDetails);
	
	void greyImage();
	void blurImage();

	void detectEdges();
	
  private:

    cannyThresholdOneTrackbar(int);
    cannyThresholdTwoTrackbar(int);
    cannyAperatureTrackbar(int);
	
	houghAccumulatorTrackbar(int);
	houghMinLenTrackbar(int);
	houghMaxGapTrackbar(int);    

	ImageDetails* imageDetails;
	
    // Canny parameters
    int cannyThresh1, cannyThresh2, cannyAperture;

    // HoughlinesP parameters
    int houghAccumulator, houghMinLen, houghMaxGap;
};

#endif /* defined(__depthOrderedGrouping__LineFinder__) */