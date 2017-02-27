//
// Created by han on 16-11-23.
//

#ifndef CVTEST_NIMAGE_H
#define CVTEST_NIMAGE_H

#include <cv.h>
#include <vector>

namespace NImage {
	using namespace cv;
	using namespace std;

	bool validPointsAroundCenter(vector<Point2f>& pointsRow, Point2f& center, bool sortByX, const Size& pattern_size, int margin);
	bool isNearEdge(const Rect& rect, const Mat& src);

	void adjustCornersUp(const vector<Point2f>& src, vector<Point2f>& dst, const Mat& srcGrid, Mat& grid);
	void adjustCornersDown(const vector<Point2f>& src, vector<Point2f>& dst, const Mat& srcGrid, Mat& grid);
	void adjustCornersLeft(const vector<Point2f>& src, vector<Point2f>& dst, const Mat& srcGrid, Mat& grid);
	void adjustCornersRight(const vector<Point2f>& src, vector<Point2f>& dst, const Mat& srcGrid, Mat& grid);

	bool processPictures(const Mat& raw, vector<Point2f>& corners, Mat& grid, string windowName);
}

#endif