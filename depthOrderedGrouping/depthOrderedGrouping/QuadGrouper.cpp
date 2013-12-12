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

// group all the same-oriented quads if they intersect enough
// do this in pairs and continue until no more can be combined
// do this for all three orientations
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

// see if two quadrilaterals combine enough
// if they do, return true and combine them into a larger quad
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
			if (quadA[0].x == quadA[1].x ||
				quadA[0].x == quadA[2].x ||
				quadA[0].x == quadA[3].x)
			*combined = combineQuads(quadA, quadB);
			else *combined = combineVertQuads(quadA, quadB);
			return true;
		}
	}
	return false;
}

// get the quad formed by two intersecting quads
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

// rectify a quadrilateral
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

// get the area of a rectified quad
float QuadGrouper::getArea(vector<Point> rect) {
	float sideA = norm(rect.at(0) - rect.at(1));
	float sideB = norm(rect.at(1) - rect.at(2));
	return sideA*sideB;
}

// get the area of the intersection between two rectified quads
float QuadGrouper::getIntxnArea(vector<Point> quad, vector<Point> rect, vector<Point> intxnQuad) {
	vector<Point> intxnRect;
	Mat homography = findHomography(quad, rect);
	perspectiveTransform(intxnQuad, intxnRect, homography);
	return getArea(intxnRect);
}

// combine quads that have vertical lines
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

// combine quads that don't have vertical lines
vector<Point> QuadGrouper::combineQuads(vector<Point> quadA, vector<Point> quadB) {
	Point leftFinal, rightFinal, topFinal, bottomFinal;
	
	// initial quad points
	Point leftA, rightA, topA, bottomA;
	Point leftB, rightB, topB, bottomB;

	// additional new quad point candidates
	Point leftC, rightC, topC, bottomC;
	Point leftD, rightD, topD, bottomD;

	// temp starting values
	leftA = rightA = topA = bottomA = quadA[0];
	leftB = rightB = topB = bottomB = quadB[0];

	// determine the corners of each quad
	for (int i=1; i<4; i++) {
		if (quadA[i].x < leftA.x) leftA = quadA[i];
		if (quadA[i].x > rightA.x) rightA = quadA[i];
		if (quadA[i].y < topA.y) topA = quadA[i];
		if (quadA[i].y > bottomA.y) bottomA = quadA[i];

		if (quadB[i].x < leftB.x) leftB = quadB[i];
		if (quadB[i].x > rightB.x) rightB = quadB[i];
		if (quadB[i].y < topB.y) topB = quadB[i];
		if (quadB[i].y > bottomB.y) bottomB = quadB[i];
	}

	// preliminary check for extremes of existing quads
	leftFinal = (leftA.x < leftB.x) ? leftA : leftB;
	rightFinal = (rightA.x > rightB.x) ? rightA : rightB;
	topFinal = (topA.y < topB.y) ? topA : topB;
	bottomFinal = (bottomA.y > bottomB.y) ? bottomA : bottomB;

	// find intersections of lines around each corner

	leftC = getIntxnPts(leftA, topA, leftB, bottomB);
	leftD = getIntxnPts(leftA, bottomA, leftB, topB);

	rightC = getIntxnPts(rightA, topA, rightB, bottomB);
	rightD = getIntxnPts(rightA, bottomA, rightB, topB);

	topC = getIntxnPts(leftA, topA, rightA, topA);
	topD = getIntxnPts(rightA, topA, leftB, topB);

	bottomC = getIntxnPts(leftA, bottomA, rightB, bottomB);
	bottomD = getIntxnPts(rightA, bottomA, leftB, bottomB);
	
	// determine the most extreme points

	if (leftFinal.x < leftC.x) leftFinal = leftC;
	if (leftFinal.x < leftD.x) leftFinal = leftD;

	if (rightFinal.x > rightC.x) rightFinal = rightC;
	if (rightFinal.x > rightD.x) rightFinal = rightD;

	if (topFinal.y < topC.y) topFinal = topC;
	if (topFinal.y < topD.y) topFinal = topD;

	if (bottomFinal.y > bottomC.y) bottomFinal = bottomC;
	if (bottomFinal.y > bottomD.y) bottomFinal = bottomD;

	// create and return the combined quad
	vector<Point> combined;
	combined.push_back(leftFinal);
	combined.push_back(topFinal);
	combined.push_back(rightFinal);
	combined.push_back(bottomFinal);
	return combined;
}

// return the intersection of two lines
Point QuadGrouper::getIntxnPts(Point A1, Point A2, Point B1, Point B2) {
	float mA, cA, dx_A, dy_A;
	float mB, cB, dx_B, dy_B;
	float intxnX, intxnY;

	dx_A = A2.x - A1.x;
	dy_A = A2.y - A1.y;
	
	if (dx_A == 0) dx_A = .0001; // vertical - shouldn't happen here
	mA = (dy_A / dx_A); // slope
	cA = A1.y - (mA*A1.x); // intercept

	dx_B = B2.x - B1.x;
	dy_B = B2.y - B1.y;

	if (dx_B == 0) dx_B = -.0001; // vertical - shouldn't happen here
	mB = (dy_B / dx_B); // slope
	cB = B1.y - (mB*B1.x);

	if( (mA - mB) == 0) { // parallel - this shouldn't happen
		mA += .0001;
		mB -= .0001;
	}
	
	// intersection
	intxnX = (cB - cA) / (mA - mB);
	intxnY = (mA*intxnX) + cA;
	return Point(intxnX, intxnY);
}

// creates a mat to store and show
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

// draws the lines
void QuadGrouper::drawQuadsHelper(Mat &grouped, vector<vector<Point>> quads, Scalar_<double> color) {

	for (int i=0; i<quads.size(); i++) {
		vector<Point> quad = quads[i];
		line(grouped, quad[0], quad[1], color);
		line(grouped, quad[1], quad[2], color);
		line(grouped, quad[2], quad[3], color);
		line(grouped, quad[3], quad[0], color);
	}
}