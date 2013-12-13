#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "imageDetails.h"
#include "LineFinder.h"
#include "QuadFinder.h"
#include "QuadGrouper.h"
#include "LineGrouping.h"

using namespace cv;

int main(int argc, char *argv[]) {
	ImageDetails imageDetails;

	imageDetails.openOriginal("borders2.jpg");

	Mat* img = imageDetails.getMat("original");

	namedWindow("Original");
	imshow("Original", *img);

	LineFinder lineFinder(&imageDetails);

	lineFinder.greyImage();
	lineFinder.blurImage();
	lineFinder.detectEdges();
	cvWaitKey(0);

	lineFinder.detectLines();
	cvWaitKey(0);

	lineFinder.findValidLines(imageDetails
		.getLineList("houghpResult"), "leftVanLines", "rightVanLines", "vertLines");

	QuadFinder* quadFinder = new QuadFinder(&imageDetails);
	vector<vector<vector<Point>>>* vec = quadFinder->getQuads();

	Scalar scalar[] = { Scalar(255,0,0), Scalar(0,255,0), Scalar(0,0,255) };

	Mat allQuads = imageDetails.getMat("original")->clone();
	for(int i = 0; i < vec->size(); i++) {
		for(int j = 0; j < vec->at(i).size(); j++) {
			vector<Point> pnts = vec->at(i).at(j);
			line(allQuads, pnts.at(0), pnts.at(1), scalar[i], 2);
			line(allQuads, pnts.at(1), pnts.at(2), scalar[i], 2);
			line(allQuads, pnts.at(2), pnts.at(3), scalar[i], 2);
			line(allQuads, pnts.at(0), pnts.at(3), scalar[i], 2);
			imageDetails.insertMat("allQuads", allQuads);
		}
	}

	namedWindow("ALLQUADS");
	imshow("ALLQUADS", *imageDetails.getMat("allQuads"));
	cvWaitKey(0);

	QuadGrouper *quadGrouper = new QuadGrouper(&imageDetails, vec);

	int argAr[] = { lineFinder.cannyThresh1, lineFinder.cannyThresh2, lineFinder.cannyAperture, 
		lineFinder.houghAccumulator, lineFinder.houghMinLen, lineFinder.houghMaxGap };

	string s;

	for(int i = 0; i < 6; i++) {
		string temp;
		stringstream ss;
		ss << argAr[i];
		ss >> temp;


		s = s.append(temp);

		if(i != 5)
			s.append("-");
	}

	imwrite("output\\quads-" + s + ".jpg", *imageDetails.getMat("allQuads"));
	return 0;
}
