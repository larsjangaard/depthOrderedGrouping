#include "QuadGrouper.h"


QuadGrouper::QuadGrouper(ImageDetails *imageDetails, vector<vector<Point>*> *quads) {
	imgDets = imageDetails;
	ungroupedQuads = quads;
	groupQuads();
}


QuadGrouper::~QuadGrouper(void) {}

void QuadGrouper::groupQuads() {

	// temporary until available input
	vector<vector<Point>> *ungroupedLeftQuads = new vector<vector<Point>>,
						  *ungroupedRightQuads = new vector<vector<Point>>,
						  *ungroupedVertQuads = new vector<vector<Point>>;

	groupQuadVec(*ungroupedLeftQuads, groupedLeftQuads);
	groupQuadVec(*ungroupedRightQuads, groupedRightQuads);
	groupQuadVec(*ungroupedVertQuads, groupedVertQuads);

	//imgDets->insertQuadList("groupedLeftQuads", groupedLeftQuads);
	//imgDets->insertQuadList("groupepRightQuads", groupedRightQuads);
	//imgDets->insertQuadList("groupedVertQuads", groupedVertQuads);
}

void QuadGrouper::groupQuadVec(vector<vector<Point>> ungroupedQuads,
							   vector<vector<Point>> *&groupedQuads) {
	bool foundAnyCombo = true;
	while (foundAnyCombo) {
		foundAnyCombo = false;
		int size = ungroupedQuads.size();
		vector<bool> foundGroup(size, false);
		for (int i=0; i < size; i++) {
			for (int j=i+1; j < size; j++) {
				vector<Point> *groupedQuad = new vector<Point>;
				if (tryCombine(ungroupedQuads[i],
							   ungroupedQuads[j],
							   *&groupedQuad)) {
					groupedQuads->push_back(*groupedQuad);
					foundGroup[i] = foundGroup[j] = foundAnyCombo = true;
				}
			}
		}
		for (int i=0; i < size; i++)
			if (!foundGroup[i])
				groupedQuads->push_back(ungroupedQuads[i]);
		if (foundAnyCombo) {
			ungroupedQuads.clear();
			ungroupedQuads = *groupedQuads;
			groupedQuads->clear();
		}
	}
}

bool QuadGrouper::tryCombine(vector<Point> quadA, vector<Point> quadB, vector<Point> *&combined) {
	vector<Point> quadC;
	if (getQuadIntxn(quadA, quadB, quadC)) {
		vector<Point> rectA = rectify(quadA);
		vector<Point> rectB = rectify(quadB);
		float areaA = getArea(rectA);
		float areaB = getArea(rectB);
		float smallArea;
		vector<Point> smallQuad, smallRect;
		if (areaA < areaB) {
			smallArea = areaA;
			smallQuad = quadA;
			smallRect = rectA;
		} else {
			smallArea = areaB;
			smallQuad = quadB;
			smallRect = rectB;
		}
		float areaC = getIntxnArea(smallQuad, smallRect, quadC);
		if (areaC/smallArea > .25) {
			*combined = combineQuads(quadA, quadB);
			return true;
		}
	}
	return false;
}

bool QuadGrouper::getQuadIntxn(vector<Point> quadA, vector<Point> quadB, vector<Point> &quadC) {

	Polygons qA, qB, intxn;
	qA[0].push_back(IntPoint(quadA.at(0).x,quadA.at(0).y));
	qA[0].push_back(IntPoint(quadA.at(1).x,quadA.at(1).y));
	qA[0].push_back(IntPoint(quadA.at(2).x,quadA.at(2).y));
	qA[0].push_back(IntPoint(quadA.at(3).x,quadA.at(3).y));
	qB[1].push_back(IntPoint(quadB.at(0).x,quadB.at(0).y));
	qB[1].push_back(IntPoint(quadB.at(1).x,quadB.at(1).y));
	qB[1].push_back(IntPoint(quadB.at(2).x,quadB.at(2).y));
	qB[1].push_back(IntPoint(quadB.at(3).x,quadB.at(3).y));

	Clipper c;
	c.AddPolygons(qA, ptSubject);
	c.AddPolygons(qB, ptClip);
	c.Execute(ctIntersection, intxn, pftNonZero, pftNonZero);

	if (!intxn.empty()) {
		quadC.push_back(Point(intxn[0].at(0).X, intxn[0].at(0).Y));
		quadC.push_back(Point(intxn[0].at(1).X, intxn[0].at(1).Y));
		quadC.push_back(Point(intxn[0].at(2).X, intxn[0].at(2).Y));
		quadC.push_back(Point(intxn[0].at(3).X, intxn[0].at(3).Y));
		return true;
	}
	return false;
}

