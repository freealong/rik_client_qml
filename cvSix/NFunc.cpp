//
// Created by han on 16-11-23.
//

#include "NFunc.h"
#include <sstream>
#include <fstream>
#include "NFindCorners.h"
#include "CPosTable.h"
#include "NImage.h"
#include "NUtils.h"
#include "CCalibrator.h"


namespace NFunc {

	void help() {
	    printf("This is used to find the corners of chessboard or locate the camera\n"
	        "   -h        # Show help.\n"
	        "   -p        # Deal with one picture. An image should be provided.\n"
	        "   -v        # Simple video mode. 'q' for exit and 's' for save.\n"
	        "   -vc       # Video mode with corners. 'q' for exit and 's' for save.\n"
	        "   -vp       # Video mode with processing if 'p' is pressed.\n"
	        "   -all      # Deal with a groups of images. A directory should be provided.\n");
	}

	void showVideo(VideoCapture& video, int picNum) {
	    //-- Deal with pics.
	    char c=' ';
	    //int picNum =10;
	    Mat frame,image;
	    CCalibrator calibrator;
	    while(c!='q') {
	        video >> frame;
	        //calibrator.calibrator(image, frame);
	        if(c=='s') {
	            stringstream ss;
	            ss << "../data/";
	            ss << picNum << ".png";
	            picNum ++;
	            imwrite(ss.str(), frame);
	            cout << "Image Saved: " << ss.str() <<endl;
	        }
	        Point2f center(frame.cols/2.0, frame.rows/2.0);
	        circle(frame, center, 4, Scalar(255,0,0), 4);
	        cv::imshow("video", frame);
	        c = waitKey(1);
	    }
	}

	void showCorners(VideoCapture& video, int picNum) {
	    Mat src;
	    Size pattern_size = Size(8,8);
	    //int picNum =10;
	    char c=' ';
	    while( c!='q' ) {
	        video >> src;
	        if(c=='s') {
	            stringstream ss;
	            ss << "../data/";
	            ss << picNum << ".png";
	            picNum ++;
	            imwrite(ss.str(), src);
	            cout << "Image Saved: " << ss.str() <<endl;
	        }

	        vector<Point2f> out_corners(60*60);
	        int out_corner_count = 0;
	        int prev_sqr_size = 0;
	        double rotAngle = 0;
	        bool found = NFindCorners::findCorners(src, pattern_size, out_corners, out_corner_count, prev_sqr_size, rotAngle);

	        if(found) {
	            for(int i=0; i<out_corner_count; ++i) {
	                circle(src, out_corners[i], 4, Scalar(0,0,255), 2);
                  // cout<<"out_corners[i] = "<<out_corners[i]<<endl;  
	            }
	        } else {
	            cout << "No Corners Found in This Frame!" <<endl;
	        }
			Point2f center(src.cols/2.0, src.rows/2.0);
	        circle(src, center, 4, Scalar(255,0,0), 4);
	        imshow("src",src);
	        c=waitKey(1);
	    }
	}

	void showGrid(VideoCapture& video, int picNum) {
	    Mat src;
	    Size pattern_size = Size(8,8);
	    //int picNum =10;
	    char c=' ';
	    while( c!='q' ) {
	        video >> src;
	        if(c=='s') {
	            stringstream ss;
	            ss << "../data/";
	            ss << picNum << ".png";
	            picNum ++;
	            imwrite(ss.str(), src);
	            cout << "Image Saved: " << ss.str() <<endl;
	        }

	        if( c=='p' ) {
	            Mat grid;
	            vector<Point2f> corners;
	            c = NImage::processPictures(src, corners, grid, "Processed Image");
	        }

	        imshow("src",src);
	        c=waitKey(1);
	    }
	}

