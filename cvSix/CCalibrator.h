//
// Created by han on 16-10-20.
//

#ifndef CVTEST_CCALIBRATOR_H
#define CVTEST_CCALIBRATOR_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
using namespace std;

class CCalibrator {
public:
    CCalibrator();

    inline cv::Mat getCameraMat() const {
        return cameraMat;
    }
    inline cv::Mat getDistortionCoef() const {
        return distortionCoef;
    }

    void calibrator(const cv::Mat& src, cv::Mat& dst);

private:
    cv::Mat cameraMat;
    cv::Mat distortionCoef;
    const string configFile="../config/calibrationPara.yml";

    int formatParameter(vector<double>& camera_matrix, vector<double>& distortion_coefficients);
    int formatString(bool &flag, bool &read, const string key_str, string &data_str, string s);
    double str2double(const string &s);
    string& trim(string &s);
    void splitString(const string &s, vector<string> &v, const string &c);


};


#endif //CVTEST_CCALIBRATOR_H
