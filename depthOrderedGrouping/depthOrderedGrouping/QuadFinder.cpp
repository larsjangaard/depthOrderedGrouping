//
//  QuadFinder.cpp
//  
//  Implementation of the QuadFinder class. Uses an ImageDetails
//  object to find Quadrilaterals from grouped lines. Uses
//  leftVanLines, rightVanLines, and vertLines provided by
//  the LineFinder class.
//
//  Lars Jangaard

#include "QuadFinder.h"

// constructor: stores the point to imageDetails
// and creates new mats from the original image.
// preconditions: imageDetails with leftVanLines, rightVanLines,
//                and vertLines lists must exist.
// postconditions: class variables are instantiated.
QuadFinder::QuadFinder(ImageDetails* img) {
	imageDetails = img;
	quadCand = new vector<vector<Point>*>;

	displayQuadMat = imageDetails->getMat("original")->clone();
	displayQuadsMat = imageDetails->getMat("original")->clone();
}

// QuadFinder::getQuads()
// returns a vector of quadrilaterals for each vanishing point.
// Points in each quadrilateral vector are arranged to be
// displayed as follows:
//        
//        P(0) +-----------+ P(1)
//             |           |
//             |           |
//             |           |
//        P(3) +-----------+ P(2)
//
//        Line1 = Vec4i(P(0).x, P(0).y, P(1).x, P(1).y);
//        Line1 = Vec4i(P(1).x, P(1).y, P(2).x, P(2).y);
//        Line1 = Vec4i(P(2).x, P(2).y, P(3).x, P(3).y);
//        Line1 = Vec4i(P(3).x, P(3).y, P(0).x, P(0).y);
//
vector<vector<vector<Point>>>* QuadFinder::getQuads() {
	findCloseLines();

	// insert vectors into return vector.
	vector<vector<vector<Point>>>* quads = new vector<vector<vector<Point>>>;

	quads->push_back(leftQuads);
	quads->push_back(rightQuads);
	quads->push_back(vertQuads);

	return quads;
}

// QuadFinder::findCloseLines()
//
// Preconditions: leftVanLines, rightVanLines, and vertLines exist in imageDetails.
// Postconditions: valid quadrilaterals are saved to leftQuads, rightQuads, and vertQuads.
//
void QuadFinder::findCloseLines() {
	vector<vector<Vec4i>> vanLines;

	String leftVanLines = "leftVanLines";
	String rightVanLines = "rightVanLines";
	String vertLines = "vertLines";

	vanLines.push_back(*imageDetails->getLineList(leftVanLines));
	vanLines.push_back(*imageDetails->getLineList(rightVanLines));
	vanLines.push_back(*imageDetails->getLineList(vertLines));
	
	vector<vector<Vec4i>*> *quadrilaterals = new vector<vector<Vec4i>*>;

	int totalCount = 0;

	for(int i = 0; i < vanLines.size()-1; i++) {
		for(int j = i+1; j < vanLines.size(); j++) {
			for(int ref = 0; ref < vanLines[i].size(); ref++) {
				for(int comp = 0; comp < vanLines[j].size() && ref < vanLines[i].size(); comp++) {
					totalCount++;
					if(closeEnough(vanLines[i].at(ref), vanLines[j].at(comp)) 
						&& abs(getLineSlope(vanLines[i].at(ref)) - getLineSlope(vanLines[j].at(comp)) > 0.2)) {
						cout << "RefLine: " << vanLines[i].at(ref) << " :: CompLine: " << vanLines[j].at(comp) << endl;
						cout << "-----------------------------------------" << endl;

						String refString, compString;

						i == 0 ? refString = leftVanLines : refString = rightVanLines;
						j == 1 ? compString = rightVanLines : compString = vertLines;

						if(refString == leftVanLines && compString == rightVanLines) {
							vertQuads.push_back(completeQuad(vanLines[i].at(ref), refString, vanLines[j].at(comp), compString));
						} else if(refString == leftVanLines && compString == vertLines) {
							rightQuads.push_back(completeQuad(vanLines[i].at(ref), refString, vanLines[j].at(comp), compString));
						}  else {
							leftQuads.push_back(completeQuad(vanLines[i].at(ref), refString, vanLines[j].at(comp), compString));
						}

						ref++;
					}
				}
			}
		}
	}

	cout << "LinesCompared: " << totalCount << endl;
}

