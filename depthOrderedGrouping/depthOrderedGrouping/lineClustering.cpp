#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <math.h>
#include <iostream>

using namespace cv;
using namespace std;

struct Edge{
	int vertex;
	double attraction;
	double repulsion;
};
double getOverlappingRatio(Vec2i overlapDistance, int dist)
{

	double overlapRatio = ((overlapDistance[1]-overlapDistance[0]) / dist);

	return overlapRatio;
	
}

double angularDifference(Vec4i lineA, Vec4i lineB)
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

	double angleDiff = abs(m1 - m2);
	return pow(cos(angleDiff), 2);
}

double calcCurvilinearity(double angleDiff, double distanceH, double distanceV)
{
  double x, c1 = 2 * pow(3.0,2.0), c2 = 2 * pow(0.1,2.0), c3= 2 * pow(0.7,2.0);
  distanceH = pow(distanceH,2.0);
  distanceV = pow(distanceV,2.0);
  x =(-(distanceH/c1) - (distanceV/c2) -((1-angleDiff)/c3));

  return exp(x);
}

double calcParallelism(int distance, double angleDiff, double overlappingRatio)
{
  double x, p1=2 * pow(3.0,2.0), p2 = 2 * pow(0.1,2.0), p3 = 2 * pow(0.1,2.0);
  distance = pow(distance,2);
  x = (-(distance / p1) - (pow((1-overlappingRatio),2) / p2) - ((1-angleDiff) / p3));
  
  return exp(x);
}

double calcOrthogonality(int distance,double angleDiff)
{
  double x, o1 = 2 * pow(2.0,2.0), o2 = 2 * pow(0.3,2.0);
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

//Mat createMat(int SizeL)
//{
//	vector< Mat <Vec2d>>
//}

Mat groupLines(vector<Vec4i>& lines)
{
  
  int x = 0;

  int sizeL = (int) lines.size();

  int LGsize = sizeL*(sizeL + 1) / 2;

  vector<vector <Edge> > graph (sizeL);

  vector<Vec4i> cluster1, cluster2, cluster3;
  
  Vec4i lineA, lineB;
  int col =1;
  int weights = 5;

  Mat center, label, eigenVec, eigenVal;

  Mat lambda = Mat::zeros(sizeL, col, CV_64F);
  Mat lineGraph = Mat::zeros(sizeL, sizeL, CV_64F);
  Mat lineGraphW = Mat::zeros(sizeL, sizeL, CV_64F);
  Mat degreeMatrix = Mat::zeros(sizeL, sizeL, CV_64F);
  Mat degreeMatrixR = Mat::zeros(sizeL, sizeL, CV_64F);
  Mat degreeMatrixA = Mat::zeros(sizeL, sizeL, CV_64F);
  Mat scaledLineGraph = Mat::zeros(sizeL, sizeL, CV_64F);

  double overlappingRatio;
  double curvilinearity, parallelism, orthogonality;
 
  bool overlap;
  int d, distanceA, distanceB, k=0;

  //Note: check for dist when lines dont overlap

  int distance, dist;
  Vec2i overlapPoint, overlapDistance;
  int pointX, pointY;
  int distanceH, distanceV, angleDiff;

  for (size_t i = 0; i < lines.size(); i++)
  {
	  //graphElement[0] = (float) i;
	  lineA = lines[i];
	  distanceA = lineA[2] - lineA[0];
	  

	  for(size_t j = i; j < lines.size(); j++)
	  {
		  if(i==j)
		  {
			  curvilinearity = 0;
			  parallelism = 0;
			  orthogonality = 0;

		  }
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

		
		struct Edge edge;
		edge.vertex = j;
		edge.attraction = curvilinearity + parallelism;
		edge.repulsion = orthogonality;
		graph[i].push_back(edge);
		edge.vertex = i;
		graph[j].push_back(edge);

		degreeMatrixA.at<double>(i,i) += parallelism + curvilinearity;
		degreeMatrixR.at<double>(i,i) += orthogonality;
		lineGraph.at<double>(i,j) = parallelism + curvilinearity - orthogonality;
		
		cout<<"curvilinearity"<<curvilinearity <<" \n";
		cout<<"parallelism"<<parallelism <<" \n";
		cout<<"orthogonality"<<orthogonality<<"\n";

		cout<< "attraction"<<lineGraph.at<double>(i,j)<<"\n";
		  
	  }
	  
  }
  for (int a =0; a<sizeL; a++)
  {
	 for (int b =0; b<sizeL; b++)
	 {
		 degreeMatrix.at<double>(a,b) = degreeMatrixA.at<double>(a,b) + degreeMatrixR.at<double>(a,b);
		 lineGraphW.at<double>(a,b) = lineGraph.at<double>(a,b) + degreeMatrixR.at<double>(a,b);

		 scaledLineGraph.at<double>(a,b) = (pow(degreeMatrix.at<double>(a,a), -0.5) * lineGraphW.at<double>(a,b) * pow(degreeMatrix.at<double>(b,b), -0.5));
	 }
  }

  
  //multiply(degreeMatrix, lineGraphW, scaledLineGraph);


  eigen(scaledLineGraph, eigenVal, eigenVec);
  //eigen(lineGraph, eigenVal, eigenVec);
  cout<< "eigenval"<<eigenVal.size()<<"\n";
  cout<< "eigenvec"<<eigenVec.size()<<"\n";
  cout<< "line"<<lines.size()<<"\n";

  for (int i = 0; i < sizeL; i++)
  {
	  lambda.at<double>(i) = eigenVec.at<double>(i,3);
  }

  //exception at memory location
    /*int clusters =3;
    kmeans(lambda, clusters, label, cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10000, 0.0001), 1, KMEANS_PP_CENTERS, center);
*/
  int w=0, e=0, r=0;
   for(int i=0;i<sizeL;i++) {

		  
	}

  return label;
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