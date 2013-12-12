#ifndef __depthOrderedGrouping__QuadFinder__
#define __depthOrderedGrouping__QuadFinder__

#define _CRT_SECURE_NO_WARNINGS

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
		vector<vector<vector<Point>>>* getQuads();

		void displayQuad(string, vector<Point>, Mat);
		void displayQuads(string, vector<vector<Point>>);
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

		double lineDist(Vec4i);
		vector<Point> findClosestPnt(Vec4i, Vec4i);

		vector<vector<Point>> leftQuads;
		vector<vector<Point>> rightQuads;
		vector<vector<Point>> vertQuads;

		// private data
		ImageDetails* imageDetails;
		vector<vector<Point>*>* quadCand;

		Mat displayQuadMat;
		Mat displayQuadsMat;
};

#endif /* defined(__depthOrderedGrouping__QuadFinder__) */