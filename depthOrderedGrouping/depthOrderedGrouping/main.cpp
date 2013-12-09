#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "imageDetails.h"
#include "LineFinder.h"
#include "QuadFinder.h"

using namespace cv;

int main(int argc, char *argv[]) {
	ImageDetails imageDetails;

	imageDetails.openOriginal("borders.jpg");

	Mat* img = imageDetails.getMat("original");

	namedWindow("Original");
	imshow("Original", *img);

	vector<Vec4i> testVector;

	imageDetails.insertLineList("testVector", &testVector);

	vector<Vec4i>* returnVector = imageDetails.getLineList("testVector");

	LineFinder* lineFinder = new LineFinder(&imageDetails);

	lineFinder->greyImage();
	lineFinder->blurImage();
	lineFinder->detectEdges();

	cvWaitKey(0);
	lineFinder->detectLines();

	cvWaitKey(0);

	lineFinder->findValidLines();

	cvWaitKey(0);

	QuadFinder* quadFinder = new QuadFinder(&imageDetails);

	return 0;
}