//
//  imageDetails.h
//  depthOrderedGrouping
//
//  Created by Lars Jangaard on 11/28/13.
//  Copyright (c) 2013 Lars Jangaard. All rights reserved.
//

#ifndef __depthOrderedGrouping__imageDetails__
#define __depthOrderedGrouping__imageDetails__

#include <iostream>
#include <map>
#include <vector>
#include <string.h>

using namespace std;

class ImageDetails {
  public:
    ImageDetails();
    
    Mat openOriginal(string);
	
    Mat* getMat(string);
    Mat* insertMat(string, Mat);
    
    vector<Vec4i> getLineList(string);
    bool insertLineList(string, vector<int>);
    
  private:
	void showImage(string, Mat);
	
    map<string, Mat> Mats;
    typedef map<string, vector<int>> LineLists;

    int proximity; // for vanishing point regions
    float vertical; // acceptable slope to consider vertical
};

#endif /* defined(__depthOrderedGrouping__imageDetails__) */
