#include "LineGrouping.h"

LineFinder* linefinder;
ImageDetails* imageDetails;

LineGrouping::LineGrouping(LineFinder* lineFinder, ImageDetails* imagedetails)
{
	linefinder = lineFinder;
	imageDetails = imagedetails;

}

//The method calculates and returns the overlapping ratio for a pair of lines
//it takes as input a vec2i to calculate the distance and an integer which is 
//the distance of one line.
double LineGrouping::getOverlappingRatio(Vec2i overlapDistance, int dist)
{

	double overlapRatio = ((overlapDistance[1] - overlapDistance[0]) / dist);

	return overlapRatio;
	
}

//The method calculates and returns the angular difference of 2 lines
//it takes as an input the vector of 4 points of the line segment
//The angle of each line is calculated as the arc tan of its slope
//and the angular difference is calculated by subtracting the two angles
double LineGrouping::angularDifference(Vec4i lineA, Vec4i lineB)
{ 
	double m1, m2;
	double denom1 =(double) (lineA[2] - lineA[0]);

	//the line is vertical therefore its angle is 90 degree
	if (denom1 == 0)
	    m1 = 90;

	else
	    m1 = atan((lineA[3] - lineA[1]) / denom1);

	
    double denom2 = (double)(lineB[2] - lineB[0]);
	
	//the line is vertical therefore its angle is 90 degrees
	if (denom2 == 0)
	    m2 = 90;
	else
	    m2 = atan((lineB[3] - lineB[1]) / denom2);

	double angleDiff = abs(m1 - m2);

	return pow(cos(angleDiff), 2.0);
}
//The method finds the minimum endpoint of a line segment
//it takes as input a line and compares the two y values at the endpoint
//When the minimum endpoint has been calculated, it returns the x and y values
//for that point
Vec2i LineGrouping::findMinPoint(Vec4i line)
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
double LineGrouping::calcCurvilinearity(double angleDiff, double distanceH, double distanceV)
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
//It returns an exponential value which is the result of the parallelism calculation
double LineGrouping::calcParallelism(int distance, double angleDiff, double overlappingRatio)
{
  double x, p1 = 2 * pow( 3.0, 2.0), p2 = 2 * pow( 0.1, 2.0), p3 = 2 * pow( 0.1, 2.0);

  distance = pow( distance, 2.0);

  x = ( -( distance / p1 ) - ( pow( ( 1 - overlappingRatio ), 2) / p2) - ( ( 1 - angleDiff ) / p3 ) );
  
  return exp(x);
}

//This method calculates the orthogonality of a pair of lines
//It takes as input the angular difference between the lines
//and the minimum distance between the endpoints of one line to the line distance 
//of the other
//It returns an exponential value which is the result of the orthogonality calcualtion
double LineGrouping::calcOrthogonality(int distance,double angleDiff)
{ 

  double x, o1 = 2 * pow( 2.0, 2.0), o2 = 2 * pow( 0.3, 2.0);

  distance = pow( distance, 2.0);

  x = ( -( distance / o1) - ( angleDiff / o2) );

  return exp(x);
}

//The method displays the lines on an image depending on the cluster
//which they belong
//It takes as input a vector of vector cluster line points and the image which the
//lines will overlay.
void LineGrouping::displayLines(vector<vector<Vec4i>>& Clusters, Mat image)
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

//The method calls the findvalidlines in the linefinder class with a vector of lines and
//three strings
//it takes as an input three vectors of lines which where obtained using the graphcuts method 
//the method returns no values because the reclassified lines are stored by linefinder
void LineGrouping::reclusterLines(vector<Vec4i> cluster1, vector<Vec4i> cluster2, vector<Vec4i> cluster3)
{
	linefinder->findValidLines(&cluster1,"left", "right", "vert");
	linefinder->findValidLines(&cluster2,"left", "right", "vert");
	linefinder->findValidLines(&cluster3,"left", "right", "vert");

}

//This method calls the getLineList in the LineFinder class
//to get vectors of good lines.
//It takes as an input a pointer to the LineFinder class. For
//each call, it receives a vector of lines which have the same
//name as was passed in the method.
//The ouput is a merged vector of the three vectors that were received
vector<Vec4i> LineGrouping::mergeLineVector (ImageDetails* imageDetails)
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



