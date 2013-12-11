#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "imageDetails.h"
#include "LineFinder.h"
#include "QuadFinder.h"
//#include "QuadGrouper.h"
#include "LineGrouping.h"

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

	LineFinder lineFinder(&imageDetails);

	lineFinder.greyImage();
	lineFinder.blurImage();
	lineFinder.detectEdges();

	cvWaitKey(0);
	lineFinder.detectLines();

	cvWaitKey(0);

	lineFinder.findValidLines(imageDetails.getLineList("houghpResult"), "leftVanLines", "rightVanLines", "vertLines");
	//lineFinder.findValidLines();

	cvWaitKey(0);

	QuadFinder* quadFinder = new QuadFinder(&imageDetails);

	vector<vector<Point>*>* vec = quadFinder->getQuads();

	LineGrouping lineGrouping(&lineFinder, &imageDetails);
	vector<vector<Vec4i>> retVec = lineGrouping.groupLines(*imageDetails.getMat("greyScale"));

	for(int i = 0; i < retVec.size(); i++) {
		cout << "VEC SIZE: " << retVec[i].size() << endl;
	}

	//QuadGrouper *quadGrouper = new QuadGrouper(&imageDetails);	
	//QuadSorter *quadSorter = new QuadSOrter(&imageDetails);
	cvWaitKey(0);

	return 0;
}
