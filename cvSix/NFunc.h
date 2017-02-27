//
// Created by han on 16-11-23.
//

#ifndef CVTEST_NFUNC_H
#define CVTEST_NFUNC_H

#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <vector>

namespace NFunc {
	using namespace cv;
	using namespace std;
	void help();
	void showVideo(VideoCapture& video, int picNum=0);
	void showCorners(VideoCapture& video, int picNum=0);
	void showGrid(VideoCapture& video, int picNum=0);
	bool dealWithImage(Mat& src, vector<double>& pos, const string& winName="src");
	bool dealWithBatches(const string& imageDir);
	bool dealWithRotImage(const Mat& image);
}
#endif