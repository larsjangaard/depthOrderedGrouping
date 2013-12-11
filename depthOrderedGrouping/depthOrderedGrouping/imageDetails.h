//
//  imageDetails.h
//  depthOrderedGrouping
//
//  Created by Lars Jangaard on 11/28/13.
//  Copyright (c) 2013 Lars Jangaard. All rights reserved.
//

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
    ImageDetails();
    
    Mat* openOriginal(string);
	
    Mat* getMat(string);
    Mat* insertMat(string, Mat);
	Mat* insertMat(string);
    
    vector<Vec4i>* getLineList(string);
    vector<Vec4i>* insertLineList(string, vector<Vec4i>*);
	vector<Vec4i>* insertLineList(string);

	vector<Point>* getPointList(string);
	vector<Point>* insertPointList(string, vector<Point>);
	vector<Point>* insertPointList(string);

	vector<Vec4i>* appendLineList(string, vector<Vec4i>*);
	bool lineListContains(string);
  private:
	void showImage(string, Mat);
	
    map<string, Mat> Mats;
    map<string, vector<Vec4i>> LineLists;
	map<string, vector<Point>> PointLists;

    int proximity; // for vanishing point regions
    float vertical; // acceptable slope to consider vertical
};

#endif /* defined(__depthOrderedGrouping__imageDetails__) */
