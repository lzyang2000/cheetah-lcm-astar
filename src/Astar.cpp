//
// Created by anxing.
//

#include "Astar.h"

namespace pathplanning{

double jump_height = 0.3;

struct collision { 
    bool is_walkable, is_jump; 
}; 
  

float min(float a,float b ){
    if(a<=b)
        return a;
    else
    {
        return b ;
    }
}

float max(float a,float b ){
    if(a<=b)
        return b;
    else
    {
        return a ;
    }
}

void Astar::InitAstar(Mat& _Map, AstarConfig _config)
{
    Mat Mask;
    InitAstar(_Map, Mask, _config);
}

void Astar::InitAstar(Mat& _Map, Mat& Mask, AstarConfig _config)
{
    // char neighbor8[8][3] = {
    //         {-1, -1, 3}, {-1, 0, 2}, {-1, 1, 1},
    //         {0, -1, 4},            {0, 1, 0},
    //         {1, -1, -3},   {1, 0, -2},  {1, 1, -1},
    // };

    // Map = _Map;
    // config = _config;
    // neighbor = Mat(8, 3, CV_8S, neighbor8).clone();

    char neighbor8[8][2] = {
              {0, 1},{-1, 1},{-1, 0},{-1, -1},
            {0, -1},{1, -1},{1, 0},{1, 1}
    };

    Map = _Map;
    config = _config;
    neighbor = Mat(8, 2, CV_8S, neighbor8).clone();

    MapProcess(Mask);
}

void Astar::PathPlanning(Point_5D _startPoint, Point_5D _targetPoint, vector<Point_5D>& path)
{
    // Get variables
    startPoint = _startPoint;
    targetPoint = _targetPoint;
   
    // Path Planning
    Node* TailNode = FindPath();
    GetPath(TailNode, path);
    Smooth(path);
}

void Astar::DrawPath(Mat& _Map, vector<Point_5D>& path, InputArray Mask, Scalar color,
        int thickness, Scalar maskcolor)
{
    // if(path.empty())
    // {
    //     cout << "Path is empty!" << endl;
    //     return;
    // }
    // _Map.setTo(maskcolor, Mask);
    // for(auto it:path)
    // {
    //     Point drawpoint= Point(it.x,it.y);
    //     rectangle(_Map, drawpoint, drawpoint, color, thickness);
    // }
    imshow("map",Mask);
}

void Astar::MapProcess(Mat& Mask)
{
    int width = Map.cols;
    int height = Map.rows;
    Mat _Map = Map.clone();

    // Transform RGB to gray image
    if(_Map.channels() == 3)
    {
        cvtColor(_Map.clone(), _Map, CV_BGR2GRAY);
    }

    // Binarize
    if(config.OccupyThresh < 0)
    {
        threshold(_Map.clone(), _Map, 0, 255, CV_THRESH_OTSU);
    } else
    {
        threshold(_Map.clone(), _Map, config.OccupyThresh, 255, CV_THRESH_BINARY);
    }

    // Inflate
    Mat src = _Map.clone();
    if(config.InflateRadius > 0)
    {
        Mat se = getStructuringElement(MORPH_ELLIPSE, Size(2 * config.InflateRadius, 2 * config.InflateRadius));
        erode(src, _Map, se);
    }

    // Get mask
    bitwise_xor(src, _Map, Mask);

    // Initial LabelMap
    LabelMap = Mat::zeros(height, width, CV_8UC1);
    for(int y=0;y<height;y++)
    {
        for(int x=0;x<width;x++)
        {
            if(_Map.at<uchar>(y, x) == 0)
            {
                LabelMap.at<uchar>(y, x) = obstacle;
            }
            else
            {
                LabelMap.at<uchar>(y, x) = free;
            }
        }
    }
}

Struct Astar::CheckCollision(Mat _LabelMap, int x, int y, double theta)
{
    Struct collision; 
    collision.is_walkable = true;
    collision.is_jump = false;

    double x_s = (xi-x)*cos(theta)+(y-yi)*sin(theta);
    double y_s = -(xi-x)*sin(theta)+(y-yi)*cos(theta);
    if(_LabelMap.at<uchar>(yi, xi) == obstacle )
    {
        // if( _LabelMap.at<uchar>(yi, xi).height<jump_height){ TODO: get height
        //     collision.is_jump=true;
        // }
        collision.is_walkable=false;

        return collision;
    }
    return collision;
    // return _LabelMap.at<uchar>(yi, xi) == obstacle;
}

void Astar::RobotKinematics(Mat neighbor,Mat _LabelMap,int &x,int &y, double &theta_, int cx,int cy,double ct,int k)
{
    y = cy + neighbor.at<char>(k, 0);
    x = cx + neighbor.at<char>(k, 1);
    // cout<<((double)(neighbor.at<char>(k, 2)-1) )<<endl;
    // theta_ = ((double)neighbor.at<char>(k, 2) * 3.1416/4.0);
    if (k>4){
        k=k-8;
    }
    theta_ = k* 3.1416/4.0;
}

Node* Astar::FindPath()
{
    int width = Map.cols;
    int height = Map.rows;
    Mat _LabelMap = LabelMap.clone();

    // Add startPoint to OpenList
    OpenList.clear();
    OpenList.push_back(new Node(startPoint));
    _LabelMap.at<uchar>(startPoint.y, startPoint.x) = inOpenList;

    while(!OpenList.empty())
    {
//--------------------------------------- Find least cost node ------------------------------------
        Node* CurNode = OpenList[0];
        int index = 0;
        int length = OpenList.size();
        for(int i = 0;i < length;i++)
        {
            if(OpenList[i]->F < CurNode->F)
            {
                CurNode = OpenList[i];
                index = i;
            }
        }
        int curX = CurNode->point.x;
        int curY = CurNode->point.y;
        double curtheta = CurNode->point.theta;
        double curlength = CurNode->point.length;
        OpenList.erase(OpenList.begin() + index);       // Delete CurNode from OpenList
        _LabelMap.at<uchar>(curY, curX) = inCloseList;

//--------------------------------------- Find least cost node end ------------------------------------


        // Determine whether arrive the target point
        if(curX == targetPoint.x && curY == targetPoint.y)
        {
            return CurNode; // Find a valid path
        }

        // Traversal the neighborhood
        for(int k = 0;k < neighbor.rows;k++)
        {
            int y,x;
            double theta_;
            RobotKinematics(neighbor,_LabelMap,x,y,theta_,curX,curY,curtheta,k);
            


            if(x < 0 || x >= width || y < 0 || y >= height)
            {
                continue;
            }
            if(_LabelMap.at<uchar>(y, x) == free || _LabelMap.at<uchar>(y, x) == inOpenList)
            {
//--------------------------------------- Determine walkable------------------------------------
                Struct collision; 
                collision.is_walkable = true;
                collision.is_jump = false;

                collision = CheckCollision( _LabelMap,x,y,theta_);
//--------------------------------- Determine walkable done------------------------------------
                if(!collision.is_walkable && !collision.is_jump)
                {
                    continue;
                }

//--------------------------------------- Define cost------------------------------------

                
                int addG, G, H, F;
                if(abs(x - curX) == 1 && abs(y - curY) == 1)
                {
                    addG = 14;
                }
                else{
                    addG = 10;
                }
                if(collision.is_jump){
                    addG = addG+5; //TODO: jump cost
                }
                G = CurNode->G + addG;
                if(config.Euclidean)
                {
                    int dist2 = (x - targetPoint.x) * (x - targetPoint.x) + (y - targetPoint.y) * (y - targetPoint.y);
                    H = round(10 * sqrt(dist2));
                }
                else
                {
                    H = 10 * (abs(x - targetPoint.x) + abs(y - targetPoint.y));
                }
                F = G + H;

                // Update the G, H, F value of node
                if(_LabelMap.at<uchar>(y, x) == free)
                {
                    Node* node = new Node();
                    // cout<<alpha_ <<endl;
                    node->point = Point_5D(x, y, theta_, length_,alpha_);
                    node->parent = CurNode;
                    node->G = G;
                    node->H = H;
                    node->F = F;
                    OpenList.push_back(node);
                    _LabelMap.at<uchar>(y, x) = inOpenList;
                }
                else // _LabelMap.at<uchar>(y, x) == inOpenList
                {
                    // Find the node
                    Node* node = NULL;
                    int length = OpenList.size();
                    for(int i = 0;i < length;i++)
                    {
                        if(OpenList[i]->point.x ==  x && OpenList[i]->point.y ==  y  )
                        {
                            node = OpenList[i];
                            break;
                        }
                    }
                    if(G < node->G)
                    {
                        node->G = G;
                        node->F = F;
                        node->parent = CurNode;
                    }
                }
//--------------------------------------- Define cost done------------------------------------

            }
        }
    }

    return NULL; // Can not find a valid path
}

void Astar::Smooth(vector<Point_3D>& path){
    for(int i=0;i<path.size();i++){
        int num = 1;
        float low = max(0,i-num);
        float high = min(path.size()-1,i+num);
        float x=0,y=0,theta=0
        for(int j=low;j<=high;j++)
        {
            x += path[j].x;
            y += path[j].y;
            theta += path[j].theta;
        }
        path[i].x = x/(high-low+1);
        path[i].y = y/(high-low+1);
        path[i].theta = theta/(high-low+1);

        // path[i].x = 0;
        // path[i].y = 0;
        // path[i].theta = theta/(high-low+1);
        // path[i].alpha = alpha/(high-low+1);
        // path[i].x = (path[i].x + path[low].x + path[high].x)/3;
        // path[i].y = (path[i].y + path[low].y + path[high].y)/3;
        // path[i].theta = (path[i].theta + path[low].theta + path[high].theta)/3;
        // path[i].alpha = (path[i].alpha + path[low].alpha + path[high].alpha)/3;
    }
}

void Astar::GetPath(Node* TailNode, vector<Point_3D>& path)
{
    PathList.clear();
    path.clear();

    // Save path to PathList
    Node* CurNode = TailNode;
    while(CurNode != NULL)
    {
        PathList.push_back(CurNode);
        CurNode = CurNode->parent;
    }

    // Save path to vector<Point>
    int length = PathList.size();
    for(int i = 0;i < length;i++)
    {
        path.push_back(PathList.back()->point);
        
        PathList.pop_back();
    }
    
}

}
