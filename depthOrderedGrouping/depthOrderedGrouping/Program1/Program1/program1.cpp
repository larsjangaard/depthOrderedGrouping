// Program0.cpp
// The program takes an image which it flips, converts to grayscale, blur, create a trackbar
//and draws the edges of the blurred image according to the slide on the track bar
//it also resizes another image and blends it to the input image
// Author: Olajumoke Fajinmi
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <math.h>
#include <iostream>

using namespace cv;
using namespace std;

struct Edge{
	int vertex;
	float parallelism;
	float curvilinearity;
	float orthogonality;
};
float getOverlappingRatio(Vec2i overlapDistance, int dist)
{

	float overlapRatio = ((overlapDistance[1]-overlapDistance[0]) / dist);

	return overlapRatio;
	
}

float angularDifference(Vec4i lineA, Vec4i lineB)
{ 
	int m1, m2;
	int denom1 = (lineA[2] - lineA[0]);

	
	if (denom1 == 0)
	    m1 = 90;
	else
	    m1 = atan((lineA[3] - lineA[1]) / denom1);

	
    int denom2 = (lineB[2] - lineB[0]);
	if (denom2 == 0)
	    m2 = 90;
	else
	    m2 = atan((lineB[3] - lineB[1]) / denom2);

	float angleDiff = abs(m1 - m2);
	return pow(cos(angleDiff), 2);
}

float calcCurvilinearity(float angleDiff, float distanceH, float distanceV)
{
  float x, c1 = 2 * pow(3.0,2.0), c2 = 2 * pow(0.1,2.0), c3= 2 * pow(0.7,2.0);
  distanceH = pow(distanceH,2.0);
  distanceV = pow(distanceV,2.0);
  x =(-(distanceH/c1) - (distanceV/c2) -((1-angleDiff)/c3));

  return exp(x);
}

float calcParallelism(int distance, float angleDiff, float overlappingRatio)
{
  float x, p1=2 * pow(3.0,2.0), p2 = 2 * pow(0.1,2.0), p3 = 2 * pow(0.1,2.0);
  distance = pow(distance,2);
  x = (-(distance / p1) - (pow((1-overlappingRatio),2) / p2) - ((1-angleDiff) / p3));
  
  return exp(x);
}

float calcOrthogonality(int distance,float angleDiff)
{
  float x, o1 = 2 * pow(2.0,2.0), o2 = 2 * pow(0.3,2.0);
  distance = pow(distance,2);
  x = (-(distance/o1)-(angleDiff/o2));

  return exp(x);
}

Vec2i findMinPoint(Vec4i line)
{
	Vec2i minPoints; 
	  if (line[1] > line[3])
	  {
		  minPoints[0] = line[2];
		  minPoints[1] = line[3];
	  }
	  else
	  {
		  minPoints[0] = line[0];
		  minPoints[1] = line[1];
	  }
	  return minPoints;
}

