#pragma once
#include <string>
#include <stdio.h>  
#include <math.h> 
#include "iostream"
#include <fstream>
#include <nlohmann/json.hpp>
#include<opencv2/opencv.hpp>
#include <vector>
#include <iostream>
#include <thread>

using json = nlohmann::json;
using namespace std;

class imSynthesis
{
private:
    string _imageName;
    string _imageSetName;
    string _imageFilePath;
    double _IoUThreshold;

public:
    imSynthesis(string Name, string imageSetName, double IoUThreshold)
    {
        this->_imageName = Name;
        this->_imageSetName = imageSetName;
        this->_imageFilePath = "../data/Cropped Data/" + _imageSetName + "/";
        this->_IoUThreshold = IoUThreshold;
    }
    void integrate();
};