// main.cpp
// This code groups edges into lines, line clusters, quadrilaterals, and
// depth-ordered planes based on cues in the single image
// Author: Lars Jangaard, Olajumoke Fajinmi, William Hoffrance
// Date: 11/3/2013

#include <opencv/cv.h>
#include <opencv/highgui.h>
using namespace cv;

struct CurImage {
	Mat original, blurred, edged, houghed, vanished;
	int thresh1;
	int thresh2;
	int aperture;
	int thresh3;
	int minLen;
	int maxGap;
	int proximity;
	vector<Vec4i> allLines, goodLines, vanPts;
	vector<Vec2i> votes;
	vector<vector<int>> buckets;

	CurImage(Mat orig, Mat blur, int t1=31, int t2=65, int ap=3, int t3=30, int len=12, int gap=2, int prox=100) {
		original = orig;
		blurred = blur;
		thresh1=t1;
		thresh2=t2;
		aperture=ap;
		thresh3=t3;
		minLen=len;
		maxGap=gap;
		proximity=prox;
	}
};

//transform1 - takes the blurred image and applies edge detection based on slidebar values
//preconditions - a valid CurImage struct exists with assigned values
//postconditions - transformed copy of image is displayed
void transform1(void *src) {
  	CurImage *curIn = (CurImage*) src;
	Canny(curIn->blurred, curIn->edged, curIn->thresh1, curIn->thresh2, curIn->aperture);
	imshow( "Output1", curIn->edged );
}

//transform2 - takes the edged image and applies houghlinesp based on slidebar values
//preconditions - a valid CurImage struct exists with assigned values
//postconditions - transformed copy of image is displayed ad lines vector is generated
void transform2(void *src) {
  	CurImage *curIn = (CurImage*) src;
	curIn->houghed = curIn->original.clone();
	HoughLinesP(curIn->edged, curIn->allLines, 1, CV_PI/180, curIn->thresh3, curIn->minLen, curIn->maxGap);

	for( int i = 0; i < curIn->allLines.size(); i++ ) {
		Vec4i l = curIn->allLines[i];
		line(curIn->houghed, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255));
	}
	imshow("Output2", curIn->houghed);
}

//transform3 - takes the lines from the houghed image and filters out those
//	with the fewest shared vanishing points
//preconditions - a valid CurImage struct exists with assigned values, including houghlines vector
//postconditions - transformed copy of image is displayed and new lines vector is generated
void transform3(void *src) {
	CurImage *curIn = (CurImage*) src;
	curIn->vanished = curIn->original.clone();

	for ( int i=0; i < curIn->allLines.size(); i++ ) {
		Vec4i line_i = curIn->allLines[i];
		float mi, ci, x1_i, y1_i, x2_i, y2_i, dx_i, dy_i;
		x1_i = line_i[0];	y1_i = line_i[1];
		x2_i = line_i[2];	y2_i = line_i[3];
		dx_i = x2_i - x1_i;	dy_i = y2_i - y1_i;

		if (dx_i == 0) dx_i = .0001;
			//curIn->goodLines.push_back(line_i);
		mi = dy_i / dx_i;
		ci = y1_i - (mi*x1_i);
		for ( int j=i+1; j < curIn->allLines.size(); j++ ) {
			Vec4i line_j = curIn->allLines[j];
			float mj, cj, x1_j, y1_j, x2_j, y2_j, dx_j, dy_j, intxnX, intxnY;
			x1_j = line_j[0];	y1_j = line_j[1];
			x2_j = line_j[2];	y2_j = line_j[3];
			dx_j = x2_j - x1_j;	dy_j = y2_j - y1_j;
			if (dx_j == 0) dx_j = -.0001;
				//curIn->goodLines.push_back(line_j);
			mj = dy_j / dx_j;
			cj = y1_j - (mj*x1_j);

			if( (mi - mj) == 0) { // parallel
				mi += .0001;
				mj -= .0001;
			}
			intxnX = (cj - ci) / (mi - mj);
			intxnY = (mi*intxnX) + ci;
			Vec4i vanPt;
			//if ( (intxnX < 0) || (intxnX > curIn->original.cols) ||
				//(intxnY < 0) || (intxnY > curIn->original.rows) ) {
			vanPt[0] = i;
			vanPt[1] = j;
			vanPt[2] = intxnX;
			vanPt[3] = intxnY;
			curIn->vanPts.push_back(vanPt);
			//}}}}
		}
	}
	for (int i=0; i<curIn->vanPts.size(); i++) {
		int voteX = curIn->vanPts[i][2];
		int voteY = curIn->vanPts[i][3];
		bool found = false;
		
		for (int j=0; j<curIn->votes.size() && found == false; j++) {
			if (abs(curIn->votes[j][0] - voteX) < curIn->proximity &&
				abs(curIn->votes[j][1] - voteY) < curIn->proximity) {
					curIn->buckets[j].push_back(i);
					found = true;
			}
		}
		if (!found) {
			Vec2i vote;
			vote[0] = voteX;
			vote[1] = voteY;
			curIn->votes.push_back(vote);

			vector<int> bucket;
			bucket.push_back(i);
			curIn->buckets.push_back(bucket);
		}
	}
	int hi3=0, hi2=0, hi1=0;
	int size3=0, size2=0, size1=0;
	int tmpHi, tmpSize;
	for (int i=0; i<curIn->buckets.size(); i++) {
		int curHi = i;
		int curSize = curIn->buckets[i].size();
		if (curSize > size1) {
			if (curSize > size2) {
				if (curSize > size3) {
					tmpHi = hi3;
					tmpSize = size3;
					hi3 = curHi;
					size3 = curSize;
					curHi = tmpHi;
					curSize = tmpSize;
				}
				tmpHi = hi2;
				tmpSize = size2;
				hi2 = curHi;
				size2 = curSize;
				curHi = tmpHi;
				curSize = tmpSize;
			}
			hi1 = curHi;
			size1 = curSize;
		}
	}
	vector<vector<int>> winners;
	winners.push_back(curIn->buckets[hi1]);
	winners.push_back(curIn->buckets[hi2]);
	winners.push_back(curIn->buckets[hi3]);

	for (int i=0; i < winners.size(); i++) {
		vector<int> bucket = winners[i];
		for (int j=0; j < bucket.size(); j++) {
			int vanPtsInd = bucket[j];
			Vec4i vanPt = curIn->vanPts[vanPtsInd];
			int allLinesInd1 = vanPt[0];
			int allLinesInd2 = vanPt[1];
			curIn->goodLines.push_back(curIn->allLines[allLinesInd1]);
			curIn->goodLines.push_back(curIn->allLines[allLinesInd2]);
		}
	}
	for( int i=0; i < curIn->goodLines.size(); i++ ) {
		Vec4i l = curIn->goodLines[i];
		line(curIn->vanished, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255));
	}
	imshow("Output3", curIn->vanished);
}

