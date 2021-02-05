//
// Created by anxing.
//
#ifndef Point_5D_H
#define Point_5D_H

#include <iostream>
#include <cmath>
#include <nav_msgs/OccupancyGrid.h>
#include <tf/tf.h>
#include <opencv2/opencv.hpp>


using namespace std;
using namespace cv;

class Point_5D{
public:
    double x=0;
    double y=0;
    double theta=0;
    double length=0;
    double alpha=0;
    Point_5D(){
        x=0;
        y=0;
        theta=0;
        length=0.6;
    }
    Point_5D(const Point_5D &point){
        x=point.x;
        y=point.y;
        theta=point.theta;
        length=point.length;
        alpha=point.alpha;
    }
    Point_5D(double x_,double y_,double theta_,double length_);
    Point_5D(double x_,double y_,double theta_,double length_,double alpha_);
    Point_5D(double x_,double y_);
};

#endif