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

	cout << vanLines.size() << " " << vanLines.at(0).size() << " " << vanLines[1].size() << " " << vanLines[2].size() << endl;

	for(int i = 0; i < vanLines.size()-1; i++) {
		for(int j = i+1; j < vanLines.size(); j++) {
			cout << "I: " << i << " J: " << j << endl;

			for(int ref = 0; ref < vanLines[i].size(); ref++) {
				for(int comp = 0; comp < vanLines[j].size() && ref < vanLines[i].size(); comp++) {
					if(closeEnough(vanLines[i].at(ref), vanLines[j].at(comp))) {

						String refString, compString;

						if(i == 0) {
							refString = leftVanLines;
						} else {
							refString = rightVanLines;
						}

						j == 1 ? compString = rightVanLines : compString = vertLines;

						vector<Vec4i> insertVec;
						insertVec.push_back(vanLines[i].at(ref));
						insertVec.push_back(vanLines[j].at(comp));
						quadrilaterals->push_back(&insertVec);

						cout << "REF: " << ref << " COMP: " << comp << endl;

						//cout << "ORIGINAL: " << vanLines[i].at(ref) << " ";

						completeQuad(vanLines[i].at(ref), refString, vanLines[j].at(comp), compString);

						vanLines[i].at(ref) = Vec4i(-1, -1, -1, -1);
						vanLines[j].at(comp) = Vec4i(-1, -1, -1, -1);

						//cout << "AFTER: " << vanLines[i].at(ref) << endl;

						//if(ref < vanLines[i].size()) ref++;
					}
				}
			}
		}
	}

	quadCand = quadrilaterals;

	cvWaitKey();
}

bool QuadFinder::closeEnough(Vec4i ref, Vec4i comp) {
	if(ref == comp) return false;

	if(abs(ref[0] - comp[0]) < 10 && abs(ref[1] - comp[1]) < 10) {
		cout << ref << " " << comp << "   ";	
		return true;
	}

	return false;
}

void QuadFinder::completeQuad(Vec4i ref, String refVanPt, Vec4i comp, String compVanPt) {
	vector<Point> *vanPts = imageDetails->getPointList("vanPts");
	imageDetails->insertMat("quad", (*imageDetails->getMat("original")).clone());

	cout << vanPts->at(0) << " " << vanPts->at(1) << " ";

	if(refVanPt == "leftVanLines") {
		Point furPnt2;
		Point furPnt = furthestPnt(comp, ref);
		Vec4i newLine = createNewLine(furPnt, vanPts->at(0), ref);
		Vec4i newLine2;

		if(compVanPt == "rightVanLines") {
			furPnt2 = furthestPnt(ref, comp);
			newLine2 = createNewLine(furPnt2, vanPts->at(1), comp);

		}


		double slope1 = getLineSlope(newLine);
		double slope2 = getLineSlope(newLine2);

		double b1 = getLineIntercept(newLine, slope1);
		double b2 = getLineIntercept(newLine2, slope2);

		cout << "y = " << slope1 << "x + " << b1 << endl;
		cout << "y = " << slope2 << "x + " << b2 << endl;

		Point inter = findLineIntercepts(slope1, getLineIntercept(newLine, slope1), slope2, getLineIntercept(newLine2, slope2));
		cout << "INTER: " << inter << " " << endl;
		
		newLine = Vec4i(inter.x, inter.y, furPnt.x, furPnt.y);
		newLine2 = Vec4i(inter.x, inter.y, furPnt2.x, furPnt2.y);

		circle(*imageDetails->getMat("quad"), inter, 6, Scalar(0,100,0), 1);
		line(*imageDetails->getMat("quad"), Point(newLine[0], newLine[1]), Point(newLine[2], newLine[3]), Scalar(0,255,0));
		line(*imageDetails->getMat("quad"), Point(newLine2[0], newLine2[1]), Point(newLine2[2], newLine2[3]), Scalar(255,0,0));
		line(*imageDetails->getMat("quad"), Point(ref[0], ref[1]), Point(ref[2], ref[3]), Scalar(0,0,255));
		line(*imageDetails->getMat("quad"), Point(comp[0], comp[1]), Point(comp[2], comp[3]), Scalar(0,0,255));

		imshow("Quad", *imageDetails->getMat("quad"));
		cvWaitKey();

		cout << "furPnt: " << newLine << endl;
	} else if (refVanPt == "rightVanLines") {
		cout << "Right: " << ref << endl;
	}
}

Vec4i QuadFinder::createNewLine(Point p1, Point p2, Vec4i ref) {
	Vec4i retLine;

	if(p1.x < p2.x)
		retLine = Vec4i(p1.x, p1.y, p2.x, p2.y);
	else
		retLine = Vec4i(p2.x, p2.y, p1.x, p1.y);

	if(retLine[2] < ref[2] || retLine[3] < ref[3])
		//retLine = extendLine(retLine, ref);

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
	double refB = (double)ref[1] - (double)(refSlope * ref[0]);

	return refB;
}

Point QuadFinder::findLineIntercepts(double slope1, double b1, double slope2, double b2) {
	double x = (b1 - b2) / (slope1 - slope2);
	double y = slope1 * x + b1;

	return Point(-x, -y);
}