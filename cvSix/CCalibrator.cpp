//
// Created by han on 16-10-20.
//

#include "CCalibrator.h"
#include <fstream>
#include <iostream>
#include <cassert>
using namespace cv;
CCalibrator::CCalibrator(): cameraMat(3,3,CV_64FC1), distortionCoef(1, 5, CV_64FC1) {
    vector<double> a, b;
    formatParameter(a, b);
    assert(a.size()==9);
    assert(b.size()==5);

    for(int i = 0; i < 3; ++i)
    {
        for(int j = 0; j < 3; ++j)
        {
            cameraMat.at<double>(i, j) = a[3 * i + j];
        }
    }
    for(int i = 0; i < 5; ++i)
    {
        distortionCoef.at<double>(0, i) = b[i];
    }

}

void CCalibrator::calibrator(const cv::Mat& src, cv::Mat& dst) {
    cv::undistort(src, dst, cameraMat, distortionCoef);
}

int CCalibrator::formatParameter(vector<double> &camera_matrix, vector<double> &distortion_coefficients) {
    ifstream data(configFile);
    assert(data.is_open());
    string s;
    string camera_matrix_data = "";
    string distortion_coefficients_data = "";
    bool camera_matrix_flag = false;
    bool distortion_coefficients_flag = false;
    bool read = false;
    while(getline(data, s))
    {
        formatString(camera_matrix_flag, read, "camera_matrix", camera_matrix_data, s);
        formatString(distortion_coefficients_flag, read, "distortion_coefficients", distortion_coefficients_data, s);
    }
    vector<string> format_camera_matrix;
    splitString(camera_matrix_data, format_camera_matrix, ",");
    for(int i = 0; i < format_camera_matrix.size(); ++i)
    {
        camera_matrix.push_back(str2double(trim(format_camera_matrix[i])));
    }
    vector<string> format_distortion_coefficients;
    splitString(distortion_coefficients_data, format_distortion_coefficients, ",");
    for(int i = 0; i < format_distortion_coefficients.size(); ++i)
    {
        distortion_coefficients.push_back(str2double(trim(format_distortion_coefficients[i])));
    }
    data.close();
    return 0;
}

int CCalibrator::formatString(bool &flag, bool &read, const string key_str, string &data_str, string s) {
    if(s.find(key_str) != string::npos)
        flag = true;
    if(read && flag)
    {
        string::size_type idx_end = s.find("]");
        if(idx_end != string::npos)
        {
            data_str += s.substr(0, idx_end);
            read = false;
            flag = false;
        }
        else
        {
            data_str += s;
        }
    }
    else if(s.find("data") != string::npos && flag)
    {
        read = true;
        string::size_type idx_begin = s.find("[");
        string::size_type idx_end = s.find("]");
        if(idx_end != string::npos)
        {
            data_str += s.substr(idx_begin + 1, idx_end);
            read = false;
            flag = false;
        }
        else
        {
            data_str += s.substr(idx_begin + 1, -1);
        }
    }
    return 0;
}

double CCalibrator::str2double(const string &s) {
    double num;
    stringstream ss;
    ss << s;
    ss >> num;
    return num;
}

string &CCalibrator::trim(string &s) {
    if (s.empty())
    {
        return s;
    }
    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}

void CCalibrator::splitString(const string &s, vector<string> &v, const string &c) {
    string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while(string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2-pos1));
        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if(pos1 != s.length())
        v.push_back(s.substr(pos1));
}

//const string CCalibrator::configFile = "../config/calibrationPara.yml";
