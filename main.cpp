// main.cpp
// This code groups edges into lines, line clusters, quadrilaterals, and
// depth-ordered planes based on cues in the single image
// Author: Lars Jangaard, Olajumoke Fajinmi, William Hoffrance
// Date: 11/3/2013

#include <opencv/cv.h>
#include <opencv/highgui.h>
using namespace cv;

// struct holds the images and data we want to maintain
// while processing through different methods
struct TheImage {
	Mat original, blurred, edged, houghed, vanished;
	int thresh1, thresh2, aperture; // for Canny
	int thresh3, minLen, maxGap; // for HoughlinesP
	int proximity; // for vanishing point regions
	float vertical; // acceptable slope to consider vertical

	//*******************************************************//
	// each Vec4i holds 2 pair of line segment coords
	//*******************************************************//
	
	// HoughlinesP result
	vector<Vec4i> allLines; 
	
	// vanishing point filter results
	// holds vectors of leftLines, rightLines, and vertLines
	//vector<vector<Vec4i>> goodLines; 
	// vector<Vec4i> vertLines, leftLines, rightLines;
	
	// collections of indeces for each pair of 'allLines', along with the coords
	// for their intersection on either the left or right side of the image.
	//vector<vector<Vec4i>> allVanPts;


	// constructor w/ default values
	TheImage(Mat orig, Mat blur, int t1=18, int t2=65, int ap=3,
			 int t3=40, int len=10, int gap=2, int prox=80, float vert=4) {
		original = orig;
		blurred = blur;
		thresh1=t1;
		thresh2=t2;
		aperture=ap;
		thresh3=t3;
		minLen=len;
		maxGap=gap;
		proximity=prox;
		vertical=vert;
	}
};


// findVanishPts
// finds your two vanishing points (left and right)
// input the image data object (TheImage) and
// vector of line vectors to be populated
// (outer vector has 3 elements: left, right, and vertical lines)
// optional vectors for all vanishing points and mean vanishing points
// if you want them returned.
void findGoodLines(TheImage *myImg,
				   vector<Vec4i> *myLines,
				   vector<Vec4i> *leftLines,
				   vector<Vec4i> *rightLines,
				   vector<Vec4i> *vertLines,
				   vector<vector<Vec4i>> *allVanPts = new vector<vector<Vec4i>>,
				   vector<Point> *meanVanPts = new vector<Point>) {

	vector<Vec4i> leftVanPts, rightVanPts;

	// go thru all the houghline segments and get their coords/slopes/intercepts
	for ( int i=0; i < myLines->size()-1; i++ ) {
		Vec4i line_i = myLines->at(i);
		float mi, ci, x1_i, y1_i, x2_i, y2_i, dx_i, dy_i;
		x1_i = line_i[0];	y1_i = line_i[1];
		x2_i = line_i[2];	y2_i = line_i[3];
		dx_i = x2_i - x1_i;	dy_i = y2_i - y1_i;

		if (dx_i == 0) dx_i = .0001; // vertical
		mi = -(dy_i / dx_i); // slope
		if (abs(mi) > myImg->vertical) vertLines->push_back(line_i); // this will go to 'goodLines' by default
		else {
			ci = y1_i - (mi*x1_i); // intercept

			// do the same thing for every other line to find intersection
			for ( int j=i+1; j < myLines->size(); j++ ) {
				Vec4i line_j = myLines->at(j);
				float mj, cj, x1_j, y1_j, x2_j, y2_j, dx_j, dy_j, intxnX, intxnY;
				x1_j = line_j[0];	y1_j = line_j[1];
				x2_j = line_j[2];	y2_j = line_j[3];
				dx_j = x2_j - x1_j;	dy_j = y2_j - y1_j;
				if (dx_j == 0) dx_j = -.0001; // vertical
				mj = -(dy_j / dx_j); // slope
				
				if (abs(mj) > myImg->vertical && j == myLines->size()-1)
					vertLines->push_back(line_j);
				else {
					cj = y1_j - (mj*x1_j);

					if( (mi - mj) == 0) { // parallel
						mi += .0001;
						mj -= .0001;
					}

					// intersections
					intxnX = (cj - ci) / (mi - mj);
					intxnY = (mi*intxnX) + ci;
					Vec4i curVanPt;

					if ( (intxnY > 0) && (intxnY < myImg->original.rows) ) {
						curVanPt[0] = i;
						curVanPt[1] = j;
						curVanPt[2] = intxnX;
						curVanPt[3] = intxnY;

						// collect the good vanishing points
						// CHANGE FOR FINAL
						if (intxnX < 265 && intxnX > 0) leftVanPts.push_back(curVanPt);
						else if (intxnX > 650 && intxnX < myImg->original.cols) rightVanPts.push_back(curVanPt);
					}
				}
			}
		}
	}
	// 2-element vector, holds left and right vanishing point vectors
	allVanPts->push_back(leftVanPts);
	allVanPts->push_back(rightVanPts);

	Mat leftLabels, rightLabels;
	Mat leftCenters, rightCenters;
	Mat leftMat(leftVanPts.size(), 2, CV_32F);
	Mat rightMat(rightVanPts.size(), 2, CV_32F);
	const int ITERNS = 10;		// iterations
	const int ATMPTS = 1;		// attempts
	
	for (int i=0; i < leftVanPts.size(); i++) {
		leftMat.at<float>(i,0) = (float)leftVanPts[i][2];
		leftMat.at<float>(i,1) = (float)leftVanPts[i][3];
	}
	kmeans(leftMat, 1, leftLabels, TermCriteria(CV_TERMCRIT_ITER, ITERNS, 1.0), ATMPTS, KMEANS_PP_CENTERS, leftCenters);
	int leftX = (int)leftCenters.at<float>(0,0);
	int leftY = (int)leftCenters.at<float>(0,1);
	meanVanPts->push_back(Point(leftX, leftY));

	for (int i=0; i < rightVanPts.size(); i++) {
		rightMat.at<float>(i,0) = (float)rightVanPts[i][2];
		rightMat.at<float>(i,1) = (float)rightVanPts[i][3];
	}
	kmeans(rightMat, 1, rightLabels, TermCriteria(CV_TERMCRIT_ITER, ITERNS, 1.0), ATMPTS, KMEANS_PP_CENTERS, rightCenters);
	int rightX = (int)rightCenters.at<float>(0,0);
	int rightY = (int)rightCenters.at<float>(0,1);
	meanVanPts->push_back(Point(rightX, rightY));

	// go thru all the vanishing points
	for (int i=0; i < allVanPts->size(); i++) {
		vector<Vec4i> curVanPts = allVanPts->at(i);
		vector<Vec4i> *curGoodLines = (i==0) ? leftLines : rightLines;
		float curMeanX = meanVanPts->at(i).x;
		float curMeanY = meanVanPts->at(i).y;

		for (int j=0; j < curVanPts.size(); j++) {
			Vec4i curVanPt = curVanPts[j];
			circle(myImg->vanished, Point(curVanPt[2], curVanPt[3]),3, Scalar(0,255,0));
			int curVanPtX = curVanPt[2];
			int curVanPtY = curVanPt[3];
			if (abs(curMeanX - curVanPtX) < myImg->proximity &&
				abs(curMeanY - curVanPtY) < myImg->proximity) {
					int allLinesInd0 = curVanPt[0];
					int allLinesInd1 = curVanPt[1];
					curGoodLines->push_back(myImg->allLines[allLinesInd0]);
					curGoodLines->push_back(myImg->allLines[allLinesInd1]);
			}
		}
	}
	
}