// on_trackbar1 - threshold 1
// the lower the threshold the more lines you get in non-edgy places
void on_trackbar1(int thresh1_slider, void *src) {
	( (CurImage*) src )->thresh1 = thresh1_slider;
	transform1(src);
}

// on_trackbar2 - threshold 2
// seems to have a relationship with threshold 1 but mostly does the same thing
void on_trackbar2(int thresh2_slider, void *src) {
	( (CurImage*) src )->thresh2 = thresh2_slider;
	transform1(src);
}

// on_trackbar3 - aperture
// compounds the edge detection.
// can only be 1, 3, 5, 7, but crashes on 1 so i don't allow it 
void on_trackbar3(int aperture_slider, void *src) {
	int aperSize = aperture_slider;
	if (aperSize < 5 ) aperSize = 3;
	else if (aperSize == 6) aperSize = 7;
	( (CurImage*) src )->aperture = aperSize;
	transform1(src);
}

// on_trackbar4 - hough threshold
void on_trackbar4(int thresh3_slider, void *src) {
	if (thresh3_slider==0) thresh3_slider=1;
	( (CurImage*) src )->thresh3 = thresh3_slider;
	transform2(src);
}

// on_trackbar5 - min line length
void on_trackbar5(int minLen_slider, void *src) {
	if (minLen_slider==0) minLen_slider=1;
	( (CurImage*) src )->thresh3 = minLen_slider;
	transform2(src);
}

// on_trackbar6 - hough threshold
void on_trackbar6(int maxGap_slider, void *src) {
	if (maxGap_slider==0) maxGap_slider=1;
	( (CurImage*) src )->thresh3 = maxGap_slider;
	transform2(src);
}

// on_trackbar7 - vanishing point region sizes
void on_trackbar7(int vanSize_slider, void *src) {
	if (vanSize_slider==0) vanSize_slider=1;
	( (CurImage*) src )->proximity = vanSize_slider;
	transform3(src);
}



// main - This method performs greyscale conversion, blur, and canny operations to an image.
// preconditions - 159.jpg exists in the code directory and is a valid JPEG.
// postconditions - intermediate results are displayed on the screen and
//					the final result is saved to the file system as output.jpg.
int main(int argc, char *argv[]) {

	// Read in and display test image
	Mat image = imread("159.jpg");     
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
	CurImage *curImage = new CurImage(image, blurred);
	
	transform1((void*)curImage);	// Perform default canny

	// Create trackbars for Canny
	createTrackbar("Thresh1", "Output1", &curImage->thresh1, 100, on_trackbar1, &*curImage);
	createTrackbar("Thresh2", "Output1", &curImage->thresh2, 300, on_trackbar2, &*curImage);
	createTrackbar("Aperture", "Output1", &curImage->aperture, 7, on_trackbar3, &*curImage);
	
	cvWaitKey(0);
	imwrite("edged.jpg", curImage->edged);	// save canny result

	namedWindow("Output2");
	transform2((void*)curImage);	// Perform default houghlinesp

	// Create trackbars for HoughlinesP
	createTrackbar("Thresh", "Output2", &curImage->thresh3, 150, on_trackbar4, &*curImage);
	createTrackbar("Min Length", "Output2", &curImage->minLen, 100, on_trackbar5, &*curImage);
	createTrackbar("Max Gap", "Output2", &curImage->maxGap, 100, on_trackbar6, &*curImage);

	cvWaitKey(0);
	imwrite("houghed.jpg", curImage->houghed); // save houghed result

	namedWindow("Output3");
	transform3((void*)curImage);
	createTrackbar("prox", "Output3", &curImage->proximity, 300, on_trackbar7, &*curImage);

	cvWaitKey(0);
	imwrite("vanished.jpg", curImage->vanished);

	return 0;
}