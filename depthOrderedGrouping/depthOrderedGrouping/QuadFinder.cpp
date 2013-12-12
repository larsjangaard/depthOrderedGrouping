#include "QuadFinder.h"

QuadFinder::QuadFinder(ImageDetails* img) {
	imageDetails = img;
	quadCand = new vector<vector<Point>*>;
	displayVec = new vector<Vec4i>;

	//leftQuads = new vector<vector<Point>>;
	//rightQuads = new vector<vector<Point>>;
	//vertQuads = new vector<vector<Point>>;
}

vector<vector<vector<Point>>>* QuadFinder::getQuads() {
	findCloseLines();

	vector<vector<vector<Point>>>* quads = new vector<vector<vector<Point>>>;
	quads->push_back(leftQuads);
	quads->push_back(rightQuads);
	quads->push_back(vertQuads);

	return quads;
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

	int totalCount = 0;

	for(int i = 0; i < vanLines.size()-1; i++) {
		for(int j = i+1; j < vanLines.size(); j++) {
			cout << "I: " << i << " - J: " << j << endl;
			for(int ref = 0; ref < vanLines[i].size(); ref++) {
				for(int comp = 0; comp < vanLines[j].size() && ref < vanLines[i].size(); comp++) {
					//cout << "(" << ref << "," << comp << ")";
					totalCount++;
					if(closeEnough(vanLines[i].at(ref), vanLines[j].at(comp))) {
						cout << "RefLine: " << vanLines[i].at(ref) << " :: CompLine: " << vanLines[j].at(comp) << endl;
						cout << "-----------------------------------------" << endl;

						String refString, compString;

						i == 0 ? refString = leftVanLines : refString = rightVanLines;
						j == 1 ? compString = rightVanLines : compString = vertLines;



						//vector<vector<Point>> leftQuads;
						//vector<vector<Point>> rightQuads;
						//vector<vector<Point>> vertQuads;

						if(refString == leftVanLines && compString == rightVanLines) {
							vector<Point> vec = completeQuad(vanLines[i].at(ref), refString, vanLines[j].at(comp), compString);
							vertQuads.push_back(vec);
						} else if(refString == leftVanLines && compString == vertLines) {
							rightQuads.push_back(completeQuad(vanLines[i].at(ref), refString, vanLines[j].at(comp), compString));
						}  else {
							leftQuads.push_back(completeQuad(vanLines[i].at(ref), refString, vanLines[j].at(comp), compString));
						}
						
						//quadCand->push_back(&completeQuad(vanLines[i].at(ref), refString, vanLines[j].at(comp), compString));

						vanLines[i].at(ref) = Vec4i(-1, -1, -1, -1);
						vanLines[j].at(comp) = Vec4i(-1, -1, -1, -1);

						ref++;
					}
				}
			}
		}
	}

	//cout << "LinesCompared: " << totalCount << endl;
	for(int i = 0; i < displayVec->size(); i++) {
		for(int j = 0; j < displayVec->size(); j++) {
			Vec4i newLine1 = displayVec->at(i);

			line(*imageDetails->getMat("quad"), Point(newLine1[0], newLine1[1]), Point(newLine1[2], newLine1[3]), Scalar(0,255,0), 2);
			//line(*imageDetails->getMat("quad"), Point(newLine2[0], newLine2[1]), Point(newLine2[2], newLine2[3]), Scalar(0,255,0), 2);
			//line(*imageDetails->getMat("quad"), Point(ref[0], ref[1]), Point(ref[2], ref[3]), Scalar(0,255,0), 2);
			//line(*imageDetails->getMat("quad"), Point(comp[0], comp[1]), Point(comp[2], comp[3]), Scalar(0,255,0), 2);
		}
	}

	imshow("All Quads", *imageDetails->getMat("quad"));

	cvWaitKey();
}

bool QuadFinder::closeEnough(Vec4i ref, Vec4i comp) {
	if(ref == comp || comp == Vec4i(-1, -1, -1, -1) || ref == Vec4i(-1, -1, -1, -1)) return false;

	double refCompP1P1 = lineDist(Vec4i(ref[0], ref[1], comp[0], comp[1]));
	double refCompP1P2 = lineDist(Vec4i(ref[0], ref[1], comp[2], comp[3]));
	double refCompP2P1 = lineDist(Vec4i(ref[2], ref[3], comp[0], comp[1]));
	double refCompP2P2 = lineDist(Vec4i(ref[2], ref[3], comp[2], comp[3]));

	if(refCompP1P1 < 10 || refCompP1P2 < 10 || refCompP2P1 < 10 || refCompP2P2 < 10) {
		return true;
	}

	return false;
}

