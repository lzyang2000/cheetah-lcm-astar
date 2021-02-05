//
// Created by anxing.
//

#include "OccMapTransform.h"


void OccupancyGridParam::GetOccupancyGridParam(nav_msgs::OccupancyGrid OccGrid)
{
    // Get parameter
    resolution = OccGrid.info.resolution;
    height = OccGrid.info.height;
    width = OccGrid.info.width;
    x = OccGrid.info.origin.position.x;
    y = OccGrid.info.origin.position.y;

    double roll, pitch, yaw;
    geometry_msgs::Quaternion q = OccGrid.info.origin.orientation;
    tf::Quaternion quat(q.x, q.y, q.z, q.w); // x, y, z, w
    tf::Matrix3x3(quat).getRPY(roll, pitch, yaw);
    theta = yaw;

    // Calculate R, t
    R = Mat::zeros(2,2, CV_64FC1);
    R.at<double>(0, 0) = resolution * cos(theta);
    R.at<double>(0, 1) = resolution * sin(-theta);
    R.at<double>(1, 0) = resolution * sin(theta);
    R.at<double>(1, 1) = resolution * cos(theta);
    t = Mat(Vec2d(x, y), CV_64FC1);
}

void OccupancyGridParam::Image2MapTransform(Point_5D& src_point, Point_5D& dst_point)
{
    // Upside down
    Mat P_src = Mat(Vec2d(src_point.x, height - 1 - src_point.y), CV_64FC1);
    // Rotate and translate
    Mat P_dst = R * P_src + t;

    dst_point.x = P_dst.at<double>(0, 0);
    dst_point.y = P_dst.at<double>(1, 0);
    dst_point.theta= src_point.theta;
    dst_point.length = src_point.length;
    dst_point.alpha = src_point.alpha;
}

void OccupancyGridParam::Map2ImageTransform(Point_5D& src_point, Point_5D& dst_point)
{
    Mat P_src = Mat(Vec2d(src_point.x, src_point.y), CV_64FC1);
    // Rotate and translate
    Mat P_dst = R.inv() * (P_src - t);
    // Upside down
    dst_point.x = round(P_dst.at<double>(0, 0));
    dst_point.y = height - 1 - round(P_dst.at<double>(1, 0));
    dst_point.theta= src_point.theta;
    dst_point.length = src_point.length;
    dst_point.alpha = src_point.alpha;
}
