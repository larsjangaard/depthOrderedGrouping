#include "LineGrouping.h"


LineGrouping::LineGrouping(void)
{
}

double getOverlappingRatio(Vec2i overlapDistance, int dist)
{

	double overlapRatio = ((overlapDistance[1] - overlapDistance[0]) / dist);

	return overlapRatio;
	
}

double angularDifference(Vec4i lineA, Vec4i lineB)
{ 
	double m1, m2;
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


//This method calculates the curvilineraity of a pair of lines
//It takes as input the angular difference between the lines,
//the horizontal and vertical difference between the start point of one line and 
//the end point of the other.
//It returns an exponential value which is the result of the curvilinearity calcualtion
double calcCurvilinearity(double angleDiff, double distanceH, double distanceV)
{

  double x, c1 = 2 * pow(3.0,2.0), c2 = 2 * pow( 0.1, 2.0 ), c3= 2 * pow( 0.7, 2.0 );

  distanceH = pow( distanceH, 2.0 );
  distanceV = pow( distanceV, 2.0 );

  x =( -(distanceH / c1) - (distanceV / c2 ) -( (1 - angleDiff) / c3) );

  return exp(x);

}

//This method calculates the parallelism of a pair of lines
//It takes as input the angular difference between the lines,
//the overlapping ratio of the lines and the minimum distance between the endpoints of 
//one line to the line distance of the other
//It returns an exponential value which is the result of the parallelism calcualtion
double calcParallelism(int distance, double angleDiff, double overlappingRatio)
{
  double x, p1 = 2 * pow( 3.0, 2.0), p2 = 2 * pow( 0.1, 2.0), p3 = 2 * pow( 0.1, 2.0);

  distance = pow( distance, 2);
  x = ( -( distance / p1 ) - ( pow( ( 1 - overlappingRatio ), 2) / p2) - ( ( 1 - angleDiff ) / p3 ) );
  
  return exp(x);
}

//This method calculates the orthogonality of a pair of lines
//It takes as input the angular difference between the lines
//and the minimum distance between the endpoints of one line to the line distance 
//of the other
//It returns an exponential value which is the result of the orthogonality calcualtion
double calcOrthogonality(int distance,double angleDiff)
{ 

  double x, o1 = 2 * pow( 2.0, 2.0), o2 = 2 * pow( 0.3, 2.0);
  distance = pow( distance, 2);
  x = ( -( distance / o1) - ( angleDiff / o2) );

  return exp(x);
}

//The method displays the lines on an image depending on the cluster
//which they belong
//It takes as input a vector of vector cluster line points and the image which the
//lines will overlay.
//currently, red is assigned to lines of cluster 1...
void displayLines(vector<vector<Vec4i>>& Clusters, Mat image)
{
  
  for( size_t i = 0; i < Clusters.size(); i++ )
  {
	vector<Vec4i> cluster = Clusters[i];
	for ( size_t j = 0; j < cluster.size(); j++ )
	{
	
         Vec4i l = cluster[j];

		 if (i ==0)
         line( image, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 2);

		 else if (i ==1)
         line( image, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,255,0), 2);

		 else if (i ==2)
         line( image, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255,0,0), 2);
	}
  }

  imshow("detected lines", image);
   waitKey();
}

vector<vector<Vec4i>> reclusterLines(LineFinder* linefinder,vector<Vec4i> cluster1, vector<Vec4i> cluster2, vector<Vec4i> cluster3)
{
	
	//LineFinder::findMeanVanPts(vector<Vec4i> *vanPts) 
	vector<vector<Vec4i>> t;
	return t;
}

//This method call the getLineList in the LineFinder class
//to get vectors of good lines.
//It takes as an input a pointer to the LineFinder class
//and returns a merged vector of three vectors
vector<Vec4i> mergeLineVector (ImageDetails* imageDetails)
{
  vector<Vec4i> lines;
  vector<Vec4i>* linesL, *linesR, *linesV ;

  
  linesL = imageDetails->getLineList("leftVanLines");
  linesR = imageDetails->getLineList("rightVanLines");
  linesV = imageDetails->getLineList("vertLines");

  for (size_t i = 0; i < linesL->size(); i++)
  {
      lines.push_back(linesL->at(i));
  }

  for (size_t i = 0; i < linesR->size(); i++)
  {
	  lines.push_back(linesR->at(i));
  }

  for (size_t i = 0; i < linesV->size(); i++)
  {
	  lines.push_back(linesV->at(i));
  }

  return lines;
}


