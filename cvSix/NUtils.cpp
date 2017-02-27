//
// Created by han on 16-11-23.
//

#include "NUtils.h"
#include <dirent.h>

namespace NUtils {
	
	double manhattanDis(Point2f p1, Point2f p2) {
    	return fabs(p1.x - p2.x) + fabs(p1.y - p2.y);
	}

	bool compPointX(const Point2f& p1, const Point2f& p2) {
	    return p1.x < p2.x;
	}

	bool compPointY(const Point2f& p1, const Point2f& p2) {
	    return p1.y < p2.y;
	}

	bool between(float x, float minVal, float maxVal) {
	    return (x>minVal) && (x<maxVal);
	}

	bool compFileName(const string& f1, const string& f2) {
		stringstream ss1(f1), ss2(f2);
		char delim = '.';
		vector<string> items1, items2;
		string item;
		while(getline(ss1, item, delim)) {
			items1.push_back(item);
		}
		while(getline(ss2, item, delim)) {
			items2.push_back(item);
		}
		assert(items1.size() == 2);
		assert(items2.size() == 2);

		return stoi(items1[0]) < stoi(items2[0]);

	}

	bool getAllFiles(const string& fileDir, vector<string>& files, const string& extension, bool sortFlag) {
		struct dirent *ptr;
		DIR *dir;
		dir = opendir(fileDir.c_str());
		const int extLength = extension.size();
		while( (ptr=readdir(dir))!= NULL ) {
			if( ptr->d_name[0] == '.' ) {
				continue;
			}
			int tempLength = 0;
			while( ptr->d_name[tempLength]!= '\0' ) {
				tempLength++;
			}
			if( tempLength<extLength ) continue;
			if( strcmp(ptr->d_name+tempLength-extLength, extension.c_str()) == 0 ) {
				files.push_back(ptr->d_name);
			}
		}
		closedir(dir);
		if( files.empty() ) {
			cerr<< "Open Directory Error: No files with extension " << extension << " found!" <<endl;
			return false;
		}

		if(sortFlag) {
			// A specific function should be provided to sort files.
			sort(files.begin(), files.end(), compFileName);
		}
		return true;
	}

	// Calculate the rotation matrix and rotated dst image 
	// based on src image and rotation angle.
	Mat rotateImage(const Mat& src, Mat& dst, const double& angle, bool autoSize) {
		const double angleInDeg = toDegree(angle);
		cout << "Rotation Angle is: "<<angleInDeg <<endl;
		double _sin = sin(angle);
		double _cos = cos(angle);
		int width = src.cols;
		int height = src.rows;
		
		Point center(width/2, height/2);
		Mat rot(2,3,CV_64FC1);
		rot = getRotationMatrix2D(center, angleInDeg, 1);
		if(autoSize) {
			int rotWidth = int(height*fabs(_sin) + width*fabs(_cos));
			int rotHeight = int(width*fabs(_sin) + height*fabs(_cos));
			
			rot.at<double>(0,2) += (rotWidth-width)/2.0;
			rot.at<double>(1,2) += (rotHeight-height)/2.0;

			dst = Mat::zeros(rotHeight, rotWidth, src.type());
			warpAffine(src, dst, rot, dst.size());
			return rot;
		} else {
			dst = Mat::zeros(height, width, src.type());
			warpAffine(src, dst, rot, dst.size());
			return rot;
		}
		
	}

	Point2f rotatePoint(const Point& pt, const Mat& rot) {
		Point2f rotPt;
		rotPt.x = rot.at<double>(0,0) * pt.x + rot.at<double >(0,1)*pt.y + rot.at<double>(0,2);
		rotPt.y = rot.at<double>(1,0) * pt.x + rot.at<double >(1,1)*pt.y + rot.at<double>(1,2);
		return rotPt;
	}

	double fromDegree(double angle) {
		return angle*3.141592654/180.0;
	}

	double toDegree(double angle) {
		return angle*180.0/3.141592654;
	}



}	// namespace NUtils
