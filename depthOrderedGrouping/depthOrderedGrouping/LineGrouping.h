#pragma once

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <math.h>
#include "imageDetails.h"
#include "LineFinder.h"
#include <iostream>

using namespace cv;
using namespace std;


class LineGrouping
{
public:
	LineGrouping(void);
	vector<vector<Vec4i>> groupLines(Mat, LineFinder*, ImageDetails*);
	~LineGrouping(void);

private:
	double getOverlappingRatio(Vec2i, int );
	double angularDifference(Vec4i, Vec4i);
	Vec2i findMinPoint(Vec4i);
	double calcCurvilinearity(double , double , double);
	double calcParallelism(int , double , double );
	double calcOrthogonality(int, double);
	void displayLines(vector<vector<Vec4i>>& , Mat );
	vector<vector<Vec4i>> reclusterLines(LineFinder*,vector<Vec4i>, vector<Vec4i>, vector<Vec4i>);
	vector<Vec4i> mergeLineVector (ImageDetails*);

	ImageDetails* imageDetails;
	LineFinder* lineFinder
};