//transform1 - takes the blurred image and applies edge detection based on slidebar values
//preconditions - a valid TheImage struct exists with assigned values
//postconditions - transformed copy of image is displayed
void transform1(void *src) {
  	TheImage *myImg = (TheImage*) src;
	Canny(myImg->blurred, myImg->edged, myImg->thresh1, myImg->thresh2, myImg->aperture);
	imshow( "Output1", myImg->edged );
}

//transform2 - takes the edged image and applies houghlinesp based on slidebar values
//preconditions - a valid TheImage struct exists with assigned values
//postconditions - transformed copy of image is displayed ad lines vector is generated
void transform2(void *src) {
  	TheImage *myImg = (TheImage*) src;
	myImg->houghed = myImg->original.clone();
	HoughLinesP(myImg->edged, myImg->allLines, 1, CV_PI/180, myImg->thresh3, myImg->minLen, myImg->maxGap);

	//vector<Vec2f> lines;
	//HoughLines(myImg->edged, lines, 1, CV_PI/180, myImg->thresh3, 0, 0 );	
	//for( size_t i = 0; i < lines.size(); i++ ) {
	//	float rho = lines[i][0], theta = lines[i][1];
	//	Point pt1, pt2;
	//	double a = cos(theta), b = sin(theta);
	//	double x0 = a*rho, y0 = b*rho;
	//	pt1.x = cvRound(x0 + 1000*(-b));
	//	pt1.y = cvRound(y0 + 1000*(a));
	//	pt2.x = cvRound(x0 - 1000*(-b));
	//	pt2.y = cvRound(y0 - 1000*(a));
	//	line( myImg->houghed, pt1, pt2, Scalar(0,0,255), 1, CV_AA);
 // }


	for( int i = 0; i < myImg->allLines.size(); i++ ) {
		Vec4i l = myImg->allLines[i];
		line(myImg->houghed, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255));
	}
	imshow("Output2", myImg->houghed);
}

//transform3 - takes the lines from the houghed image and filters out those
//	with the fewest shared vanishing points
//preconditions - a valid TheImage struct exists with assigned values, including houghlines vector
//postconditions - transformed copy of image is displayed and new lines vector is generated
void transform3(void *src) {
	TheImage *myImg = (TheImage*) src;
	myImg->vanished = myImg->original.clone();
	vector<Vec4i> leftLines, rightLines, vertLines; 
	vector<vector<Vec4i>> allVanPts, goodLines;
	vector<Point> meanVanPts;

	findGoodLines(myImg, &myImg->allLines, &leftLines, &rightLines, &vertLines, &allVanPts, &meanVanPts);
	goodLines.push_back(leftLines);
	goodLines.push_back(rightLines);
	goodLines.push_back(vertLines);

	// draw all the 'goodLines'
	for( int i=0; i < goodLines.size(); i++ ) {
		vector<Vec4i> curLines = goodLines[i];
		for( int j=0; j < curLines.size(); j++) {
			Vec4i l = curLines[j];
			line(myImg->vanished, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255));
		}
	}
	circle(myImg->vanished, meanVanPts[0], 6, Scalar(0,100,0), 1);
	circle(myImg->vanished, meanVanPts[1], 6, Scalar(0,100,0), 1);
	imshow("Output3", myImg->vanished);
}

