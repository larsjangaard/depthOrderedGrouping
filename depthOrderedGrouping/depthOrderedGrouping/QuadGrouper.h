#ifndef __depthOrderedGrouping__quadGrouper__
#define __depthOrderedGrouping__quadGrouper__

#pragma once
#include <vector>
#include <string.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "imageDetails.h"
#include "clipper.hpp"

using namespace cv;
using namespace std;
using namespace ClipperLib;

class QuadGrouper {
public:
	QuadGrouper(ImageDetails*, vector<vector<vector<Point>>>*);
	~QuadGrouper(void);
	void groupQuads();

	vector<vector<Point>> *groupedLeftQuads,
						  *groupedRightQuads,
						  *groupedVertQuads;

private:

	ImageDetails *imgDets;
	vector<vector<vector<Point>>> ungroupedQuads;

	void groupQuadVec(vector<vector<Point>>, vector<vector<Point>>*&);
	bool tryCombine(vector<Point>, vector<Point>, vector<Point>*&);
	bool getQuadIntxn(vector<Point>, vector<Point>, vector<Point>&);
	vector<Point> getQuadIntxn(vector<Point>, vector<Point>);
	vector<Point> rectify(vector<Point>);
	float getArea(vector<Point>);
	float getIntxnArea(vector<Point>, vector<Point>, vector<Point>);
	vector<Point> combineQuads(vector<Point>, vector<Point>);
	vector<Point> combineVertQuads(vector<Point>, vector<Point>);
	Point getIntxnPts(Point A1, Point A2, Point B1, Point B2);
	void drawQuads();
	void drawQuadsHelper(Mat &, vector<vector<Point>>, Scalar_<double>);
};

#endif