//
//  imageDetails.cpp
//  depthOrderedGrouping
//
//  Created by Lars Jangaard on 11/28/13.
//  Copyright (c) 2013 Lars Jangaard. All rights reserved.
//

#include "imageDetails.h"

ImageDetails::ImageDetails() {
	cannyThresh1 = 18;
	cannyThresh2 = 65;
	cannyAperature = 3;

	houghAccumulator = 40;
	houghMinLen = 10;
	houghMaxGap = 2;

    proximity = 80;
    vertical = 4;
}

Mat ImageDetails::openOriginal(string imageLocation) {
    string matName = "original";
    Mat image = imread(imageLocation);
    
    Mats[matName] = image;
    
    showImage(matName, image);
    
    cvWaitKey(0);
}

Mat* ImageDetails::getMat(string imageName) {
	return Mats[imageName];
}

Mat* ImageDetails::insertMat(string matName, Mat imageMat) {
    Mats[matName] = imageMat;
    return Mats[matName];
}

vector<Vec4i> ImageDetails::getLineList(string lineListName) {
    return LineList[lineListName];
}

vector<Vec4i> ImageDetails::insertLineList(string lineListName, vector<Vec4i> lineList) {
    LineList[lineListName] = lineList;
}

void ImageDetails::showImage(string windowName, Mat mat) {
    namedWindow(windownName);
    imshow(windowName, mat);
}