vector<Point> rectify(vector<Point> quad) {
	float longSideA = max(norm(quad.at(0) - quad.at(1)),
						norm(quad.at(2) - quad.at(3)));
	float longSideB = max(norm(quad.at(1) - quad.at(2)),
						norm(quad.at(3) - quad.at(0)));
	vector<Point> rect;
	rect.push_back(Point(0,0));
	rect.push_back(Point(longSideA,0));
	rect.push_back(Point(longSideA,longSideB));
	rect.push_back(Point(0,longSideB));
	return rect;
}

float QuadGrouper::getArea(vector<Point> rect) {
	float sideA = norm(rect.at(0) - rect.at(1));
	float sideB = norm(rect.at(1) - rect.at(2));
	return sideA*sideB;
}

float QuadGrouper::getIntxnArea(vector<Point> quad, vector<Point> rect, vector<Point> intxnQuad) {
	vector<Point> intxnRect;
	Mat homography = findHomography(quad, rect);
	perspectiveTransform(intxnQuad, intxnRect, homography);
	return getArea(intxnRect);
}

vector<Point> QuadGrouper::combineQuads(vector<Point> quadA, vector<Point> quadB) {
	int left, right, topLeftY, topRightY, botLeftY, botRightY;
	topLeftY = topRightY = botLeftY = botRightY = NULL;
	left = right = quadA.at(0).x;
	for (int i=0; i<4; i++) {
		left = min(left, quadA[i].x);
		left = min(left, quadB[i].x);
		right = max(right, quadA[i].x);
		right = max(right, quadB[i].x);
	}
	vector<vector<Point>> horizLines;
	for (int ind1=0; ind1<4; ind1++) {
		int ind2 = (ind1<4) ? ind1+1 : 0;
		if (quadA[ind1].x != quadA[ind2].x) {
			vector<Point> line;
			line.push_back(quadA[ind1]);
			line.push_back(quadA[ind2]);
			horizLines.push_back(line);
		}
		if (quadB[ind1].x != quadB[ind2].x) {
			vector<Point> line;
			line.push_back(quadB[ind1]);
			line.push_back(quadB[ind2]);
			horizLines.push_back(line);
		}
	}
	
	// find the top and bottom horizontal lines
	for (int i=0; i < horizLines.size(); i++) {
		// find slope and intercept
        float m, c, x1, y1, x2, y2, dx, dy;
		x1 = horizLines[i][0].x;
		y1 = horizLines[i][0].y;
		x2 = horizLines[i][1].x;
		y2 = horizLines[i][1].y;
        dx = x2-x1;
		dy = y2-y1;
        // if (dx_i == 0) dx_i = .0001; // none of these should be vertical
        m = (dy / dx); // slope
		c = y1 - (m*x1); // intercept

		int thisLeftY = m*left + c;
		int thisRightY = m*right + c;

		// find lines with highest and lowest intersections
		// with the far left and right sides
		topLeftY = (topLeftY==NULL) ?
				   thisLeftY : min(topLeftY, thisLeftY);
		topRightY = (topRightY==NULL) ?
					thisRightY : min(topRightY, thisRightY);
		botLeftY = (botLeftY==NULL) ?
				   thisLeftY : max(botLeftY, thisLeftY);
		botRightY = (botRightY==NULL) ?
					thisRightY : max(botRightY, thisRightY);
	}

	// create and return the combined quad
	vector<Point> combined;
	combined.push_back(Point(left, topLeftY));
	combined.push_back(Point(right, topRightY));
	combined.push_back(Point(left, botLeftY));
	combined.push_back(Point(right, botRightY));
	return combined;
}

void QuadGrouper::drawQuads() {
	Mat grouped = imgDets->getMat("original")->clone();
	string matName = "grouped";

	drawQuadsHelper(grouped, *groupedLeftQuads, Scalar_<double>(0,0,255));
	drawQuadsHelper(grouped, *groupedRightQuads, Scalar_<double>(0,255,0));
	drawQuadsHelper(grouped, *groupedVertQuads, Scalar_<double>(255,0,0));

	imgDets->insertMat(matName, grouped);
	namedWindow(matName);
	imshow(matName, grouped);
	cvWaitKey(0);
}

void QuadGrouper::drawQuadsHelper(Mat &grouped, vector<vector<Point>> quads, Scalar_<double> color) {

	for (int i=0; i<quads.size(); i++) {
		vector<Point> quad = quads[i];
		line(grouped, quad[0], quad[1], color);
		line(grouped, quad[1], quad[2], color);
		line(grouped, quad[2], quad[3], color);
		line(grouped, quad[3], quad[0], color);
	}
}