	bool dealWithImage(Mat& src,  vector<double>& pos, const string& winName) {
	    static bool firstRun = true;
	    static CPosTable table;
	    static vector<Point3f> xyzPoints;
	    static CCalibrator calibrator;
	    static Size boardSize = Size(8,8);
	    static double gridLength = 20.0;
	    if(firstRun) {
	        table.loadTable();
	        /* Cordinate Defination of board.
	         *  ---> y
	         *  |
	         *  |
	         *  |/ x
	         * NOTICE: The order here must suit for the order of corners 
	         *         and cordinate defination.
	         */
	        for(int i=0;i<boardSize.height+1;++i) {
	            for(int j=0; j<boardSize.width+1; ++j) {
	                Point3f temp(gridLength*(j), gridLength*(i), 0);
	                xyzPoints.push_back(temp);
	            }
	        }
	        firstRun = false;
	    }
	    Mat grid;
	    vector<Point2f> corners;
	    if( NImage::processPictures(src, corners, grid, winName) ) {
	        Point pt;
	        table.find(grid, pt);
	        //cout << grid <<endl;
	        //cout << "The camera is at: " << pt <<endl;

	        Mat srcGray;
	        cvtColor(src, srcGray, COLOR_BGR2GRAY);
	        cornerSubPix(srcGray, corners, Size(11,11), Size(-1,-1), 
	            TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1) );

	        Mat tvec(3,1,CV_64FC1,0);
	        Mat rvec(3,1,CV_64FC1,0);

	        solvePnPRansac(Mat(xyzPoints), Mat(corners), calibrator.getCameraMat(), calibrator.getDistortionCoef(),
	                               rvec, tvec);

	        Mat rot(3,3,CV_64FC1,0);
	        Rodrigues(rvec, rot);
	        Mat Tmat = (Mat_<double>(4,4) << 
	                        rot.at<double>(0,0), rot.at<double>(0,1), rot.at<double>(0,2), tvec.at<double>(0,0),
	                        rot.at<double>(1,0), rot.at<double>(1,1), rot.at<double>(1,2), tvec.at<double>(1,0),
	                        rot.at<double>(2,0), rot.at<double>(2,1), rot.at<double>(2,2), tvec.at<double>(2,0),
	                        0.0, 0.0, 0.0, 1.0);
	        Tmat = Tmat.inv();
	        Mat trans = (Mat_<double>(3,1) << Tmat.at<double>(0,3), Tmat.at<double>(1,3), Tmat.at<double>(2,3));
	        cout << Tmat <<endl;
	        cout << "The position within grid is: " << trans <<endl;
	        pos.clear();
	        pos.push_back(pt.x);
	        pos.push_back(pt.y);
	        pos.push_back(trans.at<double>(0));
	        pos.push_back(trans.at<double>(1));
	        pos.push_back(trans.at<double>(2));
	        pos.push_back((gridLength*(pt.x)) + trans.at<double>(0));
	        pos.push_back((gridLength*(pt.y)) + trans.at<double>(1));
	        pos.push_back(trans.at<double>(2));
	        return true;

	    } else {
	        return false;
	    }
	}

	bool dealWithBatches(const string& imageDir) {
	    fstream out(imageDir+"pos.csv", ios::out|ios::app);
	    vector< string > images;
	    NUtils::getAllFiles(imageDir, images, ".png", true);
	    //NUtils::printVector<string>(images);

	    for(int i=0; i<images.size(); ++i) {
	        cout << endl;
	        cout << "---- Dealing with " << images[i] << "----"<<endl;
	        Mat src = imread(imageDir+images[i], 1);
	        vector<double> pos;
	        stringstream ss;
	        ss << images[i] <<",";
	        if( dealWithImage(src, pos, "src") ) {
	            for(int j=0; j<pos.size()-1; ++j) {
	                ss << pos[j] << ",";
	            }
	            ss << pos[pos.size()-1];
	        } else {
	            ss << "invalid data";
	            //return false;
	        }
	        out << ss.str() <<endl;
	    }

	    out.close();
	    return true;
	}

	bool dealWithRotImage(const Mat& image) {
		Mat src = image.clone();
		const double PI = 3.141592654;
		double angle = 270;
		double _sin = sin(angle*PI/180.0);
		double _cos = cos(angle*PI/180.0);
		int width = src.cols;
		int height = src.rows;
		int rotWidth = int(height*fabs(_sin) + width*fabs(_cos));
		int rotHeight = int(width*fabs(_sin) + height*fabs(_cos));

		cout << "Image Size: "<<height << "*" <<width<<endl;
		cout << "rotImage Size: "<<rotHeight << "*" <<rotWidth<<endl;

		Point center(width/2, height/2);
		Point rotCenter(rotWidth/2, rotHeight/2);

		Mat rot(2,3,CV_64FC1);
		rot = getRotationMatrix2D(center, angle, 1);  //invertAffineTransform

		rot.at<double>(0,2) += (rotWidth-width)/2.0;
		rot.at<double>(1,2) += (rotHeight-height)/2.0;


		Mat dst(Mat::zeros(rotHeight, rotWidth, src.type()));
		warpAffine(src, dst, rot, dst.size());

/*		坐标运算关系
		Point pt(width/3, height/3);
		Point rotPt;
		rotPt.x = rot.at<double>(0,0) * pt.x + rot.at<double >(0,1)*pt.y + rot.at<double>(0,2);
		rotPt.y = rot.at<double>(1,0) * pt.x + rot.at<double >(1,1)*pt.y + rot.at<double>(1,2);
*/

/*		还原方式
		Mat src2(Mat::zeros(height, width, src.type()));
		Mat iRot(2,3,CV_64FC1);
		invertAffineTransform(rot, iRot);
		warpAffine(dst, src2, iRot, src2.size());
*/
		circle(src, center, 4, Scalar(255,0,0), 4);
		circle(dst, rotCenter, 4, Scalar(255,0,0), 4);

		imshow("src", src);
		waitKey(1);

		imshow("dst", dst);
		waitKey(1);

		waitKey(0);
		return true;
	}

}
