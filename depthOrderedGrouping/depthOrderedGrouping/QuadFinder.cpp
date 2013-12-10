#include "QuadFinder.h"

QuadFinder::QuadFinder(ImageDetails* img) {
	imageDetails = img;
	quadCand = new vector<vector<Point>*>;
}

vector<vector<Point>*>* QuadFinder::getQuads() {
	findCloseLines();

	return quadCand;
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

						quadCand->push_back(&completeQuad(vanLines[i].at(ref), refString, vanLines[j].at(comp), compString));

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

	double refCompP1P1 = lineDist(Vec4i(ref[0], ref[1], comp[0], comp[1]));
	double refCompP1P2 = lineDist(Vec4i(ref[0], ref[1], comp[2], comp[3]));
	double refCompP2P1 = lineDist(Vec4i(ref[2], ref[3], comp[0], comp[1]));
	double refCompP2P2 = lineDist(Vec4i(ref[2], ref[3], comp[2], comp[3]));

	if(refCompP1P1 < 10 || refCompP1P2 < 10 || refCompP2P1 < 10 || refCompP2P2 < 10) {
		return true;
	}

	/*if(abs(ref[0] - comp[0]) < 10 && abs(ref[1] - comp[1]) < 10) {
		return true;
	}*/

	return false;
}

