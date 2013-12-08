//
//  imageDetails.cpp
//  depthOrderedGrouping
//
//  Created by Lars Jangaard on 11/28/13.
//  Copyright (c) 2013 Lars Jangaard. All rights reserved.
//

#include "imageDetails.h"

float ImageDetails::vertical; // acceptable slope to consider vertical
int ImageDetails::proximity;  // temp

ImageDetails::ImageDetails(string imgLoc) {

	imageLocation = imgLoc;
	vertical = 4; // acceptable slope to consider vertical
	proximity = 80; // temp
}

Mat* ImageDetails::openOriginal() {
    string matName = "original";
    Mat image = imread(imageLocation);
    mats[matName] = image;
    showImage(matName, image);
    cvWaitKey(0);
	return &image;
}

Mat* ImageDetails::getMat(string imageName) {
	return &mats[imageName];
}

Mat* ImageDetails::insertMat(string matName, Mat imageMat) {
    mats[matName] = imageMat;
    return &mats[matName];
}

vector<Vec4i>* ImageDetails::getLineList(string lineListName) {
    return &lineLists[lineListName];
}

vector<Vec4i>* ImageDetails::insertLineList(string lineListName, vector<Vec4i> lineList) {
    lineLists[lineListName] = lineList;
	return &lineLists[lineListName];
}

void ImageDetails::showImage(string windowName, Mat mat) {
    namedWindow(windowName);
    imshow(windowName, mat);
}