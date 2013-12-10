#ifndef __depthOrderedGrouping__QuadFinder__
#define __depthOrderedGrouping__QuadFinder__

#include <iostream>
#include <string.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <math.h>

#include "imageDetails.h"

using namespace cv;

class QuadFinder
{
	public:
		QuadFinder(ImageDetails*);
		vector<vector<Point>*>* getQuads();
	
	private:
		void findCloseLines();
		bool closeEnough(Vec4i, Vec4i);
		vector<Point> completeQuad(Vec4i, String, Vec4i, String);

		Vec4i createNewLine(Point, Point);
		Vec4i extendLine(Vec4i, Vec4i);
		vector<Point> furthestPnt(Vec4i, Vec4i);

		double getLineSlope(Vec4i);
		double getLineIntercept(Vec4i, double);
		Point findLineIntercepts(Vec4i, Vec4i);

		ImageDetails* imageDetails;
		vector<vector<Point>*>* quadCand;
		vector<Point> findClosestPnt(Vec4i, Vec4i);

		double lineDist(Vec4i);
};

#endif /* defined(__depthOrderedGrouping__QuadFinder__) */