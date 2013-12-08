/*
 *  LineFinder.cpp
 *  
 *
 *  Created by Lars Jangaard on 11/29/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "LineFinder.h"
#include "ImageDetails.h"

ImageDetails* LineFinder::imageDetails;
int LineFinder::cannyThresh1;
int LineFinder::cannyThresh2;
int LineFinder::cannyAperture;
<<<<<<< HEAD
int LineFinder::houghAccumulator;
int LineFinder::houghMinLen;
int LineFinder::houghMaxGap;
int LineFinder::proximity; // for vanishing point regions
float LineFinder::vertical; // acceptable slope to consider vertical
=======
int LineFinder::houghAccumulator, LineFinder::houghMinLen, LineFinder::houghMaxGap;
int LineFinder::validLineProx;
int LineFinder::verticalThresh;

ImageDetails* LineFinder::imageDetails;
>>>>>>> convertMain

LineFinder::LineFinder(ImageDetails* img) {
    imageDetails = img;

	cannyThresh1 = 18;
	cannyThresh2 = 65;
	cannyAperture = 3;

	houghAccumulator = 40;
	houghMinLen = 10;
	houghMaxGap = 2;

<<<<<<< HEAD
	proximity = 80;
	vertical = 4;
=======
	verticalThresh = 4;
	validLineProx = 80;
>>>>>>> convertMain
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
	imageDetails->insertMat(matName, blurred);

	namedWindow(matName);
	imshow(matName, blurred);
	cvWaitKey(0);
}

void LineFinder::detectEdges() {
	Mat edged;
	string matName = "edged";
    Canny(*imageDetails->getMat("blurred"), edged, cannyThresh1, cannyThresh2, cannyAperture);

	namedWindow(matName);

    createTrackbar("Thresh1", matName, &cannyThresh1, 100, cannyThresholdOneTrackbar);
    createTrackbar("Thresh2", matName, &cannyThresh2, 300, cannyThresholdTwoTrackbar);
    createTrackbar("Aperture", matName, &cannyAperture, 7, cannyAperatureTrackbar);

    imageDetails->insertMat(matName, edged);
    imshow(matName, edged);
	cvWaitKey(0);
}

void LineFinder::detectLines() {
<<<<<<< HEAD

	Mat houghed = imageDetails->getMat("original")->clone();
	string matName = "houghed";
    vector<Vec4i> houghpResult;

	imageDetails->insertMat(matName, houghed);
	namedWindow(matName);
        
    HoughLinesP(*imageDetails->getMat("edged"), houghpResult, 1, CV_PI/180, houghAccumulator, houghMinLen, houghMaxGap);
	imageDetails->insertLineList("houghpResult", houghpResult);

    for( int i = 0; i < houghpResult.size(); i++ ) {
        Vec4i l = houghpResult[i];
        line(*imageDetails->getMat(matName), Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255));
    }

    createTrackbar("Accumulator", matName, &houghAccumulator, 150, houghAccumulatorTrackbar);
    createTrackbar("Min Length", matName, &houghMinLen, 100, houghMinLenTrackbar);
    createTrackbar("Max Gap", matName, &houghMaxGap, 100, houghMaxGapTrackbar);

	imshow(matName, houghed);
	cvWaitKey(0);
}

//findInitGoodLines - takes the lines from the houghed image and filters out those
//					  with the fewest shared vanishing points. Uses
//preconditions - a valid TheImage struct exists with assigned values, including houghlines vector
//postconditions - transformed copy of image is displayed and new lines vector is generated
void LineFinder::findInitGoodLines() {
	Mat vanished = imageDetails->getMat("original")->clone();
	string matName = "vanished";

    vector<Vec4i> leftLinesVec, rightLinesVec, vertLinesVec;
    vector<vector<Vec4i>> allVanPts, goodLines;
    vector<Point> meanVanPts;

	imageDetails->insertMat(matName, vanished);

    findGoodLines("houghpResult",
				  "initLeftLines",
				  "initRightLines",
				  "initVertLines",
				  &allVanPts,
				  &meanVanPts);

	leftLinesVec = *imageDetails->getLineList("initLeftLines");
	rightLinesVec = *imageDetails->getLineList("initRightLines");
	vertLinesVec = *imageDetails->getLineList("initVertLines");
    
	goodLines.push_back(leftLinesVec);
    goodLines.push_back(rightLinesVec);
    goodLines.push_back(vertLinesVec);

    // draw all the 'goodLines'
    for( int i=0; i < goodLines.size(); i++ ) {
        vector<Vec4i> curLines = goodLines[i];
        for( int j=0; j < curLines.size(); j++) {
            Vec4i l = curLines[j];
            line(vanished, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255));
        }
    }
    circle(vanished, meanVanPts[0], 6, Scalar(0,100,0), 1);
    circle(vanished, meanVanPts[1], 6, Scalar(0,100,0), 1);
    imshow(matName, vanished);
	imwrite(matName + ".jpg", vanished);
	cvWaitKey(0);
}

// findGoodLines
// finds your two vanishing points (left and right)
// input name line vector already stored
// and names of line vectors that will be
// created, stored, and populated
// (left, right, and vertical)
// optional vectors for returning vanishing points
void LineFinder::findGoodLines(string myLines,
							   string myLeftLines,
							   string myRightLines,
							   string myVertLines,
							   vector<vector<Vec4i>> *allVanPts = new vector<vector<Vec4i>>,
							   vector<Point> *meanVanPts = new vector<Point>) {
	vector<Vec4i> *myLinesVec = imageDetails->getLineList(myLines);
	vector<Vec4i> *leftLinesVec = new vector<Vec4i>;
	vector<Vec4i> *rightLinesVec = new vector<Vec4i>;
	vector<Vec4i> *vertLinesVec = new vector<Vec4i>;
	vector<Vec4i> leftVanPts, rightVanPts;

	//imageDetails->insertLineList(myLeftLines, *leftLinesVec);
	//imageDetails->insertLineList(myRightLines, *rightLinesVec);
	//imageDetails->insertLineList(myVertLines, *vertLinesVec);

	
	// go thru all the houghline segments and get their coords/slopes/intercepts
	for ( int i=0; i < myLinesVec->size()-1; i++ ) {
		Vec4i line_i = myLinesVec->at(i);
		float mi, ci, x1_i, y1_i, x2_i, y2_i, dx_i, dy_i;
		x1_i = line_i[0];	y1_i = line_i[1];
		x2_i = line_i[2];	y2_i = line_i[3];
		dx_i = x2_i - x1_i;	dy_i = y2_i - y1_i;
		
		if (dx_i == 0) dx_i = .0001; // vertical
		mi = (dy_i / dx_i); // slope
		if (abs(mi) > vertical) vertLinesVec->push_back(line_i); // this will go to 'goodLines' by default
		else {
			ci = y1_i - (mi*x1_i); // intercept
			// do the same thing for every other line to find intersection
			for ( int j=i+1; j < myLinesVec->size(); j++ ) {
				Vec4i line_j = myLinesVec->at(j);
				float mj, cj, x1_j, y1_j, x2_j, y2_j, dx_j, dy_j, intxnX, intxnY;
				x1_j = line_j[0];	y1_j = line_j[1];
				x2_j = line_j[2];	y2_j = line_j[3];
				dx_j = x2_j - x1_j;	dy_j = y2_j - y1_j;
				if (dx_j == 0) dx_j = -.0001; // vertical
				mj = (dy_j / dx_j); // slope
				
				if (abs(mj) > vertical && j == myLinesVec->size()-1)
					vertLinesVec->push_back(line_j);
				else {
					cj = y1_j - (mj*x1_j);
=======
	imageDetails->insertMat("houghed", (*imageDetails->getMat("original")).clone());
    vector<Vec4i> houghpResult;
	namedWindow("Output2");
        
	HoughLinesP(*imageDetails->getMat("edged"), houghpResult, 1, CV_PI/180, houghAccumulator, houghMinLen, houghMaxGap);

	imageDetails->insertLineList("houghpResult", &houghpResult);

	for( int i = 0; i < (*imageDetails->getLineList("houghpResult")).size(); i++ ) {
		Vec4i l = (*imageDetails->getLineList("houghpResult"))[i];
        line(*imageDetails->getMat("houghed"), Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255));
    }

	createTrackbar("Accum", "Output2", &houghAccumulator, 150, houghAccumulatorTrackbar);
    createTrackbar("Min Length", "Output2", &houghMinLen, 100, houghMinLenTrackbar);
    createTrackbar("Max Gap", "Output2", &houghMaxGap, 100, houghMaxGapTrackbar);

	imshow("Output2", *imageDetails->getMat("houghed"));
}

void LineFinder::findValidLines() {
	imageDetails->insertMat("vanished", (*imageDetails->getMat("original")).clone());

	vector<Point> *vanPts = new vector<Point>;
	vector<vector<Vec4i>*> *goodLines = new vector<vector<Vec4i>*>;

	getLineDetails();

	vanPts->push_back(findMeanVanPts(imageDetails->getLineList("leftVanLines")));
	vanPts->push_back(findMeanVanPts(imageDetails->getLineList("rightVanLines")));

	trimLines(*vanPts);

    // draw all the 'goodLines'
    for( int i=0; i < goodLines->size(); i++ ) {
        vector<Vec4i> *curLines = (*goodLines)[i];
        
		for( int j=0; j < curLines->size(); j++) {
            Vec4i l = (*curLines)[j];
            line(*imageDetails->getMat("vanished"), Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255));
        }
    }

    circle(*imageDetails->getMat("vanished"), (*vanPts)[0], 6, Scalar(0,100,0), 1);
    circle(*imageDetails->getMat("vanished"), (*vanPts)[1], 6, Scalar(0,100,0), 1);
        
    // Trackbar for vanishing points filter (goes slow)
    createTrackbar("Prox", "Output3", &validLineProx, 300, goodLineProxTrackbar);		
		
	imshow("Output3", *imageDetails->getMat("vanished"));
}

void LineFinder::getLineDetails() {
	//Mat *original = imageDetails->getMat("original");

	imageDetails->insertLineList("vertLines");
	imageDetails->insertLineList("leftVanLines");
	imageDetails->insertLineList("rightVanLines");

    // go thru all the houghline segments and get their coords/slopes/intercepts
	for ( int i=0; i < imageDetails->getLineList("houghpResult")->size(); i++ ) {
        Vec4i line_i = imageDetails->getLineList("houghpResult")->at(i);
        float mi, ci, x1_i, y1_i, x2_i, y2_i, dx_i, dy_i;
        x1_i = line_i[0];        y1_i = line_i[1];
        x2_i = line_i[2];        y2_i = line_i[3];
        dx_i = x2_i - x1_i;        dy_i = y2_i - y1_i;

        if (dx_i == 0) dx_i = .0001; // vertical
        mi = (dy_i / dx_i); // slope
		if (abs(mi) > verticalThresh) imageDetails->getLineList("vertLines")->push_back(line_i); // this will go to 'goodLines' by default
        else {
            ci = y1_i - (mi*x1_i); // intercept

            // do the same thing for every other line to find intersection
            for ( int j=i+1; j < imageDetails->getLineList("houghpResult")->size(); j++ ) {
				Vec4i line_j = imageDetails->getLineList("houghpResult")->at(j);
				float mj, cj, x1_j, y1_j, x2_j, y2_j, dx_j, dy_j, intxnX, intxnY;
				x1_j = line_j[0];        y1_j = line_j[1];
				x2_j = line_j[2];        y2_j = line_j[3];
				dx_j = x2_j - x1_j;        dy_j = y2_j - y1_j;
				if (dx_j == 0) dx_j = -.0001; // vertical
				mj = (dy_j / dx_j); // slope

				if (abs(mj) > verticalThresh && j == imageDetails->getLineList("houghpResult")->size()-1)
					imageDetails->getLineList("vertLines")->push_back(line_j);
				else {
					cj = y1_j - (mj*x1_j);

>>>>>>> convertMain
					if( (mi - mj) == 0) { // parallel
						mi += .0001;
						mj -= .0001;
					}
<<<<<<< HEAD
=======

>>>>>>> convertMain
					// intersections
					intxnX = (cj - ci) / (mi - mj);
					intxnY = (mi*intxnX) + ci;
					Vec4i curVanPt;
<<<<<<< HEAD
					
=======

>>>>>>> convertMain
					if ( (intxnY >= 0) && (intxnY < imageDetails->getMat("original")->rows) ) {
						curVanPt[0] = i;
						curVanPt[1] = j;
						curVanPt[2] = intxnX;
						curVanPt[3] = intxnY;
<<<<<<< HEAD
						
						// collect the good vanishing points
						// CHANGE FOR FINAL
						if (intxnX < 265 && intxnX > 0) leftVanPts.push_back(curVanPt);
						else if (intxnX > 650 && intxnX < imageDetails->getMat("original")->cols)
							rightVanPts.push_back(curVanPt);
					}
				}
			}
		}
	}
	// 2-element vector, holds left and right vanishing point vectors
	allVanPts->push_back(leftVanPts);
	allVanPts->push_back(rightVanPts);
	
	Mat leftLabels, rightLabels;
	Mat leftCenters, rightCenters;
	Mat leftMat(leftVanPts.size(), 2, CV_32F);
	Mat rightMat(rightVanPts.size(), 2, CV_32F);
	const int ITERNS = 10;               // iterations
	const int ATMPTS = 1;                // attempts
	
	for (int i=0; i < leftVanPts.size(); i++) {
		leftMat.at<float>(i,0) = (float)leftVanPts[i][2];
		leftMat.at<float>(i,1) = (float)leftVanPts[i][3];
	}
	
	kmeans(leftMat, 1, leftLabels, TermCriteria(CV_TERMCRIT_ITER, ITERNS, 1.0),
		   ATMPTS, KMEANS_PP_CENTERS, leftCenters);
	int leftX = (int)leftCenters.at<float>(0,0);
	int leftY = (int)leftCenters.at<float>(0,1);
	meanVanPts->push_back(Point(leftX, leftY));
	
	for (int i=0; i < rightVanPts.size(); i++) {
		rightMat.at<float>(i,0) = (float)rightVanPts[i][2];
		rightMat.at<float>(i,1) = (float)rightVanPts[i][3];
	}
	kmeans(rightMat, 1, rightLabels, TermCriteria(CV_TERMCRIT_ITER, ITERNS, 1.0),
		   ATMPTS, KMEANS_PP_CENTERS, rightCenters);
	int rightX = (int)rightCenters.at<float>(0,0);
	int rightY = (int)rightCenters.at<float>(0,1);
	meanVanPts->push_back(Point(rightX, rightY));
	
	// go thru all the vanishing points
	for (int i=0; i < allVanPts->size(); i++) {
		vector<Vec4i> curVanPts = allVanPts->at(i);
		vector<Vec4i> *curGoodLines = (i==0) ? leftLinesVec : rightLinesVec;
		float curMeanX = meanVanPts->at(i).x;
		float curMeanY = meanVanPts->at(i).y;
		
		for (int j=0; j < curVanPts.size(); j++) {
			Vec4i curVanPt = curVanPts[j];
			circle(*imageDetails->getMat("vanished"), Point(curVanPt[2], curVanPt[3]),3, Scalar(0,255,0));
			int curVanPtX = curVanPt[2];
			int curVanPtY = curVanPt[3];
			if (abs(curMeanX - curVanPtX) < proximity &&
				abs(curMeanY - curVanPtY) < proximity) {
					int myLinesInd0 = curVanPt[0];
					int myLinesInd1 = curVanPt[1];
					curGoodLines->push_back(myLinesVec->at(myLinesInd0));
					curGoodLines->push_back(myLinesVec->at(myLinesInd1));
			}
		}
	}

	imageDetails->insertLineList(myLeftLines, *leftLinesVec);
	imageDetails->insertLineList(myRightLines, *rightLinesVec);
	imageDetails->insertLineList(myVertLines, *vertLinesVec);
}

=======

						// collect the good vanishing points
						// CHANGE FOR FINAL
						if (intxnX < 265 && intxnX > 0) imageDetails->getLineList("leftVanLines")->push_back(curVanPt);
						else if (intxnX > 650 && intxnX < imageDetails->getMat("original")->cols) imageDetails->getLineList("rightVanLines")->push_back(curVanPt);
					}
                }
            }
        }
    }
}

Point LineFinder::findMeanVanPts(vector<Vec4i> *vanPts) {
    Mat labels;
    Mat centers;
    Mat mat(vanPts->size(), 2, CV_32F);
    const int ITERNS = 10;                // iterations
    const int ATMPTS = 1;                // attempts
        
    for (int i = 0; i < vanPts->size(); i++) {
        mat.at<float>(i,0) = (float)(*vanPts)[i][2];
        mat.at<float>(i,1) = (float)(*vanPts)[i][3];
    }

    kmeans(mat, 1, labels, TermCriteria(CV_TERMCRIT_ITER, ITERNS, 1.0), ATMPTS, KMEANS_PP_CENTERS, centers);

	return Point(centers.at<float>(0,0), centers.at<float>(0,1));
}

void LineFinder::trimLines(vector<Point> vanPts) {
	vector<vector<Vec4i>*> allVanLines;
	vector<vector<Vec4i>*> curGoodLines;

	allVanLines.push_back(imageDetails->getLineList("leftVanLines"));
	allVanLines.push_back(imageDetails->getLineList("rightVanLines"));

    for (int i=0; i < allVanLines.size(); i++) {
        vector<Vec4i> curVanPts = *allVanLines.at(i);
		curGoodLines.push_back(new vector<Vec4i>);

        float curMeanX = vanPts.at(i).x;
        float curMeanY = vanPts.at(i).y;

        for (int j=0; j < curVanPts.size(); j++) {
            Vec4i curVanPt = curVanPts[j];
			circle(*imageDetails->getMat("vanished"), Point(curVanPt[2], curVanPt[3]),3, Scalar(0,255,0));
            int curVanPtX = curVanPt[2];
            int curVanPtY = curVanPt[3];

			if (abs(curMeanX - curVanPtX) < validLineProx &&
				abs(curMeanY - curVanPtY) < validLineProx) {

				curGoodLines.at(i)->push_back((*imageDetails->getLineList("houghpResult"))[curVanPt[0]]);
                curGoodLines.at(i)->push_back((*imageDetails->getLineList("houghpResult"))[curVanPt[1]]);
            }
        }
    }

	imageDetails->insertLineList("leftVanLines", curGoodLines.at(0));
	imageDetails->insertLineList("rightVanLines", curGoodLines.at(1));
}
>>>>>>> convertMain

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
void LineFinder::houghAccumulatorTrackbar(int slider, void *src) {
    if (slider == 0) slider = 1;
    houghAccumulator = slider;
    detectLines();
}

// houghMinLenTrackbar - min line length
void LineFinder::houghMinLenTrackbar(int slider, void *src) {
    if (slider == 0) slider = 1;
    houghMinLen = slider;
    detectLines();
}

// houghMaxGapTrackbar - hough threshold
void LineFinder::houghMaxGapTrackbar(int slider, void *src) {
    if (slider == 0) slider = 1;
    houghMaxGap = slider;
    detectLines();
}

void LineFinder::goodLineProxTrackbar(int slider, void *src) {
    if (slider == 0) slider = 1;

    validLineProx = slider;
    findValidLines();
}