// closeEnough - determines the distance between two lines from different orientations. If
//               the distance is within threshold, true is returned.
// preconditions: none.
// postconditions: true is returned if the lines are close enough. false is returned if the
//                 lines are equal or too far apart.
//
bool QuadFinder::closeEnough(Vec4i ref, Vec4i comp) {
	if(ref == comp || comp == Vec4i(-1, -1, -1, -1) || ref == Vec4i(-1, -1, -1, -1)) return false;

	double refCompP1P1 = lineDist(Vec4i(ref[0], ref[1], comp[0], comp[1]));
	double refCompP1P2 = lineDist(Vec4i(ref[0], ref[1], comp[2], comp[3]));
	double refCompP2P1 = lineDist(Vec4i(ref[2], ref[3], comp[0], comp[1]));
	double refCompP2P2 = lineDist(Vec4i(ref[2], ref[3], comp[2], comp[3]));

	double threshold = 30;

	if(refCompP1P1 < threshold || refCompP1P2 < threshold || refCompP2P1 < threshold || refCompP2P2 < threshold) {
		return true;
	}

	return false;
}

// QuadFinder::displayQuad
// 
// Preconditions: none.
// Postconditions: the vector of points representing a quadrilateral is displayed.
//
void QuadFinder::displayQuad(string windowName, vector<Point> quadPnts, Mat displayQuadMat) {
	for(int i = 0; i < quadPnts.size(); i++) {
		line(displayQuadMat, quadPnts[i], quadPnts[(i+1)%quadPnts.size()], Scalar(0,255,0), 1);
	}

	imshow(windowName, displayQuadMat);
}

// QuadFinder::displayQuads
// 
// Preconditions: none.
// Postconditions: all quadrilaterals in the vector are displayed.
//
void QuadFinder::displayQuads(string windowName, vector<vector<Point>> quads) {
	for(int i = 0; i < quads.size(); i++) {
		displayQuad(windowName, quads[i], displayQuadsMat);
	}

	imshow(windowName, displayQuadsMat);
}

// QuadFinder::completeQuad
//
// Preconditions: "vanPts" point list exists in imageDetails. Two lines, ref and comp, that are
//                close in proximity have been identified.
//
// Postconditions: a completed quadrilateral has been formed and pushed to the respective vector.
//
vector<Point> QuadFinder::completeQuad(Vec4i ref, String refVanPt, Vec4i comp, String compVanPt) {
	vector<Point> *vanPts = imageDetails->getPointList("vanPts");
	imageDetails->insertMat("quad", (*imageDetails->getMat("original")).clone());

	Vec4i leftLine, rightLine, newLine1, newLine2;
	Point leftFur, rightFur, vanPt;

	vector<Point> furPnts = furthestPnt(ref, comp);
	vector<Point> closePnts = findClosestPnt(ref, comp);
	Point inter;

	cout << "Ref: " << ref << " :: Comp: " << comp << endl;
	cout << "FurRef:   " << furPnts[0] << " :: FurComp: " << furPnts[1] << endl;

	inter = findIntercepts(furPnts[0], getLineSlope(comp), furPnts[1], getLineSlope(ref));
	Point rcInter = findIntercepts(closePnts[0], getLineSlope(ref), closePnts[1], getLineSlope(comp));

	vector<Point> quads;
	quads.push_back(rcInter);
	quads.push_back(furPnts[0]);
	quads.push_back(inter);
	quads.push_back(furPnts[1]);

	cout << "QuadPnts: " << rcInter << furPnts[0] << inter << furPnts[1] << endl;

	line(displayQuadMat, Point(ref[0], ref[1]), Point(ref[2], ref[3]), Scalar(255,0,0), 1);
	line(displayQuadMat, Point(comp[0], comp[1]), Point(comp[2], comp[3]), Scalar(255,0,0), 1);
    circle(displayQuadMat, rcInter, 6, Scalar(0,100,0), 1);
	imshow("Display Quad", displayQuadMat);

	displayQuad("Display Quad", quads, displayQuadMat);

	return quads;
}

Vec4i QuadFinder::createNewLine(Point p1, Point p2) {
	Vec4i retLine;

	if(p1.x < p2.x)
		retLine = Vec4i(p1.x, p1.y, p2.x, p2.y);
	else
		retLine = Vec4i(p2.x, p2.y, p1.x, p1.y);

	return retLine;
}