vector<Point> QuadFinder::completeQuad(Vec4i ref, String refVanPt, Vec4i comp, String compVanPt) {
	vector<Point> *vanPts = imageDetails->getPointList("vanPts");
	imageDetails->insertMat("quad", (*imageDetails->getMat("original")).clone());

	cout << vanPts->at(0) << " " << vanPts->at(1) << " ";

	Vec4i leftLine, rightLine, newLine1, newLine2;
	Point leftFur, rightFur, vanPt;

	vector<Point> furPnts = furthestPnt(ref, comp);
	vector<Point> closePnts = findClosestPnt(ref, comp);
	Point inter;

	cout << "FURPNTS: " << furPnts[0] << furPnts[1] << endl;
	cout << "CLOSEPNTS: " << closePnts[0] << closePnts[1] << endl;
	cout << "LINES: " << ref << comp << endl;

	if(compVanPt == "vertLines") {
		rightFur = furPnts[0];
		leftFur = furPnts[1];

		if(rightFur.y > leftFur.y) {
			rightLine = Vec4i(rightFur.x, rightFur.y, rightFur.x, rightFur.y - 2*(lineDist(comp)));
		} else {
			rightLine = Vec4i(rightFur.x, rightFur.y, rightFur.x, rightFur.y + 2*(lineDist(comp)));
		}

		if(refVanPt == "leftVanLines") {
			//leftFur = furthestPnt(comp, ref);
			leftLine = createNewLine(leftFur, vanPts->at(0));
		}
		else {
			//leftFur = furthestPnt(comp, ref);
			leftLine = createNewLine(leftFur, vanPts->at(1));
		}

		double slope = getLineSlope(leftLine);
		double b = getLineIntercept(leftLine, slope);
		double y = slope * rightFur.x + b;

		inter = Point(rightFur.x, y);

		newLine1 = Vec4i(inter.x, inter.y, leftFur.x, leftFur.y);
		newLine2 = Vec4i(inter.x, inter.y, rightFur.x, rightFur.y);

	/*line(*imageDetails->getMat("quad"), Point(leftLine[0], leftLine[1]), Point(leftLine[2], leftLine[3]), Scalar(255,0,0));
	line(*imageDetails->getMat("quad"), Point(rightLine[0], rightLine[1]), Point(rightLine[2], rightLine[3]), Scalar(0,255,0));
	line(*imageDetails->getMat("quad"), Point(newLine1[0], newLine1[1]), Point(newLine1[2], newLine1[3]), Scalar(0,255,0));
	line(*imageDetails->getMat("quad"), Point(newLine2[0], newLine2[1]), Point(newLine2[2], newLine2[3]), Scalar(255,0,0));
	line(*imageDetails->getMat("quad"), Point(ref[0], ref[1]), Point(ref[2], ref[3]), Scalar(0,0,255));
	line(*imageDetails->getMat("quad"), Point(comp[0], comp[1]), Point(comp[2], comp[3]), Scalar(0,0,255));

	imshow("Quad", *imageDetails->getMat("quad"));
	cvWaitKey();*/
	} else {
		leftFur = furPnts[1];
		leftLine = createNewLine(leftFur, vanPts->at(0));

		rightFur = furPnts[0];
		rightLine = createNewLine(rightFur, vanPts->at(1));

		inter = findLineIntercepts(leftLine, rightLine);

		newLine1 = Vec4i(inter.x, inter.y, leftFur.x, leftFur.y);
		newLine2 = Vec4i(inter.x, inter.y, rightFur.x, rightFur.y);
	}

	line(*imageDetails->getMat("quad"), Point(leftLine[0], leftLine[1]), Point(leftLine[2], leftLine[3]), Scalar(255,0,0));
	line(*imageDetails->getMat("quad"), Point(rightLine[0], rightLine[1]), Point(rightLine[2], rightLine[3]), Scalar(0,255,0));
	line(*imageDetails->getMat("quad"), Point(newLine1[0], newLine1[1]), Point(newLine1[2], newLine1[3]), Scalar(0,255,0));
	line(*imageDetails->getMat("quad"), Point(newLine2[0], newLine2[1]), Point(newLine2[2], newLine2[3]), Scalar(255,0,0));
	line(*imageDetails->getMat("quad"), Point(ref[0], ref[1]), Point(ref[2], ref[3]), Scalar(0,0,255));
	line(*imageDetails->getMat("quad"), Point(comp[0], comp[1]), Point(comp[2], comp[3]), Scalar(0,0,255));

	imshow("Quad", *imageDetails->getMat("quad"));
	//cvWaitKey();

	Point rcInter = findLineIntercepts(ref, comp);

	cout << "rcInter: " << rcInter << endl;

	vector<Point> quads;
	quads.push_back(rcInter);
	quads.push_back(furPnts[0]);
	quads.push_back(furPnts[1]);
	quads.push_back(inter);

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

vector<Point> QuadFinder::findClosestPnt(Vec4i ref, Vec4i comp) {
	double refS = getLineSlope(ref);
	double refB = getLineIntercept(ref, refS);

	double compS = getLineSlope(comp);
	double compB = getLineIntercept(comp, compS);

	int Rx, Ry, Cy, Cx, rMax, cMaxX, cMaxY;

	if(ref[0] < ref[2]) {
		rMax = ref[2];
		Rx = ref[0];
		Ry = ref[1];
	} else {
		rMax = ref[0];
		Rx = ref[2];
		Ry = ref[3];
	}

	if(comp[0] < comp[2]) {
		cMaxX = comp[2];
		cMaxY = comp[3];
		Cx = comp[0];
		Cy = ref[1];
	} else {
		cMaxY = comp[0];
		cMaxX = comp[1];
		Cx = comp[2];
		Cy = comp[3];
	}

	double minDist = 100;
	vector<Point> minPnts;
	minPnts.push_back(Point(0,0));
	minPnts.push_back(Point(0,0));

	cout << endl << "y = " << refS << "x + " << refB<< endl;
	cout << endl << "y = " << compS << "x + " << compB << endl;

	cout << endl << "RMAX: " << rMax << " - Rx: " << Rx << " , ";
	cout << "CMAX: " << cMaxX << " - Cx: " << Cx << endl;
	Cy -= 1;

	for(Rx; Rx < rMax; Rx++) {
		Ry = (refS * Rx) + refB;

		//cout << Rx << " " << Ry << " ";
		if(compS == 0.0000 || compS == 0.0000) {
			comp[1] < comp[3] ? Cy = comp[1] : Cy = comp[3];

			for(int cy = Cy; cy < cMaxY; cy++) {
				double dist = lineDist(Vec4i(Rx, Ry, Cx, cy));

				if( dist < minDist ) {
					//cout << Rx << " " << Ry << " ";
					minDist = dist;
					minPnts[0] = Point(Rx, Ry);
					minPnts[1] = Point(Cx, cy);
				}
			}
		} else {
			for(int cx = Cx; cx < cMaxX; cx++) {
				Cy = (compS * cx) + compB;

				double dist = lineDist(Vec4i(Rx, Ry, cx, Cy));

				if( dist < minDist ) {
					//cout << Rx << " " << Ry << " ";
					minDist = dist;
					minPnts[0] = Point(Rx, Ry);
					minPnts[1] = Point(cx, Cy);
				}
			}
		}
	}
	cout << "MINPTS: " << minPnts[0] << minPnts[1] << endl;

	if(minPnts[0] != Point(ref[0], ref[1]) || minPnts[0] != Point(ref[2], ref[3])) {
		int refD1 = lineDist(Vec4i(ref[0], ref[1], minPnts[0].x, minPnts[0].y));
		int refD2 = lineDist(Vec4i(ref[2], ref[3], minPnts[0].x, minPnts[0].y));

		refD1 < refD2 ? minPnts[0] = Point(ref[0], ref[1]) : minPnts[0] = Point(ref[2], ref[3]);
	}

	if(minPnts[1] != Point(comp[0], comp[1]) || minPnts[1] != Point(comp[2], comp[3])) {
		int compD1 = lineDist(Vec4i(comp[0], comp[1], minPnts[1].x, minPnts[1].y));
		int compD2 = lineDist(Vec4i(comp[2], comp[3], minPnts[1].x, minPnts[1].y));

		compD1 < compD2 ? minPnts[1] = Point(comp[0], comp[1]) : minPnts[1] = Point(comp[2], comp[3]);
	}

	cout << "MINPTS: " << minPnts[0] << minPnts[1] << endl;

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
