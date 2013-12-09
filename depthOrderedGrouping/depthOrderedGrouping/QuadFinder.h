#ifndef __depthOrderedGrouping__QuadFinder__
#define __depthOrderedGrouping__QuadFinder__

#include <iostream>
#include <string.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <math.h>

#include "imageDetails.h"



class QuadFinder
{
	public:
		QuadFinder(ImageDetails*);
	
	private:
		void findCloseLines();
		bool closeEnough(Vec4i, Vec4i);
		void completeQuad(Vec4i, String, Vec4i, String);

		Vec4i createNewLine(Point, Point, Vec4i);
		Vec4i extendLine(Vec4i, Vec4i);
		Point furthestPnt(Vec4i, Vec4i);

		double getLineSlope(Vec4i);
		double getLineIntercept(Vec4i, double);
		Point findLineIntercepts(double, double, double, double);

		ImageDetails* imageDetails;
		vector<vector<Vec4i>*>* quadCand;


};

#endif /* defined(__depthOrderedGrouping__QuadFinder__) */