#include "imSynthesis.h"

using json = nlohmann::json;
using namespace std;

typedef struct{
    float x;
    float y;
    float w;
    float h;
    float theta;
    float score;
    int label;
}detection;


double IOU(vector<double> x_A, vector<double> y_A, vector<double> x_B, vector<double> y_B);

int find_leftmost_point(vector<cv::Point2f> intersectingRegion);

vector<cv::Point2f> sort_points(vector<cv::Point2f> intersectingRegion);

float rbox_iou(detection d1, detection d2);

void imSynthesis::integrate()
{	
    int i, j, k;
    vector<double> x_A, y_A;
    vector<double> x_B, y_B;
    double quadrilateralA, quadrilateralB;
    bool identified;
    bool duplicate;
    double iou;

	ifstream jfile("../json_example/result_2.json");
	json data = json::parse(jfile);
    jfile.close();

    string modelist[2] = {"row", "column"};

    for(int l=0; l<2; ++l){
        string mode = modelist[l];

        for(i=0; i<data[mode].size(); ++i){
            for(k=0;k<4;k++){
                x_A.push_back(data[mode][i]["location"]["points"][k]["x"]);
                y_A.push_back(data[mode][i]["location"]["points"][k]["y"]);
            }
            identified  = false;
            for(j=min(0, i-5); j<min(int(data["normal"].size()), i+5); ++j){
                for(k=0;k<4;k++){
                    x_B.push_back(data["normal"][i]["location"]["points"][k]["x"]);
                    y_B.push_back(data["normal"][i]["location"]["points"][k]["y"]);
                }
                iou = IOU(x_A, x_B, y_A, y_B);

                x_B.clear();
                y_B.clear();

                if(iou > _IoUThreshold){
                    identified = true;
                    break;
                }
            }
            if(identified == false){
                data["normal"].push_back(data[mode][i]);
            }

            x_A.clear();
            y_A.clear();

            printf("\r%s", mode.c_str());
            printf("处理中[%.2lf%%]", i*100.0 / (data[mode].size() - 1));            
        }
    }

    for(i=0; i<data["normal"].size(); ++i){
        for(k=0;k<4;k++){
            x_A.push_back(data["normal"][i]["location"]["points"][k]["x"]);
            y_A.push_back(data["normal"][i]["location"]["points"][k]["y"]);
        }
        identified  = false;
        for(j=i; j<min(int(data["normal"].size()), i+5); ++j){
            for(k=0;k<4;k++){
                x_B.push_back(data["normal"][i]["location"]["points"][k]["x"]);
                y_B.push_back(data["normal"][i]["location"]["points"][k]["y"]);
            }
            iou = IOU(x_A, x_B, y_A, y_B);

            x_B.clear();
            y_B.clear();

            if(iou > _IoUThreshold){
                duplicate = true;
                break;
            }
        }

        x_A.clear();
        y_A.clear();

        if(duplicate == true){
            data["normal"].erase(i);
        }

        printf("\rnormal复制处理中[%.2lf%%]", i*100.0 / (data["normal"].size() - 1));
    }

    cout << "\n" << data["normal"].size() << endl;
}

double ComputePolygonArea(vector<double> x, vector<double> y)
{
    int point_num = x.size();

    if(point_num < 3)return 0.0;
    
    double s = 0;

    for(int i = 0; i < point_num-1; ++i)
    {
        s += x[i] * y[i+1] - y[i] * x[i+1];
    }
    s += x[point_num-1] * y[0] - y[point_num-1] * x[0];
    
    return fabs(s/2.0);
}

double IOU(vector<double> x_A, vector<double> x_B, vector<double> y_A, vector<double> y_B)
{
    detection r1;
    detection r2;

    double iou;

    r1.x = (x_A[0]+x_A[2])/2;
    r1.y = (y_A[0]+y_A[2])/2;
    r1.w = sqrt(pow(x_A[0]-x_A[1],2)+pow(y_A[0]-y_A[1],2));
    r1.h = sqrt(pow(x_A[2]-x_A[1],2)+pow(y_A[2]-y_A[1],2));
    r1.theta = atan((y_A[2]-y_A[0])/(x_A[2]-x_A[0]))-atan((y_A[1]-y_A[0])/(x_A[1]-x_A[0]));

    r2.x = (x_B[0]+x_B[2])/2;
    r2.y = (y_B[0]+y_B[2])/2;
    r2.w = sqrt(pow(x_B[0]-x_B[1],2)+pow(y_B[0]-y_B[1],2));
    r2.h = sqrt(pow(x_B[2]-x_B[1],2)+pow(y_B[2]-y_B[1],2));
    r2.theta = atan((y_B[2]-y_B[0])/(x_B[2]-x_B[0]))-atan((y_B[1]-y_B[0])/(x_B[1]-x_B[0]));

    iou = rbox_iou(r1, r2);

    return iou;
}

float rbox_iou(detection d1, detection d2)
{
    float inter_area;
    float area_r1 = d1.w * d1.h;
    float area_r2 = d2.w * d2.h;
    cv::RotatedRect rect1;
    rect1.center = cv::Point2f(d1.x, d1.y);
    rect1.size = cv::Size(d1.w, d1.h);
    rect1.angle = d1.theta;
    cv::RotatedRect rect2;
    rect2.center = cv::Point2f(d2.x, d2.y);
    rect2.size = cv::Size(d2.w, d2.h);
    rect2.angle = d2.theta;
 
    vector<cv::Point2f> intersectingRegion;
    cv::rotatedRectangleIntersection(rect1, rect2, intersectingRegion);
    
    if (intersectingRegion.empty())
    {
        inter_area = 0;
    }
    else
    {
        vector<cv::Point2f> sort_intersectingRegion = sort_points(intersectingRegion);
        inter_area = cv::contourArea(sort_intersectingRegion);
    }
    return inter_area / (area_r1 + area_r2 - inter_area + 0.00000001);
}


int find_leftmost_point(vector<cv::Point2f> intersectingRegion)
{
    int index = 0;
    float tmp = intersectingRegion[0].x;
    for(int i=1; i<intersectingRegion.size(); i++)
    {
        if(intersectingRegion[i].x < tmp)
        {
            tmp = intersectingRegion[i].x;
            index = i;
        }
    }
    return index;
}



vector<cv::Point2f> sort_points(vector<cv::Point2f> intersectingRegion)
{
    vector<cv::Point2f> sort_intersectingRegion;
    int leftmost_index = find_leftmost_point(intersectingRegion);
 
    vector<float> arctan;
    for(int i=0; i<intersectingRegion.size(); i++)
    {
        arctan.push_back(atan2(intersectingRegion[i].y - intersectingRegion[leftmost_index].y, intersectingRegion[i].x - intersectingRegion[leftmost_index].x));
    }
 
    vector<int> index;
    for(int i=0; i<arctan.size(); i++)
    {
        index.push_back(i);
    }
    
    sort(index.begin(), index.end(), [&](const int& a, const int& b) {return (arctan[a] < arctan[b]);});
    
    for(int i=0; i<index.size(); i++)
    {
        sort_intersectingRegion.push_back(intersectingRegion[index[i]]);
    }
    return sort_intersectingRegion;
}