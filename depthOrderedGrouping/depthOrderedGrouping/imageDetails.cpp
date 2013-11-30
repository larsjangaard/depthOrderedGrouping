//
//  imageDetails.cpp
//  depthOrderedGrouping
//
//  Created by Lars Jangaard on 11/28/13.
//  Copyright (c) 2013 Lars Jangaard. All rights reserved.
//

#include "imageDetails.h"

ImageDetails::ImageDetails() {

}

Mat* ImageDetails::openOriginal(string imageLocation) {
    string matName = "original";
    Mat image = imread(imageLocation);
    
    Mats[matName] = image;
    
    showImage(matName, image);
    
    cvWaitKey(0);

	return &image;
}

Mat* ImageDetails::getMat(string imageName) {
	return &Mats[imageName];
}

Mat* ImageDetails::insertMat(string matName, Mat imageMat) {
    Mats[matName] = imageMat;
    return &Mats[matName];
}

vector<Vec4i>* ImageDetails::getLineList(string lineListName) {
    return &LineLists[lineListName];
}

vector<Vec4i>* ImageDetails::insertLineList(string lineListName, vector<Vec4i> lineList) {
    LineLists[lineListName] = lineList;
	return &LineLists[lineListName];
}

void ImageDetails::showImage(string windowName, Mat mat) {
    namedWindow(windowName);
    imshow(windowName, mat);
}