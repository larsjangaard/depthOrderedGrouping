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

	imageDetails.openOriginal("borders.jpg");

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

	lineFinder.findValidLines(imageDetails.getLineList("houghpResult"), "leftVanLines", "rightVanLines", "vertLines");

	//cvWaitKey(0);

	cout << "Found " << imageDetails.getLineList("leftVanLines")->size() + imageDetails.getLineList("rightVanLines")->size()
		+ imageDetails.getLineList("vertLines")->size() << " lines." << endl;

	cout << "LeftVanLines: " << imageDetails.getLineList("leftVanLines")->size() << endl;
	cout << "LeftVanLines: " << imageDetails.getLineList("rightVanLines")->size() << endl;
	cout << "LeftVanLines: " << imageDetails.getLineList("vertLines")->size() << endl;

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

	//LineGrouping lineGrouping(&lineFinder, &imageDetails);
	//vector<vector<Vec4i>> retVec = lineGrouping.groupLines(*imageDetails.getMat("greyScale"));

	//for(int i = 0; i < retVec.size(); i++) {
	//	cout << "VEC SIZE: " << retVec[i].size() << endl;
	//}

	QuadGrouper *quadGrouper = new QuadGrouper(&imageDetails, vec);
	//QuadSorter *quadSorter = new QuadSOrter(&imageDetails);

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

	imwrite("output\\quads-" + s + ".jpg", *imageDetails.getMat("original"));
	return 0;
}
