//
// Created by anxing.
//

#ifndef OCCMAPTRANSFORM_H
#define OCCMAPTRANSFORM_H

#include <iostream>
#include <cmath>
#include <nav_msgs/OccupancyGrid.h>
#include <tf/tf.h>
#include <opencv2/opencv.hpp>
#include "Point_5D.h"

using namespace std;
using namespace cv;


//-------------------------------- Class---------------------------------//
class OccupancyGridParam{

public: // Interface
    void GetOccupancyGridParam(nav_msgs::OccupancyGrid OccGrid);
    void Image2MapTransform(Point_5D& src_point, Point_5D& dst_point);
    void Map2ImageTransform(Point_5D& src_point, Point_5D& dst_point);

private: // Private function

public:  // Public variable
    double resolution;
    int height;
    int width;
    // Origin pose
    double x;
    double y;
    double theta;

private: // Private variable
    // Transform
    Mat R;
    Mat t;

};



#endif //OCCMAPTRANSFORM_H
