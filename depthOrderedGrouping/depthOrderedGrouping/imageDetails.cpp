//
//  imageDetails.cpp
//  depthOrderedGrouping
//
//  Created by Lars Jangaard on 11/28/13.
//  Copyright (c) 2013 Lars Jangaard. All rights reserved.
//

#include "imageDetails.h"
#include <string.h>

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

Mat* ImageDetails::insertMat(string matName) {
	Mats[matName] = *(new Mat); //*newMat;
	return &Mats[matName];
}

vector<Vec4i>* ImageDetails::getLineList(string lineListName) {
    return &LineLists[lineListName];
}

vector<Vec4i>* ImageDetails::insertLineList(string lineListName, vector<Vec4i> *lineList) {
    LineLists[lineListName] = *lineList;
	return &LineLists[lineListName];
}

vector<Vec4i>* ImageDetails::insertLineList(string lineListName) {
    LineLists[lineListName] = *(new vector<Vec4i>);
	return &LineLists[lineListName];
}

void ImageDetails::showImage(string windowName, Mat mat) {
    namedWindow(windowName);
    imshow(windowName, mat);
}

vector<Point>* ImageDetails::getPointList(string pointListName) {
	return &PointLists[pointListName];
}

vector<Point>* ImageDetails::insertPointList(string pointListName, vector<Point> pointList) {
	PointLists[pointListName] = pointList;
	return &PointLists[pointListName];
}

vector<Point>* ImageDetails::insertPointList(string pointListName) {
	PointLists[pointListName] = *(new vector<Point>);
	return &PointLists[pointListName];
}

vector<Vec4i>* ImageDetails::appendLineList(string org, vector<Vec4i>* app) {
	vector<Vec4i>* orgLineList = getLineList(org);

	for(int i = 0; i < app->size(); i++) {
		orgLineList->push_back(app->at(i));
	}
	
	return orgLineList;
}