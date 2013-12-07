// main.cpp
// This code groups edges into lines, line clusters, quadrilaterals, and
// depth-ordered planes based on cues in the single image
// Author: Lars Jangaard, Olajumoke Fajinmi, William Hoffrance
// Date: 11/3/2013

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "ImageDetails.h"
#include "LineFinder.h"
using namespace cv;



// main - This method performs greyscale conversion, blur, and canny operations to an image.
// preconditions - 159.jpg exists in the code directory and is a valid JPEG.
// postconditions - intermediate results are displayed on the screen and
//                                        the final result is saved to the file system as output.jpg.
int main(int argc, char *argv[]) {

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
}