#include "QuadFinder.h"

QuadFinder::QuadFinder(ImageDetails* img) {
	imageDetails = img;


	findCloseLines();
}

void QuadFinder::findCloseLines() {
	vector<vector<Vec4i>> vanLines;

	String leftVanLines = "leftVanLines";
	String rightVanLines = "rightVanLines";
	String vertLines = "vertLines";

	vanLines.push_back(*imageDetails->getLineList(leftVanLines));
	vanLines.push_back(*imageDetails->getLineList(rightVanLines));
	vanLines.push_back(*imageDetails->getLineList(vertLines));
	
	vector<vector<Vec4i>*> *quadrilaterals = new vector<vector<Vec4i>*>;

	for(int i = 0; i < vanLines.size()-1; i++) {
		for(int j = i+1; j < vanLines.size(); j++) {
			for(int ref = 0; ref < vanLines[i].size(); ref++) {
				for(int comp = 0; comp < vanLines[j].size() && ref < vanLines[i].size(); comp++) {
					if(closeEnough(vanLines[i].at(ref), vanLines[j].at(comp))) {

						String refString, compString;

						i == 0 ? refString = leftVanLines : refString = rightVanLines;
						j == 1 ? compString = rightVanLines : compString = vertLines;

						completeQuad(vanLines[i].at(ref), refString, vanLines[j].at(comp), compString);

						vanLines[i].at(ref) = Vec4i(-1, -1, -1, -1);
						vanLines[j].at(comp) = Vec4i(-1, -1, -1, -1);
					}
				}
			}
		}
	}

	cvWaitKey();
}

bool QuadFinder::closeEnough(Vec4i ref, Vec4i comp) {
	if(ref == comp) return false;

	if(abs(ref[0] - comp[0]) < 10 && abs(ref[1] - comp[1]) < 10) {
		return true;
	}

	return false;
}

void QuadFinder::completeQuad(Vec4i ref, String refVanPt, Vec4i comp, String compVanPt) {
	vector<Point> *vanPts = imageDetails->getPointList("vanPts");
	imageDetails->insertMat("quad", (*imageDetails->getMat("original")).clone());

	cout << vanPts->at(0) << " " << vanPts->at(1) << " ";

	Vec4i leftLine, rightLine, newLine1, newLine2;
	Point leftFur, rightFur, vanPt;

	if(compVanPt == "vertLines") {
		rightFur = furthestPnt(ref, comp);
		leftFur = furthestPnt(comp, ref);

		if(rightFur.y > leftFur.y) {
			rightLine = Vec4i(rightFur.x, rightFur.y, rightFur.x, rightFur.y - 2*(lineDist(comp)));
		} else {
			rightLine = Vec4i(rightFur.x, rightFur.y, rightFur.x, rightFur.y + 2*(lineDist(comp)));
		}

		if(refVanPt == "leftVanLines") {
			leftFur = furthestPnt(comp, ref);
			leftLine = createNewLine(leftFur, vanPts->at(0));
		}
		else {
			leftFur = furthestPnt(comp, ref);
			leftLine = createNewLine(leftFur, vanPts->at(1));
		}

		double slope = getLineSlope(leftLine);
		double b = getLineIntercept(leftLine, slope);
		double y = slope * rightFur.x + b;

		Point inter = Point(rightFur.x, y);

		newLine1 = Vec4i(inter.x, inter.y, leftFur.x, leftFur.y);
		newLine2 = Vec4i(inter.x, inter.y, rightFur.x, rightFur.y);
	} else {
		leftFur = furthestPnt(comp, ref);
		leftLine = createNewLine(leftFur, vanPts->at(0));

		rightFur = furthestPnt(ref, comp);
		rightLine = createNewLine(rightFur, vanPts->at(1));

		Point inter = findLineIntercepts(leftLine, rightLine);

		newLine1 = Vec4i(inter.x, inter.y, leftFur.x, leftFur.y);
		newLine2 = Vec4i(inter.x, inter.y, rightFur.x, rightFur.y);
	}

	//line(*imageDetails->getMat("quad"), Point(leftLine[0], leftLine[1]), Point(leftLine[2], leftLine[3]), Scalar(255,0,0));
	//line(*imageDetails->getMat("quad"), Point(rightLine[0], rightLine[1]), Point(rightLine[2], rightLine[3]), Scalar(0,255,0));
	line(*imageDetails->getMat("quad"), Point(newLine1[0], newLine1[1]), Point(newLine1[2], newLine1[3]), Scalar(0,255,0));
	line(*imageDetails->getMat("quad"), Point(newLine2[0], newLine2[1]), Point(newLine2[2], newLine2[3]), Scalar(255,0,0));
	line(*imageDetails->getMat("quad"), Point(ref[0], ref[1]), Point(ref[2], ref[3]), Scalar(0,0,255));
	line(*imageDetails->getMat("quad"), Point(comp[0], comp[1]), Point(comp[2], comp[3]), Scalar(0,0,255));

	imshow("Quad", *imageDetails->getMat("quad"));
	cvWaitKey();
}

Vec4i QuadFinder::createNewLine(Point p1, Point p2) {
	Vec4i retLine;

	if(p1.x < p2.x)
		retLine = Vec4i(p1.x, p1.y, p2.x, p2.y);
	else
		retLine = Vec4i(p2.x, p2.y, p1.x, p1.y);

	return retLine;
}

Vec4i QuadFinder::extendLine(Vec4i ref, Vec4i comp) {
	int compDistance = sqrt(pow(abs(comp[0] - comp[2]),2.0) + pow(abs(comp[1] - comp[3]), 2.0));

	double refSlope = (double)(ref[1] - ref[3]) / (double)(ref[0] - ref[2]);
	double refB = (double)ref[1] - (double)(refSlope * ref[0]);

	int x = ref[2] + compDistance + 5;
	int y = refSlope * x + refB;

	ref[2] = x;
	ref[3] = y;

	return ref;
}

Point QuadFinder::furthestPnt(Vec4i ref, Vec4i comp) {
	int distance1 = abs(ref[0] - comp[0]) + abs(ref[1] - comp[1]);
	int distance2 = abs(ref[2] - comp[0]) + abs(ref[3] - comp[1]);

	if(distance1 > distance2) {
		return Point(ref[0], ref[1]);
	} else {
		return Point(ref[2], ref[3]);
	}
}

double QuadFinder::getLineSlope(Vec4i ref) {
	double refSlope = (double)(ref[1] - ref[3]) / (double)(ref[0] - ref[2]);
	return refSlope;
} 

double QuadFinder::getLineIntercept(Vec4i ref, double refSlope) {
	double refB = (double)ref[1] - (double)((double)refSlope * (double)ref[0]);
	return refB;
}

Point QuadFinder::findLineIntercepts(Vec4i line1, Vec4i line2) {
	double slope1 = getLineSlope(line1);
	double slope2 = getLineSlope(line2);

	double b1 = getLineIntercept(line1, slope1);
	double b2 = getLineIntercept(line2, slope2);

	cout << endl << "y = " << slope1 << "x + " << b1 << endl;
	cout << endl << "y = " << slope2 << "x + " << b2 << endl;

	double x = (b2 - b1) / (slope1 - slope2);
	double y = slope1 * x + b1;

	return Point(x, y);
}

double QuadFinder::lineDist(Vec4i ref) {
	double distance = sqrt(pow(abs(ref[0] - ref[2]),2.0) + pow(abs(ref[1] - ref[3]), 2.0));
	return distance;
}