Mat groupLines(vector<Vec4i>& lines)
{
  
  int x = 0;

  size_t sizeL = lines.size();

  int LGsize = sizeL*(sizeL + 1) / 2;

  vector<vector <Edge> > graph (sizeL);
  
  Vec4i lineA, lineB;
  int weights = 5;

  Mat centers, labels;
  //Mat lineVec = Mat(lines);

  Mat lineGraph = Mat::zeros(LGsize, weights, CV_32F);

  float overlappingRatio;
  float curvilinearity, parallelism, orthogonality;
  
  bool overlap;
  int d, distanceA, distanceB, k=0;

  //Note: check for dist when lines dont overlap

  int distance, dist;
  Vec2i overlapPoint, overlapDistance;
  int pointX, pointY;
  int distanceH, distanceV, angleDiff;

  for (size_t i = 0; i < lines.size()-1; i++)
  {
	  //graphElement[0] = (float) i;
	  lineA = lines[i];
	  distanceA = lineA[2] - lineA[0];
	  

	  for(size_t j = i+1; j < lines.size(); j++)
	  {
		  //graphElement[1] = (float) j;
		  lineB = lines[j];
		  distanceB = lineB[2] - lineB[0];

		  angleDiff = angularDifference(lineA, lineB);

		  
		  if(lineA[2] < lineB[0])
		  {
			  int dx= lineA[2]-lineA[0];
			  int dy= lineA[3] - lineA[1];
			  distanceH = lineB[0] - lineA[2];
			  //distanceV = distanceH * dx / dy;
			  
			  if (dy == 0)
				 distanceV = lineA[3]-lineB[1];
			  else
			   distanceV = distanceH * dx / dy;

			  curvilinearity = calcCurvilinearity(angleDiff,distanceH, distanceV);
		  }
		  else if (lineB[2] < lineA[0])
		  {
               int dx= lineB[2]-lineB[0];
			   int dy= lineB[3] - lineB[1];
			   distanceH = lineA[0] - lineB[2];
			   if (dy == 0)
				   distanceV = lineB[3]-lineA[1];
			   else
			   distanceV = distanceH * dx / dy;
			   

			   curvilinearity = calcCurvilinearity(angleDiff, distanceH, distanceV);
		  }

		  if (lineA[0] >= lineB[0] && lineA[0] <= lineB[2])
		  {
			  if (lineA[2] <= lineB[2])
			  {
				overlapPoint =  findMinPoint(lineA);
				overlapDistance[0] = lineA[0] ;
				overlapDistance[1] = lineA[2] ;

			  }

			  else
			  {
				  overlapPoint[0] = lineA[0];
				  overlapPoint[1] = lineA[1];

				  overlapDistance[0] = lineA[0] ;
				  overlapDistance[1] = lineB[2] ;
			  }  

			  dist =distanceB;

			  //find the point X on the line distance
			  //pointX = overlapPoint[0]- lineB[0];
			  if (distanceB == 0)
				  distance = 0;
			  else
			  {
			   //find the point Y on the line distance
			   pointY =  ((overlapPoint[0]- lineB[0]) * (lineB[3] - lineB[1] ))/distanceB;
			   distance = overlapPoint[1] - pointY;
			  }
		  }

		  else if (lineA[2] >= lineB[0] && lineA[2] <= lineB[2])
		  {
			   //overlapPoint[0] = lineA[2];
			   //overlapPoint[1] = lineA[3];

			   overlapDistance[0] = lineB[0] ;
			   overlapDistance[1] = lineA[2] ;
			  //points = lineB[2] - lineB[0];

			 if (distanceB == 0)
				 distance = 0;
			 else
			 {
			  //find the point Y on the line distance
			  pointY =  ((lineA[2]- lineB[0]) * (lineB[3] - lineB[1] ))/distanceB;
			  distance = lineA[3] - pointY;
			 }
		  }  

		  else if (lineB[0] >= lineA[0] && lineB[0] <= lineA[2])
		  {
			  if (lineB[2] <= lineA[2])
			  {
				overlapPoint =  findMinPoint(lineB);
				overlapDistance[0] = lineB[0] ;
			    overlapDistance[1] = lineB[2] ;
			  }

			  else
			  {
				  overlapPoint[0] = lineB[0];
				  overlapPoint[1] = lineB[1];
				  overlapDistance[0] = lineB[0] ;
			      overlapDistance[1] = lineA[2] ;
			  }  

			  dist= distanceA;

			  //find the point X on the line distance
			  //pointX = overlapPoint[0]- lineB[0];
			   
			  if (distanceA == 0)
				  distance = 0;
			  else
			  {
			      //find the point Y on the line distance
			      pointY =  ((overlapPoint[0]- lineA[0]) * (lineA[3] - lineA[1] ))/distanceA;
			      distance = overlapPoint[1] - pointY;
			  }



		  }

		  else if (lineB[2] >= lineA[0] && lineB[2] <= lineA[2])
		  {
			  //overlapPoint[0] = lineB[2];
			  //overlapPoint[1] = lineB[3];

			  overlapDistance[0] = lineB[2] ;
			  overlapDistance[1] = lineA[0] ;

			  dist = lineA[2] -lineA[0];

			  //find the point Y on the line distance
			  pointY =  ((lineB[2]- lineA[0]) * (lineA[3] - lineA[1] ))/distanceA;
			  distance = lineB[3] - pointY;

		  } 

		  // the lines do not overlap
		  else
		  {
			  //confirm this
			  dist = 0;
			  distance = (int)distanceH;

		  }
		  if (dist == 0)
			  overlappingRatio = 0;
		  else
              overlappingRatio = getOverlappingRatio(overlapDistance, dist);

		  parallelism = calcParallelism(distance, angleDiff, overlappingRatio);
	      orthogonality = calcOrthogonality(distance, angleDiff);

		  //graphElement[2] = curvilinearity;
		  //graphElement[3] = parallelism;
		  //graphElement[4] = orthogonality;

		lineGraph.at<float>(k,0) = (float) i;
		lineGraph.at<float>(k,1) = (float) j;
		lineGraph.at<float>(k,2) = curvilinearity;
		lineGraph.at<float>(k,3) = parallelism;
		lineGraph.at<float>(k,4) = orthogonality;

		//graphLines[x] = graphElement;
		++x;
		struct Edge edge;
		edge.vertex = j;
		edge.parallelism = parallelism;
		edge.curvilinearity = curvilinearity;
		edge.orthogonality = orthogonality;
		graph[i].push_back(edge);
		cout<<"curvilinearity"<<curvilinearity <<" \n";
		cout<<"parallelism"<<parallelism <<" \n";
		cout<<"orthogonality"<<orthogonality<<"\n";
	  }
	  ++x;
  }
  int clusters =3;
  kmeans(lineGraph, clusters, labels, cvTermCriteria(1, 10, 1.0), 1, KMEANS_PP_CENTERS, centers);

  return labels;
}
int main(int argc, char** argv)
{


 Mat src = imread("159.jpg");

 Mat dst, cdst, edst, fdst;
 vector<vector<Point> > contours ;
 Mat blurred;
        GaussianBlur(src, blurred, Size(5, 5), 2.0, 2.0);
        imshow("Output Image", blurred);

		Canny(blurred, dst, 20, 40,3);

 //////cvtColor( src, src, CV_BGR2GRAY );
 ///////// Apply Histogram Equalization
 ////// equalizeHist( src, edst );


 //////  Mat blurred;
 //////       GaussianBlur(edst, blurred, Size(5, 5), 2.0, 2.0);
 //////       imshow("Output Image", blurred);

	//////	Canny(blurred, dst, 20, 40,3);
 //Canny(src, dst, 8000, 13000, 7);
		 //Canny(src, dst, 40, 90);
 imwrite("output.jpg", dst);

 cvtColor(dst, cdst, CV_GRAY2BGR);

  vector<Vec4i> lines;
  HoughLinesP(dst, lines, 1,CV_PI/180, 35, 15,2);
  
  Mat labels = groupLines(lines);

  for( size_t i = 0; i < lines.size(); i++ )
  {
	
    Vec4i l = lines[i];


			
			if (i ==0 || i % 3==0)
            line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 2);
			else if (i ==1 || i % 3==1)
            line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,255,0), 2);
			else
            line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255,0,0), 2);
  }

 imshow("source", dst);
 
 imshow("detected lines", cdst);

 waitKey();

 return 0;
}