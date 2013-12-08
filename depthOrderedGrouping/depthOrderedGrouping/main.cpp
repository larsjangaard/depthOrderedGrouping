#include <opencv/cv.h>
#include <opencv/highgui.h>
<<<<<<< HEAD
#include "ImageDetails.h"
#include "LineFinder.h"
using namespace cv;

=======
#include "imageDetails.h"
#include "LineFinder.h"

using namespace cv;

int main(int argc, char *argv[]) {
	ImageDetails imageDetails;

	imageDetails.openOriginal("borders.jpg");

	Mat* img = imageDetails.getMat("original");
>>>>>>> convertMain

	namedWindow("Original");
	imshow("Original", *img);

	vector<Vec4i> testVector;

<<<<<<< HEAD
    // Read in and display test image
	//ImageDetails *myImageDetails = new ImageDetails("159.jpg");
	ImageDetails *myImgDetails = new ImageDetails("borders.jpg");
    myImgDetails->openOriginal();
	
	LineFinder *myLineFinder = new LineFinder(myImgDetails);

	myLineFinder->greyImage();
	myLineFinder->blurImage();
	myLineFinder->detectEdges();
	myLineFinder->detectLines();
	myLineFinder->findInitGoodLines();

    return 0;
=======
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

	return 0;
>>>>>>> convertMain
}