// on_trackbar1 - threshold 1
// the lower the threshold the more lines you get in non-edgy places
void on_trackbar1(int thresh1_slider, void *src) {
	( (TheImage*) src )->thresh1 = thresh1_slider;
	transform1(src);
}

// on_trackbar2 - threshold 2
// seems to have a relationship with threshold 1 but mostly does the same thing
void on_trackbar2(int thresh2_slider, void *src) {
	( (TheImage*) src )->thresh2 = thresh2_slider;
	transform1(src);
}

// on_trackbar3 - aperture
// compounds the edge detection.
// can only be 1, 3, 5, 7, but crashes on 1 so i don't allow it 
void on_trackbar3(int aperture_slider, void *src) {
	int aperSize = aperture_slider;
	if (aperSize < 5 ) aperSize = 3;
	else if (aperSize == 6) aperSize = 7;
	( (TheImage*) src )->aperture = aperSize;
	transform1(src);
}

// on_trackbar4 - hough threshold
void on_trackbar4(int thresh3_slider, void *src) {
	if (thresh3_slider==0) thresh3_slider=1;
	( (TheImage*) src )->thresh3 = thresh3_slider;
	transform2(src);
}

// on_trackbar5 - min line length
void on_trackbar5(int minLen_slider, void *src) {
	if (minLen_slider==0) minLen_slider=1;
	( (TheImage*) src )->thresh3 = minLen_slider;
	transform2(src);
}

// on_trackbar6 - hough threshold
void on_trackbar6(int maxGap_slider, void *src) {
	if (maxGap_slider==0) maxGap_slider=1;
	( (TheImage*) src )->thresh3 = maxGap_slider;
	transform2(src);
}

// on_trackbar7 - vanishing point region sizes
void on_trackbar7(int vanSize_slider, void *src) {
	if (vanSize_slider==0) vanSize_slider=1;
	( (TheImage*) src )->proximity = vanSize_slider;
	transform3(src);
}



// main - This method performs greyscale conversion, blur, and canny operations to an image.
// preconditions - 159.jpg exists in the code directory and is a valid JPEG.
// postconditions - intermediate results are displayed on the screen and
//					the final result is saved to the file system as output.jpg.
int main(int argc, char *argv[]) {

	// Read in and display test image
	//Mat image = imread("159.jpg");
	Mat image = imread("borders.jpg");
	namedWindow("Input");
	imshow("Input", image);
	cvWaitKey(0);
	
	// Perform a greyscale conversion and display result
	Mat greyed;
	cvtColor(image, greyed, CV_BGR2GRAY);
	namedWindow("Output1");
	imshow("Output1", greyed);
	cvWaitKey(0);

	// Perform blur and display result
	Mat blurred;
	GaussianBlur(greyed, blurred, Size(5, 5), 2.0, 2.0);
	//blur(greyed, blurred, Size(3, 3));
	imshow("Output1", blurred);
	cvWaitKey(0);

	// Create image struct to retain a reference to the previous image
	// and keep track of slider values
	TheImage *myImage = new TheImage(image, blurred);
	
	transform1((void*)myImage);	// Perform default canny

	// Create trackbars for Canny
	createTrackbar("Thresh1", "Output1", &myImage->thresh1, 100, on_trackbar1, &*myImage);
	createTrackbar("Thresh2", "Output1", &myImage->thresh2, 300, on_trackbar2, &*myImage);
	createTrackbar("Aperture", "Output1", &myImage->aperture, 7, on_trackbar3, &*myImage);
	
	cvWaitKey(0);
	imwrite("edged.jpg", myImage->edged);	// save canny result

	namedWindow("Output2");
	transform2((void*)myImage);	// Perform default houghlinesp

	// Create trackbars for HoughlinesP
	createTrackbar("Thresh", "Output2", &myImage->thresh3, 150, on_trackbar4, &*myImage);
	createTrackbar("Min Length", "Output2", &myImage->minLen, 100, on_trackbar5, &*myImage);
	createTrackbar("Max Gap", "Output2", &myImage->maxGap, 100, on_trackbar6, &*myImage);

	cvWaitKey(0);
	imwrite("houghed.jpg", myImage->houghed); // save houghed result

	namedWindow("Output3");
	transform3((void*)myImage);

	// Trackbar for vanishing points filter (goes slow)
	createTrackbar("prox", "Output3", &myImage->proximity, 300, on_trackbar7, &*myImage);

	cvWaitKey(0);
	imwrite("vanished.jpg", myImage->vanished);

	return 0;
}