// furthestPnt - finds and returns the furthest end points between two lines.
//
// Preconditions: none.
// Postconditions: finds the furthest end points between two lines
//
vector<Point> QuadFinder::furthestPnt(Vec4i ref, Vec4i comp) {
	vector<Point> closest = findClosestPnt(ref, comp);
	Point refPoint, compPoint;

	closest[0] == Point(ref[0], ref[1]) ? refPoint = Point(ref[2], ref[3]) : refPoint = Point(ref[0], ref[1]);
	closest[1] == Point(comp[0], comp[1]) ? compPoint = Point(comp[2], comp[3]) : compPoint = Point(comp[0], comp[1]);

	vector<Point> furs;
	furs.push_back(refPoint);
	furs.push_back(compPoint);

	return furs;
}

// findClosestPnt - finds and returns the closest end points between two lines.
//
// Preconditions: none.
// Postconditions: finds the closest end points between two lines
//
vector<Point> QuadFinder::findClosestPnt(Vec4i ref, Vec4i comp) {
	double refS = getLineSlope(ref);
	double refB = getLineIntercept(ref, refS);

	double compS = getLineSlope(comp);
	double compB = getLineIntercept(comp, compS);

	vector<Point> l1, l2;
	
	l1.push_back(Point(ref[0], ref[1]));
	l1.push_back(Point(ref[2], ref[3]));

	l2.push_back(Point(comp[0], comp[1]));
	l2.push_back(Point(comp[2], comp[3]));

	double minDist = 100;

	vector<Point> minPnts;
	minPnts.push_back(Point(0,0));
	minPnts.push_back(Point(0,0));

	for(int i = 0; i < l1.size(); i++) {
		for(int j = 0; j < l2.size(); j++) {
			double dist = lineDist(Vec4i(l1.at(i).x, l1.at(i).y, l2.at(j).x, l2.at(j).y));

			if(dist < minDist) {
				minDist = dist;

				minPnts[0] = l1.at(i);
				minPnts[1] = l2.at(j);
			}
		}
	}

	cout << "CloseRef: " << minPnts[0] << " :: CloseComp: " << minPnts[1] << endl;

	return minPnts;
}

double QuadFinder::getLineSlope(Vec4i ref) {
	if(double(ref[0] - ref[2]) == 0) {
		return 0;
	}

	double refSlope = (double)(ref[1] - ref[3]) / (double)(ref[0] - ref[2]);
	return refSlope;
} 

double QuadFinder::getLineIntercept(Vec4i ref, double refSlope) {
	if(refSlope == 0) {
		return -1;
	}

	double refB = (double)ref[1] - (double)((double)refSlope * (double)ref[0]);
	return refB;
}

Point QuadFinder::findLineIntercepts(Vec4i line1, Vec4i line2) {
	double slope1 = getLineSlope(line1);
	double slope2 = getLineSlope(line2);

	double b1 = getLineIntercept(line1, slope1);
	double b2 = getLineIntercept(line2, slope2);

	double x = (b2 - b1) / (slope1 - slope2);
	double y = slope1 * x + b1;

	return Point(x, y);
}

double QuadFinder::lineDist(Vec4i ref) {
	double distance = sqrt(pow(abs(ref[0] - ref[2]),2.0) + pow(abs(ref[1] - ref[3]), 2.0));
	return distance;
}

// findIntercepts
// preconditions: none.
// postconditions: the intersection of two lines, a Point, is returned.
Point QuadFinder::findIntercepts(Point pnt1, double slope1, Point pnt2, double slope2) {
	Point retPnt;
	double b1 = pnt1.y - (slope1 * pnt1.x);
	double b2 = pnt2.y - (slope2 * pnt2.x);

	cout << endl << "y = " << slope1 << "x + " << b1 << endl;
	cout << endl << "y = " << slope2 << "x + " << b2 << endl;

	if(slope1 == 0) {
		retPnt.x = pnt1.x;
		retPnt.y = slope2 * retPnt.x + b2;

		return retPnt;
	}

	else if(slope2 == 0) {
		retPnt.x = pnt2.x;
		retPnt.y = slope1 * retPnt.x + b1;

		return retPnt;
	}

	double x = (b2 - b1) / (slope1 - slope2);
	double y = slope1 * x + b1;

	return Point(x, y);
}
