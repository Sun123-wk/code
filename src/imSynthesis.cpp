#include "imSynthesis.h"

using json = nlohmann::json;
using namespace std;

double ComputePolygonArea(vector<double> x, vector<double> y);

double IOU(vector<double> x_A, vector<double> y_A, vector<double> x_B, vector<double> y_B);

void imSynthesis::integrate()
{	
    int i, j, k;
    vector<double> x_A, y_A;
    vector<double> x_B, y_B;
    double quadrilateralA, quadrilateralB;
    bool identified;
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
            for(j=0; j<data["normal"].size(); ++j){
                for(k=0;k<4;k++){
                    x_B.push_back(data["normal"][i]["location"]["points"][k]["x"]);
                    y_B.push_back(data["normal"][i]["location"]["points"][k]["y"]);
                }
                iou = IOU(x_A, x_B, y_A, y_B);

                if(iou > _IoUThreshold){
                    identified = true;
                    break;
                }
            }
            if(identified == false){
                data["normal"].push_back(data[mode][i]);
            }
        }

    }

    //cout << data[mode][1]["location"]["points"][1]["x"] << endl;

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
    vector<double> x_sum;
    vector<double> y_sum;

    return 0.0;

}