vector<vector<Vec4i>> groupLines(Mat image,LineFinder* linefinder, ImageDetails* imageDetails)
{
  
  double overlappingRatio;
  double curvilinearity, parallelism, orthogonality;
 
  int distanceA, distanceB;

  int distance, dist;
  Vec2i overlapPoint, overlapDistance;
  int pointY;
  int distanceH, distanceV, angleDiff;

  vector<Vec4i> cluster1, cluster2, cluster3;

  vector<vector<Vec4i>> Clusters;
  
  Vec4i lineA, lineB;
  int col =1;
  int center = 0;

  Mat eigenVec, eigenVal;

  vector<Vec4i> lines = mergeLineVector(imageDetails);
  int sizeL = (int) lines.size();

  Mat lambda = Mat::zeros(sizeL, col, CV_64F);
  Mat lineGraph = Mat::zeros(sizeL, sizeL, CV_64F);
  Mat lineGraphW = Mat::zeros(sizeL, sizeL, CV_64F);
  Mat degreeMatrix = Mat::zeros(sizeL, sizeL, CV_64F);
  Mat degreeMatrixR = Mat::zeros(sizeL, sizeL, CV_64F);
  Mat degreeMatrixA = Mat::zeros(sizeL, sizeL, CV_64F);
  Mat scaledLineGraph = Mat::zeros(sizeL, sizeL, CV_64F);


  for (size_t i = 0; i < lines.size(); i++)
  {
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


			 overlapDistance[0] = lineB[0] ;
			 overlapDistance[1] = lineA[2] ;

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

			  overlapDistance[0] = lineB[2] ;
			  overlapDistance[1] = lineA[0] ;

			  dist = distanceA;

			  //find the point Y on the line distance
			  pointY =  ((lineB[2]- lineA[0]) * (lineA[3] - lineA[1] ))/distanceA;
			  distance = lineB[3] - pointY;

		  } 

		  // if the lines do not overlap
		  else
		  {
			  
			  dist = 0;
			  distance = (int)distanceH;

		  }
		  if (dist == 0)
			  overlappingRatio = 0;
		  else
              overlappingRatio = getOverlappingRatio(overlapDistance, dist);

		  parallelism = calcParallelism(distance, angleDiff, overlappingRatio);
	      orthogonality = calcOrthogonality(distance, angleDiff);

        //build the diagonal/degree matrix for the attraction weights
		degreeMatrixA.at<double>(i,i) += parallelism + curvilinearity;

		//build the diagonal/degree matrix for the repulsion weights
		degreeMatrixR.at<double>(i,i) += orthogonality;

		//build the adjacency matrix
		lineGraph.at<double>(i,j) = parallelism + curvilinearity - orthogonality;
		 
	  }
	  
  }

  for (int a =0; a<sizeL; a++)
  {
	 for (int b =0; b<sizeL; b++)
	 {
		 //build the degree or diagonal matrix using the attraction and repulsion degree matrix
		 degreeMatrix.at<double>(a,b) = degreeMatrixA.at<double>(a,b) + degreeMatrixR.at<double>(a,b);
		 
		 //build the laplacian matrix
		 lineGraphW.at<double>(a,b) =  degreeMatrixR.at<double>(a,b) - lineGraph.at<double>(a,b);
		 //lineGraphW.at<double>(a,b) = lineGraph.at<double>(a,b) + degreeMatrixR.at<double>(a,b);


		 //build the symmetric matrix
		 scaledLineGraph.at<double>(a,b) = (pow(degreeMatrix.at<double>(a,a), -0.5) * lineGraphW.at<double>(a,b) * pow(degreeMatrix.at<double>(b,b), -0.5));

	 }
  }

  
  eigen(scaledLineGraph, eigenVal, eigenVec);
  double eigenGap = 0.0;
  int g = 0;
  cout<< "eigenval"<<eigenVal.size()<<"\n";
  cout<< "eigenvec"<<eigenVec.size()<<"\n";
  cout<< "line"<<lines.size()<<"\n";

 
  for (int i = 0; i < sizeL; i++)
  {
	  if ( eigenGap < eigenVal.at<double>(i) ){
		  eigenGap = eigenVal.at<double>(i);
		  g = i;
	  }
	  cout<< "eigenval"<<eigenVal.at<double>(i)<<"\n";
  }


  for (int i = 0; i < sizeL; i++)
  {
	  
	  lambda.at<double>(i) = eigenVec.at<double>(i,g);

	  if (lambda.at<double>(i) <= center )
	   {
		   cluster1.push_back(lines[i]);
           cout<< "the lambdaValue"<<lambda.at<double>(i)<<"\n";
	   }

	  else if (cvIsNaN(lambda.at<double>(i))== 1)
	  {

		  	  cluster2.push_back(lines[i]);
			  cout<< "the lambdaValue ::"<<lambda.at<double>(i)<<"\n";

	  }

	  else if (lambda.at<double>(i) > center )
	  {

		      cluster3.push_back(lines[i]);
	          cout<< "the lambdaValue :::"<<lambda.at<double>(i)<<"\n";
	  }
	  	  
  }

  vector<vector<Vec4i>> Clusters1 = reclusterLines(linefinder,cluster1, cluster2, cluster3);
  Clusters.push_back(cluster1);
  Clusters.push_back(cluster2);
  Clusters.push_back(cluster3);


  displayLines(Clusters, image);
  cout<<eigenGap;

  return Clusters;
}

LineGrouping::~LineGrouping(void)
{
}
