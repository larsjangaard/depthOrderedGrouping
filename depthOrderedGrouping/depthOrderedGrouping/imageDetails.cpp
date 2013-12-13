//
//  imageDetails.cpp
//  
//  Implementation of the imageDetails class. Allows Mats,
//  vector<Vec4i>, and vector<Point> lists to be stored
//  and managed by the imageDetails class.
//
//  Lars Jangaard

#include "imageDetails.h"
#include <string.h>

// constructor
// preconditions: none.
// postconditions: none.
//
ImageDetails::ImageDetails() {

}

// openOriginal - opens the image that will be depth ordered.
// preconditions: image at imageLocation must exist.
// postconditions: original image is inserted in Mats and
//                 the pointer is returned.
//
Mat* ImageDetails::openOriginal(string imageLocation) {
    string matName = "original";
    Mat image = imread(imageLocation);
    
    Mats[matName] = image;
    showImage(matName, image);

	return &image;
}

// getMat - returns the mat in Mats with key imageName.
// preconditions: image at Mat[imageName] must exist.
// postconditions: pointer to the mat is returned.
//
Mat* ImageDetails::getMat(string imageName) {
	return &Mats[imageName];
}

// insertMat - inserts and returns the mat in Mats with key imageName.
//             If a Mat[imageName] already exists its overwritten.
// preconditions: none.
// postconditions: image is inserted or overwritten, the pointer to
//                 the mat is returned.
//
Mat* ImageDetails::insertMat(string matName, Mat imageMat) {
    Mats[matName] = imageMat;
    return &Mats[matName];
}

// insertMat - inserts a new mat and returns a pointer to the mat.
//             If a Mat[imageName] already exists its overwritten.
// preconditions: none.
// postconditions: new empty image is inserted or overwritten, 
//                 the pointer to the mat is returned.
//
Mat* ImageDetails::insertMat(string matName) {
	Mats[matName] = *(new Mat);
	return &Mats[matName];
}

// getLineList - gets the list stored in LineLists and returns it.
// preconditions: LineLists[lineListName] exists.
// postconditions: returns a pointer to the list.
//
vector<Vec4i>* ImageDetails::getLineList(string lineListName) {
    return &LineLists[lineListName];
}

// insertLineList - inserts lineList into LineList[lineListName]
// preconditions: none.
// postconditions: inserts the list, overwritting the previous list if it existed.
//
vector<Vec4i>* ImageDetails::insertLineList(string lineListName, vector<Vec4i> *lineList) {
    LineLists[lineListName] = *lineList;
	return &LineLists[lineListName];
}

// insertLineList - inserts a new line list into LineList[lineListName]
// preconditions: none.
// postconditions: inserts the new list, overwritting the 
//                 previous list if it existed.
//
vector<Vec4i>* ImageDetails::insertLineList(string lineListName) {
    LineLists[lineListName] = *(new vector<Vec4i>);
	return &LineLists[lineListName];
}

// showImage - displays the Mat in window windowName
// preconditions: none.
// postconditions: Mat is displayed.
//
void ImageDetails::showImage(string windowName, Mat mat) {
    namedWindow(windowName);
    imshow(windowName, mat);
}

// getPointList - returns PointLists[pointListName]
// preconditions: PointLists pointListName must exist.
// postconditions: the point list is returned.
//
vector<Point>* ImageDetails::getPointList(string pointListName) {
	return &PointLists[pointListName];
}

// getPointList - insert and returns PointLists[pointListName]
// preconditions: none.
// postconditions: point list is inserted and returned. Overwrites
//                 the original if it existed.
//
vector<Point>* ImageDetails::insertPointList(string pointListName, vector<Point> pointList) {
	PointLists[pointListName] = pointList;
	return &PointLists[pointListName];
}

// getPointList - inserts new list and returns PointLists[pointListName]
// preconditions: none.
// postconditions: point list is inserted and returned. Overwrites
//                 the original if it existed.
//
vector<Point>* ImageDetails::insertPointList(string pointListName) {
	PointLists[pointListName] = *(new vector<Point>);
	return &PointLists[pointListName];
}

// appendLineList - appends the line list at org with line list app
// preconditions: org must exist in LineList.
// postconditions: line list app is appended to line list org.
//
vector<Vec4i>* ImageDetails::appendLineList(string org, vector<Vec4i>* app) {
	vector<Vec4i>* orgLineList = getLineList(org);

	for(int i = 0; i < app->size(); i++) {
		orgLineList->push_back(app->at(i));
	}
	
	return orgLineList;
}

// lineListContains - returns true if listName is contained in LineLists
// preconditions: none.
// postconditions: true is returned if list is found, otherwise false.
//
bool ImageDetails::lineListContains(string listName) {
	map<string, vector<Vec4i>>::iterator it = LineLists.find(listName);

	if(it == LineLists.end()) {
		return false;
	}

	return true;
}

// removeLineList - removes the line list at LineLists[listName]
// preconditions: LineLists[listName] must exist.
// postconditions: LineLists[listName] is removed from LineLists.
//
void ImageDetails::removeLineList(string listName) {
	LineLists.erase(listName);
}