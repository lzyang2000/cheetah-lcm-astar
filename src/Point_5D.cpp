//
// Created by anxing.
//
#include "Point_5D.h"

Point_5D::Point_5D(double x_,double y_,double theta_,double length_){
    x=x_;
    y=y_;
    theta=theta_;
    length=length_;
}

Point_5D::Point_5D(double x_,double y_,double theta_,double length_,double alpha_){
    x=x_;
    y=y_;
    theta=theta_;
    length=length_;
    alpha=alpha_;
}

Point_5D::Point_5D(double x_,double y_){
    x=x_;
    y=y_;
    theta=0;
    length=1;
}