vector<vector<Vec4i>> LineGrouping::groupLines(Mat image)
{
  
  //get the vector of lines
  vector<Vec4i> lines = mergeLineVector(imageDetails);

  int sizeL = (int) lines.size();

  vector<Vec4i> cluster1, cluster2, cluster3;

  vector<vector<Vec4i>> Clusters;
  
  int clusters =3;

  Vec4i lineA, lineB;

  int col = 1;

  Mat center, label, eigenVec, eigenVal;

  //intialize the matrices that will be used spectral clustering
  Mat lambda = Mat::zeros(sizeL, clusters, CV_32F);
  Mat sumColumn = Mat::zeros(sizeL, col, CV_32F);
  Mat nLambda = Mat::zeros(sizeL, clusters, CV_32F);
  Mat identityMatrix = Mat::zeros(sizeL, sizeL, CV_64F);
  Mat lineGraph = Mat::zeros(sizeL, sizeL, CV_64F);
  Mat lineGraphW = Mat::zeros(sizeL, sizeL, CV_64F);
  Mat degreeMatrix = Mat::zeros(sizeL, sizeL, CV_64F);
  Mat degreeMatrixR = Mat::zeros(sizeL, sizeL, CV_64F);
  Mat degreeMatrixA = Mat::zeros(sizeL, sizeL, CV_64F);
  Mat scaledLineGraph = Mat::zeros(sizeL, sizeL, CV_64F);

  double overlappingRatio;
  double curvilinearity, parallelism, orthogonality;
 
  int distanceA, distanceB, k=0;

  
  int distance, dist;
  Vec2i overlapPoint, overlapDistance;
  int pointY;
  int distanceH, distanceV, angleDiff;

  for (int i = 0; i < sizeL; i++)
  {
	  double scalar = 1;
	  //build the identity matrix
	  identityMatrix.at<double>(i,i) = 1 ;

	  lineA = lines[i];

	  //get the length of line a
	  distanceA = lineA[2] - lineA[0];
	  

	  for(int j = i; j < sizeL; j++)
	  {
		  
		  if(i==j)
		  {
			  curvilinearity = 0;
			  parallelism = 0;
			  orthogonality = 0;

		  }

		  lineB = lines[j];

		  // get the length of line b
		  distanceB = lineB[2] - lineB[0];

		  angleDiff = angularDifference(lineA, lineB);

		  
		  if(lineA[2] < lineB[0])
		  {
			  int dx= lineA[2] - lineA[0];
			  int dy= lineA[3] - lineA[1];
			  distanceH = lineB[0] - lineA[2];
			  
			  
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
				//get the minimum endpoint for line a
				overlapPoint =  findMinPoint(lineA);

				//assign overlap points for the line being checked.
				//this will be used to calculate the overlapping ratio
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
			   //find the point that the endpoint overlaps on the line distance
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
			  //find the point that the endpoint overlaps on the line distance
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
			      //find the point that the endpoint overlaps on the line distance
			      pointY =  ((overlapPoint[0]- lineA[0]) * (lineA[3] - lineA[1] ))/distanceA;
			      distance = overlapPoint[1] - pointY;
			  }



		  }

		  else if (lineB[2] >= lineA[0] && lineB[2] <= lineA[2])
		  {

			  overlapDistance[0] = lineB[2] ;
			  overlapDistance[1] = lineA[0] ;

			  dist = distanceA;

			  //find the point that the endpoint overlaps on the line distance
			  pointY =  ((lineB[2]- lineA[0]) * (lineA[3] - lineA[1] ))/distanceA;
			  distance = lineB[3] - pointY;

		  } 

		 else if (lineA[1] >= lineB[1] && lineA[1] <= lineB[3])
		  {
			  if (lineA[3] <= lineB[3])
			  {
				overlapPoint =  findMinPoint(lineA);
				overlapDistance[0] = lineA[1] ;
				overlapDistance[1] = lineA[3] ;

			  }

			  else
			  {
				  overlapPoint[0] = lineA[0];
				  overlapPoint[1] = lineA[1];

				  overlapDistance[0] = lineA[1] ;
				  overlapDistance[1] = lineB[3] ;
			  }  

			  dist = lineB[3] - lineB[1];

			  if (dist == 0)
				  distance = 0;
			  else
			  {
			   //find the point that the endpoint overlaps on the line distance
			   pointY =  ((overlapPoint[0]- lineB[0]) * (lineB[3] - lineB[1] ))/dist;
			   distance = overlapPoint[1] - pointY;
			  }
		  }

          else if (lineA[3] >= lineB[1] && lineA[3] <= lineB[3])
		  {


			 overlapDistance[0] = lineB[1] ;
			 overlapDistance[1] = lineA[3] ;

			 dist = lineB[3] - lineB[1];

			 if (dist == 0)
				 distance = 0;
			 else
			 {
			  //find the point that the endpoint overlaps on the line distance
			  pointY =  ((lineA[2]- lineB[0]) * (lineB[3] - lineB[1] ))/dist;
			  distance = lineA[2] - pointY;
			 }
		  }  
		  else if (lineB[1] >= lineA[1] && lineB[1] <= lineA[3])
		  {
			  if (lineB[3] <= lineA[3])
			  {
				overlapPoint =  findMinPoint(lineB);
				overlapDistance[0] = lineB[1] ;
			    overlapDistance[1] = lineB[3] ;
			  }

			  else
			  {
				  overlapPoint[0] = lineB[0];
				  overlapPoint[1] = lineB[1];
				  overlapDistance[0] = lineB[1] ;
			      overlapDistance[1] = lineA[3] ;
			  }  

			  dist= lineA[3] - lineA[1];

			   
			  if (dist== 0)
				  distance = 0;
			  else
			  {
			      //find the point that the endpoint overlaps on the line distance
			      pointY =  ((overlapPoint[0]- lineA[1]) * (lineA[2] - lineA[0] ))/dist;
			      distance = overlapPoint[1] - pointY;
			  }



		  }

		  else if (lineB[3] >= lineA[1] && lineB[3] <= lineA[1])
		  {

			  overlapDistance[0] = lineB[3] ;
			  overlapDistance[1] = lineA[1] ;

			  dist = lineA[3] - lineA[1];

			  if (dist == 0)
				  distance = 0;
			  else
			  {
			  //find the point Y on the line distance
			  pointY =  ((lineB[3]- lineA[1]) * (lineA[2] - lineA[0] ))/dist;
			  distance = lineB[2] - pointY;
			  }

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
		degreeMatrixA.at<double>(i,i) += abs(parallelism + curvilinearity) ;

		//build the diagonal/degree matrix for the repulsion weights
		degreeMatrixR.at<double>(i,i) += abs(orthogonality) ;

		//build the adjacency matrix
		lineGraph.at<double>(i,j) = abs(parallelism + curvilinearity - orthogonality) ;
		
		 
	  }
	  
  }

  for (int a =0; a<sizeL; a++)
  {

	 for (int b =0; b<sizeL; b++)
	 {
		 //build the degree or diagonal matrix using the attraction and repulsion degree matrix
		 degreeMatrix.at<double>(a,b) = degreeMatrixA.at<double>(a,b) + degreeMatrixR.at<double>(a,b) + 2*identityMatrix.at<double>(a,b);
		 
		 //build the laplacian matrix
		 lineGraphW.at<double>(a,b) =  degreeMatrixR.at<double>(a,b) + lineGraph.at<double>(a,b) + identityMatrix.at<double>(a,b);
		 
		 

		 //build the symmetric matrix
		 scaledLineGraph.at<double>(a,b) = (pow(degreeMatrix.at<double>(a,a), -0.5) * lineGraphW.at<double>(a,b) * pow(degreeMatrix.at<double>(a,a), -0.5));
		 


	 }
  }

  //get the eigenvalues and eigen vectors for the scaled matrix
  eigen(scaledLineGraph, eigenVal, eigenVec);

  for (int n = 0; n < sizeL; n++)
  {
	  for (int m = 0; m < clusters; m++)
	  {

      //using the k eigen vector clustering methods,
	  //add the first the k eigen vectors to a matrix
	  //where each eigenvector is added to a new column
	  //of the lambda matrix
	  lambda.at<float>(n,m) = (float)eigenVec.at<double>(n,m);
	  float val =pow((double)lambda.at<float>(n,m), 2.0);
	  sumColumn.at<float>(n) =+ val;
	  }
  }

  //renormalize the vector for clustering
  // mathematical calculation can be found in http://ai.stanford.edu/~ang/papers/nips01-spectral.pdf
  for (int p = 0; p < sizeL; p++)
  {
	  for (int q = 0; q < clusters; q++)
	  {
		  nLambda.at<float>(p,q) = lambda.at<float>(p,q) / pow((double)sumColumn.at<float>(p), 0.5);
	  }
	  	  
  }
    //using the opencv kmeans method, cluster the normalized vector, nLamba into "three" clusters
    kmeans(nLambda, clusters, label, TermCriteria(CV_TERMCRIT_ITER, 10000, 0.0001), 1, KMEANS_PP_CENTERS, center);
	
	for(int i=0;i<clusters;i++) {
		cout << "center "<<i<<": ";
		for(int j=0;j<center.cols;j++) cout << center.at<float>(i,j)<<",";
		cout << endl;
	}
	
     for (int i = 0; i < sizeL; i++)
     {
	  	  
		  cout<< "the labelValue"<<label.at<int>(i,0)<<"\n";
		  cout<< "the lambdaValue"<<nLambda.at<float>(i,0)<<"\n";

		 //for each item in the output array where the index of the array corresponds to the line,
		 //check iwhich cluster the line has been assigned.
		 //based on its assigned cluster id, assign the line to that cluster
	     if (label.at<int>(i,0) == 0 )
	     {
		   cluster1.push_back(lines[i]);
           
	     }

	     else if (label.at<int>(i,0) == 1 )
	     {

		  	  cluster2.push_back(lines[i]);
			  //cout<< "the lambdaValue ::"<<nLambda.at<float>(i)<<"\n";

	     }

		  else if (label.at<int>(i,0) == 2 )
		  {
		  	  cluster3.push_back(lines[i]);
			  //cout<< "the lambdaValue :"<<nLambda.at<float>(i)<<"\n";
		  }
	  
}
 
  //recluster the lines based on the mean vanishing point of each cluster
  reclusterLines(cluster1, cluster2, cluster3);

  Clusters.push_back(cluster1);
  Clusters.push_back(cluster2);
  Clusters.push_back(cluster3);

  //display the lines of each vectors with a color assigned to each cluster
  displayLines(Clusters, image);
  

  return Clusters;
}

//LineGrouping::~LineGrouping(LineFinder* lineFinder, ImageDetails* imagedetail)
//{
//}
