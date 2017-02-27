//
// Created by han on 16-11-15.
//

#ifndef CVTEST_NFINDCORNERS_H
#define CVTEST_NFINDCORNERS_H

#include <cv.h>
#include <highgui.h>

using namespace std;
using namespace cv;

namespace NFindCorners {

#define MAX_PATTERN Size(50,50)

    bool findCorners(Mat src, Size pattern_size, vector<Point2f>& out_corners, int& out_corner_count, int& prev_sqr_size, double& rotAngle);
}

#endif //CVTEST_NFINDCORNERS_H
