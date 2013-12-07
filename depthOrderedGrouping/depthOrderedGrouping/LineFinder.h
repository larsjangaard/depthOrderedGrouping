/*
 *  LineFinder.h
 *  
 *
 *  Created by Lars Jangaard on 11/29/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once
#ifndef __depthOrderedGrouping__LineFinder__
#define __depthOrderedGrouping__LineFinder__

#include <iostream>
#include <string.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "imageDetails.h"

using namespace cv;
using namespace std;

class LineFinder {

	public:
		LineFinder(ImageDetails*);
	
		static void greyImage();
		static void blurImage();
		static void detectEdges();
		static void detectLines();
		static void findInitGoodLines();

		// findGoodLines
		// finds your two vanishing points (left and right)
		// input name line vector already stored
		// and names of line vectors that will be
		// created, stored, and populated
		// (left, right, and vertical)
		// optional vectors for returning vanishing points
		static void findGoodLines(string myLines,
								  string myLeftLines,
								  string myRightLines,
								  string myVertLines,
								  vector<vector<Vec4i>> *allVanPts,
								  vector<Point> *meanVanPts);
	
	private:
		
		static void cannyThresholdOneTrackbar(int, void*);
		static void cannyThresholdTwoTrackbar(int, void*);
		static void cannyAperatureTrackbar(int, void*);
	
		static void houghAccumulatorTrackbar(int, void*);
		static void houghMinLenTrackbar(int, void*);
		static void houghMaxGapTrackbar(int, void*);

		static ImageDetails* imageDetails;
	
		// Canny parameters
		static int cannyThresh1;
		static int cannyThresh2;
		static int cannyAperture;

		// HoughlinesP parameters
		static int houghAccumulator;
		static int houghMinLen;
		static int houghMaxGap;

		static int proximity; // for vanishing point regions
		static float vertical; // acceptable slope to consider vertical
};

#endif /* defined(__depthOrderedGrouping__LineFinder__) */