vector<Point> QuadFinder::completeQuad(Vec4i ref, String refVanPt, Vec4i comp, String compVanPt) {
	vector<Point> *vanPts = imageDetails->getPointList("vanPts");
	imageDetails->insertMat("quad", (*imageDetails->getMat("original")).clone());

	Vec4i leftLine, rightLine, newLine1, newLine2;
	Point leftFur, rightFur, vanPt;

	vector<Point> furPnts = furthestPnt(ref, comp);
	//vector<Point> closePnts = findClosestPnt(ref, comp);
	Point inter;

	cout << "Ref: " << ref << " :: Comp: " << comp << endl;
	cout << "FurRef:   " << furPnts[0] << " :: FurComp: " << furPnts[1] << endl;

	if(compVanPt == "vertLines") {
		rightFur = furPnts[0];
		leftFur = furPnts[1];

		if(rightFur.y > leftFur.y) {
			rightLine = Vec4i(rightFur.x, rightFur.y, rightFur.x, rightFur.y - 2*(lineDist(comp)));
		} else {
			rightLine = Vec4i(rightFur.x, rightFur.y, rightFur.x, rightFur.y + 2*(lineDist(comp)));
		}

		if(refVanPt == "leftVanLines") {
			leftLine = createNewLine(leftFur, vanPts->at(0));
		}
		else {
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

	//line(*imageDetails->getMat("quad"), Point(leftLine[0], leftLine[1]), Point(leftLine[2], leftLine[3]), Scalar(255,0,0));
	//line(*imageDetails->getMat("quad"), Point(rightLine[0], rightLine[1]), Point(rightLine[2], rightLine[3]), Scalar(0,255,0));
	//line(*imageDetails->getMat("quad"), Point(newLine1[0], newLine1[1]), Point(newLine1[2], newLine1[3]), Scalar(255,0,0), 1);
	//line(*imageDetails->getMat("quad"), Point(newLine2[0], newLine2[1]), Point(newLine2[2], newLine2[3]), Scalar(0,255,0), 1);
	//line(*imageDetails->getMat("quad"), Point(ref[0], ref[1]), Point(ref[2], ref[3]), Scalar(0,0,255), 2);
	//line(*imageDetails->getMat("quad"), Point(comp[0], comp[1]), Point(comp[2], comp[3]), Scalar(0,0,255), 2);

	imshow("Quad", *imageDetails->getMat("quad"));

	Point rcInter = findLineIntercepts(ref, comp);

	cout << "rcInter: " << rcInter << endl << endl;

	//cvWaitKey();

	displayVec->push_back(newLine1);
	displayVec->push_back(newLine2);
	displayVec->push_back(ref);
	displayVec->push_back(comp);

	vector<Point> quads;
	quads.push_back(rcInter);
	quads.push_back(furPnts[0]);
	quads.push_back(inter);
	quads.push_back(furPnts[1]);

	/*cout << "P0: " << rcInter << " P1: " << furPnts[0] << endl;
	line(*imageDetails->getMat("quad"), rcInter, furPnts[0], Scalar(0,255,0), 2);
	imshow("Quad", *imageDetails->getMat("quad"));
	cvWaitKey();

	cout << " P1: " << furPnts[0] << " P2: " << inter << endl;
	line(*imageDetails->getMat("quad"), furPnts[0], inter, Scalar(0,255,0), 2);
	imshow("Quad", *imageDetails->getMat("quad"));
	cvWaitKey();

	cout << " P2: " << furPnts[1] << " P3: " << inter << endl;
	line(*imageDetails->getMat("quad"), furPnts[1], inter, Scalar(0,255,0), 2);
	imshow("Quad", *imageDetails->getMat("quad"));
	cvWaitKey();

	line(*imageDetails->getMat("quad"), rcInter, furPnts[1], Scalar(0,255,0), 2);
	cout << "P3: " << rcInter << " P0: " << furPnts[1] << endl;
	imshow("Quad", *imageDetails->getMat("quad"));
	cvWaitKey();*/

	imshow("Quad", *imageDetails->getMat("quad"));

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

	vector<Point> l1, l2;
	
	l1.push_back(Point(ref[0], ref[1]));
	l1.push_back(Point(ref[0], ref[1]));

	l2.push_back(Point(ref[0], ref[1]));
	l2.push_back(Point(ref[0], ref[1]));

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
				minPnts[1] = l1.at(j);
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

	//cout << endl << "y = " << slope1 << "x + " << b1 << endl;
	//cout << endl << "y = " << slope2 << "x + " << b2 << endl;

	double x = (b2 - b1) / (slope1 - slope2);
	double y = slope1 * x + b1;

	return Point(x, y);
}

double QuadFinder::lineDist(Vec4i ref) {
	double distance = sqrt(pow(abs(ref[0] - ref[2]),2.0) + pow(abs(ref[1] - ref[3]), 2.0));
	return distance;
}
