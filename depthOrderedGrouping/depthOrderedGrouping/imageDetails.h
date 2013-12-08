//
//  imageDetails.h
//  depthOrderedGrouping
//
//  Created by Lars Jangaard on 11/28/13.
//  Copyright (c) 2013 Lars Jangaard. All rights reserved.
//
#pragma once
#ifndef __depthOrderedGrouping__imageDetails__
#define __depthOrderedGrouping__imageDetails__

#include <iostream>
#include <map>
#include <vector>
#include <string.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace cv;
using namespace std;

class ImageDetails {
  public:
    ImageDetails(string imgLoc);
    
    Mat* openOriginal();
    Mat* getMat(string);
    Mat* insertMat(string, Mat);
	Mat* insertMat(string);
    
    vector<Vec4i>* getLineList(string);
<<<<<<< HEAD
    vector<Vec4i>* insertLineList(string, vector<Vec4i>);

	static float vertical; // acceptable slope to consider vertical
	static int proximity;  // temp
=======
    vector<Vec4i>* insertLineList(string, vector<Vec4i>*);
	vector<Vec4i>* insertLineList(string);

	vector<Point>* getPointList(string);
	vector<Point>* insertPointList(string, vector<Point>);
	vector<Point>* insertPointList(string);
>>>>>>> convertMain
    
  private:
	void showImage(string, Mat);
	
<<<<<<< HEAD
	string imageLocation;
    map<string, Mat> mats;
    map<string, vector<Vec4i>> lineLists;
=======
    map<string, Mat> Mats;
    map<string, vector<Vec4i>> LineLists;
	map<string, vector<Point>> PointLists;

    int proximity; // for vanishing point regions
    float vertical; // acceptable slope to consider vertical
>>>>>>> convertMain
};

#endif /* defined(__depthOrderedGrouping__imageDetails__) */
