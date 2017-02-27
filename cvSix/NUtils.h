//
// Created by han on 16-11-23.
//

#ifndef CVTEST_NUTILS_H
#define CVTEST_NUTILS_H
#include <cv.h>
#include <vector>
#include <string>
using namespace cv;
using namespace std;

namespace NUtils {
	double manhattanDis(Point2f p1, Point2f p2);
	bool compPointX(const Point2f& p1, const Point2f& p2);
	bool compPointY(const Point2f& p1, const Point2f& p2);
	bool between(float x, float minVal, float maxVal);
	bool getAllFiles(const string& fileDir, vector<string>& files, const string& extension, bool sortFlag=false);

	Mat rotateImage(const Mat& src, Mat& dst, const double& angle, bool autoSize=true);
	Point2f rotatePoint(const Point& pt, const Mat& rot);

	double fromDegree(double angle);
	double toDegree(double angle);

	template<typename M>
	void printVector(const vector<M>& vec) {
		for(int i=0; i<vec.size(); ++i) {
			cout<< vec[i] <<endl;
		}
	}

}



#endif