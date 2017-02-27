//
// Created by han on 16-11-23.
//

#include "NImage.h"
#include "NUtils.h"
#include "NFindCorners.h"

namespace NImage {

	using namespace NUtils;

	bool validPointsAroundCenter(vector<Point2f>& pointsRow, Point2f& center, bool sortByX, const Size& pattern_size, int margin) {
	    // (pattern_size+1)*(pattern_size+1) points grid is needed.
	    if( pointsRow.size() < pattern_size.width +1 ) {
	        return false;
	    }
	    if(sortByX) {
	        sort(pointsRow.begin(), pointsRow.end(), compPointX);
	        for(auto it = pointsRow.begin()+1; it!=pointsRow.end()-1; ) {
	            if( fabs(it->y - (it-1)->y) > margin && fabs(it->y - (it+1)->y) > margin ) {
	                it = pointsRow.erase(it);
	            } else {
	                it++;
	            }
	        }
	    } 
	    else {
	        sort(pointsRow.begin(), pointsRow.end(), compPointY);
	        for(auto it = pointsRow.begin()+1; it!=pointsRow.end()-1; ) {
	            if( fabs(it->x - (it-1)->x) > margin && fabs(it->x - (it+1)->x) > margin ) {
	                it = pointsRow.erase(it);
	            } else {
	                it++;
	            }
	        }
	    }
	    const int eachSideCount = pattern_size.width / 2;
	    bool found = false;
	    vector<Point2f> result;
	    for(int i=0; i< pointsRow.size(); ++i) {
	        if(pointsRow[i] == center) {
	            if(i<eachSideCount) {
	                return false;
	            } else {
	                if(pointsRow.size()-1-i >=4) {
	                    //If there are 4 points before and after pivot in pivotCol.
	                    found = true;
	                    for(int j=-eachSideCount; j<=eachSideCount; ++j) {
	                        result.push_back( pointsRow[i+j] );
	                    }
	                } else {
	                    return false;
	                }
	            }
	            break;
	        }
	    }
	    assert(found);
	    pointsRow.clear();
	    pointsRow = result;
	    return true;
	}

	bool isNearEdge(const Rect& rect, const Mat& src) {
	    int maxX = src.cols-1;
	    int maxY = src.rows-1;
	    Point tl = rect.tl();
	    Point br = rect.br();
	    int margin = 7;
	    if( tl.x < margin || tl.x > maxX-margin || tl.y < margin || tl.y > maxY-margin )
	        return true;
	    if( br.x < margin || br.x > maxX-margin || br.y < margin || br.y > maxY-margin )
	        return true;
	    return false;
	}



	struct SVote {
		int up;
		int down;
		int left;
		int right;
		SVote():up(0), down(0), left(0), right(0) {}

		// Get the direction which has the max votes.
		char maxVote() {
			int temp = max(max(up, down), max(left, right));
			char direction = 'n';  // n for not clear.
			if(temp==up) {
				if(up > down + left + right) {
					direction = 'u';
				}
			} else if(temp==down) {
				if(down > up + left + right) {
					direction = 'd';
				}
			} else if(temp==left) {
				if(left > up + down + right) {
					direction = 'l';
				}
			} else {
				if(right > up + down + left) {
					direction = 'r';
				}
			}
			return direction;
		}
	};


	void adjustCornersUp(const vector<Point2f>& src, vector<Point2f>& dst, const Mat& srcGrid, Mat& grid) {
		dst = src;
		grid = srcGrid;
	}
	void adjustCornersDown(const vector<Point2f>& src, vector<Point2f>& dst, const Mat& srcGrid, Mat& grid) {
		dst.clear();
		int height = srcGrid.rows;
		int width = srcGrid.cols;
		// adjust corners.
		assert(src.size() == (height+1)*(width+1) );
		for(int i=src.size()-1; i>=0; --i) {
			dst.push_back(src[i]);
		}
		// adjust grid.
		grid = Mat::zeros(srcGrid.size(), CV_8UC1);
		for(int i=0; i<height; ++i) {
			for(int j=0; j<width; ++j) {
				grid.at<char>(i,j) = srcGrid.at<char>(height-1-i, width-1-j);
			}
		}

	}
	void adjustCornersLeft(const vector<Point2f>& src, vector<Point2f>& dst, const Mat& srcGrid, Mat& grid) {
		dst.clear();
		int height = srcGrid.rows;
		int width = srcGrid.cols;
		assert(src.size() == (height+1)*(width+1) );
		grid = Mat::zeros(srcGrid.size(), CV_8UC1);

		// adjust corners.
		for(int i=height; i>=0; --i) {
			for(int j=0; j<width+1; ++j) {
				dst.push_back(src[i+j*(height+1)]);
			}
		}

		//adjust grid.
		for(int i=0; i<height; ++i) {
			for(int j=0; j<width; ++j) {
				grid.at<char>(i,j) = srcGrid.at<char>(height-1-j,i);
			}
		}

	}
	void adjustCornersRight(const vector<Point2f>& src, vector<Point2f>& dst, const Mat& srcGrid, Mat& grid) {
		dst.clear();
		int height = srcGrid.rows;
		int width = srcGrid.cols;
		assert(src.size() == (height+1)*(width+1) );
		grid = Mat::zeros(srcGrid.size(), CV_8UC1);

		// adjust corners.
		for(int i=0; i<height+1; ++i) {
			for(int j=width; j>=0; --j) {
				dst.push_back(src[i+j*(height+1)]);
			}
		}

		//adjust grid.
		for(int i=0; i<height; ++i) {
			for(int j=0; j<width; ++j) {
				grid.at<char>(i,j) = srcGrid.at<char>(j,width-1-i);
			}
		}
	}

	/*
	 * Func: Process a picture. Find the center grid with size pattern_size.
	 *
	 * Params:
	 *      raw: picture.
	 *      grid: the grid if it is found.
	 *      windowName: show the picture with this name.
	 * Returns:
	 *      whether the gird is found.
	 */
	bool processPictures(const Mat& raw, vector<Point2f>& corners, Mat& grid, string windowName) {
	    
	    //#define DEBUG_SHOW_IMAGES
	    //#define DEBUG_EACH_PICS

	    Mat src = raw.clone();
	    Mat rotSrc, rotRaw;  // raw mat and working mat after rotaion.
	    //-- All corners.
	    vector<Point2f> out_corners(60*60);
	    int out_corner_count = 0;
	    int prev_sqr_size = 0;
	    //-- Image centers.
	    Point2f center(src.cols/2.0, src.rows/2.0);
	    
	    //-- 8*8 grids.
	    Size pattern_size = Size(8,8);
	    Point2f pivot;    //The closest point to center.
	    vector<Point2f> pivotRow; //The closest column to center.
	    Mat numGrid(pattern_size, CV_8UC1);

	    double rotAngle=0;  // Calculate rotation angle from grid edge.
	    double addtionAngle=0; // Calculate rotation angle from number direction.
		
	    bool found = NFindCorners::findCorners(src, pattern_size, out_corners, out_corner_count, prev_sqr_size, rotAngle);
      cout<<"NFindCorners::findCorners -> "<<found<<endl;
      if(found) {
        Mat rotMatrix(2,3,CV_64FC1);
        rotMatrix = rotateImage(src, rotRaw, rotAngle);
        rotSrc = rotRaw.clone();

        // Corners at rotated image.
        vector<Point2f> rot_corners;
        //Show corners and find the pivot.
        double minDis = 666;
        for(int i=0; i<out_corner_count; ++i) {
          circle(src, out_corners[i], 2, Scalar(0,0,255), 2);
          Point2f temp = rotatePoint(out_corners[i], rotMatrix);
          rot_corners.push_back(temp);
          circle(rotSrc, temp, 2, Scalar(0,255,0), 2);

          double tempDis = manhattanDis(center, out_corners[i]);
          if( tempDis < minDis ) {
            minDis = tempDis;
            pivot = out_corners[i];
          }
        }


        circle(src, pivot, 4, Scalar(0,255,0),4);
        circle(src, center, 4, Scalar(255,0,0), 4);

        pivot = rotatePoint(pivot, rotMatrix);
        center = rotatePoint(center, rotMatrix);
        circle(rotSrc, pivot, 4, Scalar(0,255,0),4);
        circle(rotSrc, center, 4, Scalar(255,0,0), 4);

        //Find the pivot column.
        const double distanceMargin = prev_sqr_size/2.0;
        for(int i=0; i<out_corner_count; ++i) {
          if(fabs(rot_corners[i].y - pivot.y) < distanceMargin && 
             fabs(rot_corners[i].x - pivot.x) < 15.0* distanceMargin) {
            pivotRow.push_back(rot_corners[i]);
            circle(rotSrc, rot_corners[i], 3, Scalar(0,0,255), 3);
          }
        }


        //cout << distanceMargin <<endl;
        //cout << pivotRow[0].x - pivotRow[1].x <<endl;
        //Get 9 points in center column.
        if( !validPointsAroundCenter(pivotRow, pivot, true, pattern_size, distanceMargin) ) {

          cerr << "No 9 points at pivotRow" <<endl;
#ifdef DEBUG_SHOW_IMAGES
          imshow("PivotRow Error", rotSrc);
          waitKey(0);
#endif
          return false;
        }

        vector<vector<Point2f>> groups(pattern_size.height+1);
        for(int i=0; i<pivotRow.size(); ++i ){
          groups[i].push_back(pivotRow[i]);
          // if(i>0) {
          // 	cout << pivotRow[i].x - pivotRow[i-1].x <<endl;
          // }
        }
        const int cycleTime = pattern_size.height/2;
        bool lostCorners = false;
        vector<Point2f> preVectUp = pivotRow;   
        vector<Point2f> preVectDown = pivotRow;
        float preDistUp = pivot.y - distanceMargin;
        float preDistDown = pivot.y + distanceMargin;
        double yMarginCoff = 3.75;
        for(int i=0; i<cycleTime; ++i) {
          int countTempUp = 0;
          int countTempDown = 0;
          float tempDistUp = 0;
          float tempDistDown = 6666;
          vector<Point2f> tempUp(pattern_size.width+1, Point2f(-1,-1));
          vector<Point2f> tempDown(pattern_size.width+1, Point2f(-1,-1));

          for(int j=0; j<out_corner_count; ++j) {
            if( rot_corners[j].y < preDistUp ){
              for(int k=0; k<pattern_size.width+1; ++k) {
                if( fabs(rot_corners[j].x - preVectUp[k].x)<distanceMargin 
                    && between(rot_corners[j].y, preVectUp[k].y-yMarginCoff*distanceMargin, preVectUp[k].y-distanceMargin) ) {
                  tempUp[k] = rot_corners[j];
                  if(rot_corners[j].y > tempDistUp) {
                    tempDistUp = rot_corners[j].y;
                  }
                  countTempUp++;
                  groups[k].push_back(rot_corners[j]);
                  break;
                }
              }
            } else if( rot_corners[j].y > preDistDown) {
              for(int k=0; k<pattern_size.width+1; ++k) {
                if( fabs(rot_corners[j].x - preVectDown[k].x)<distanceMargin 
                    && between(rot_corners[j].y, preVectDown[k].y+distanceMargin, preVectDown[k].y+yMarginCoff*distanceMargin) ) {
                  tempDown[k] = rot_corners[j];
                  if(rot_corners[j].y < tempDistDown) {
                    tempDistDown = rot_corners[j].y;
                  }
                  countTempDown++;
                  groups[k].push_back(rot_corners[j]);
                  break;
                }
              }
            }
          }

          if( countTempDown!=pattern_size.width+1 || countTempUp!=pattern_size.width+1 ) {
            break;
          }
          preDistUp = tempDistUp;
          preDistDown = tempDistDown;
          preVectUp = tempUp;
          preVectDown = tempDown;

        }

        RNG rng(0xFFFFFFFF);
        bool allCornersFound = true;
        for(int i=0; i<pattern_size.width+1; ++i) {
          int color = (unsigned) rng;
          if(!validPointsAroundCenter(groups[i], pivotRow[i], false, pattern_size, distanceMargin)) {
            cout << "No 9 points at column " << i <<endl;
            allCornersFound = false;
          }
          for(int j=0; j< groups[i].size(); ++j) {
            circle(rotSrc, groups[i][j], 7, Scalar(color&255, (color>>8)&255, (color>>16)&255), 2);
          }
        }
        if(! allCornersFound) {
          cerr << "Not all 9*9 points found!" <<endl;
#ifdef DEBUG_SHOW_IMAGES
          imshow("Corners Error", rotSrc);
          waitKey(0);
#endif
          return false;
        }
	        
        //Get pics of numbers in grid.
        SVote vote;
        for(int i=0; i<pattern_size.width; ++i) {
          for(int j=0; j<pattern_size.height; ++j) {
            float minX = min(groups[i][j].x, groups[i][j+1].x);
            float maxX = max(groups[i+1][j].x, groups[i+1][j+1].x);
            float minY = min(groups[i][j].y, groups[i+1][j].y);
            float maxY = max(groups[i][j+1].y, groups[i+1][j+1].y);
            Mat num_raw = rotRaw(Range(minY, maxY), Range(minX, maxX)).clone();

            //Find contours near the` picture center.
            Mat num(num_raw.size(), CV_8UC1);
            cvtColor(num_raw, num, CV_BGR2GRAY);
            threshold(num, num, 140, 255, THRESH_BINARY); //150//160

            vector<vector<Point>> contours;
            vector<Vec4i> hierarchy;
            findContours(num, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
            vector<vector<Point>> contours_poly( contours.size());
            vector<Rect> boundRect(contours.size());
            for(int k=0; k<contours.size(); k++) {
              approxPolyDP(Mat(contours[k]), contours_poly[k], 3, true);
              boundRect[k] = boundingRect(Mat(contours_poly[k]));
            }
	                
#ifdef DEBUG_EACH_PICS
            Mat drawing( Mat::zeros(num.size(), CV_8UC3 ));
#endif

            vector<Rect> validBoundRect;
            for( int k = 0; k< contours.size(); k++ )
              {
#ifdef DEBUG_EACH_PICS
                Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
                drawContours( drawing, contours, k, color, 1, 8, vector<Vec4i>(), 0, Point() );
                rectangle( drawing, boundRect[k].tl(), boundRect[k].br(), color, 1, 8, 0 );
#endif
	                	
                if( !isNearEdge(boundRect[k], num) && boundRect[k].area() > 100 ) {
	                        
                  validBoundRect.push_back(boundRect[k]);
                }
              }

            //-- If the coding units are 0 and blank.
            if( validBoundRect.size() == 0 ) {
              numGrid.at<char>(j,i) = 8;

            } else if( validBoundRect.size() ==1 ) {
              numGrid.at<char>(j,i) = 0;
              // TODO: Vote for direction here.
              int numHeight = num.rows;
              int numWidth = num.cols;
              Point tl = validBoundRect[0].tl();
              Point br = validBoundRect[0].br();
              // VOTE: up and down
              if( tl.x < 0.5*numWidth && br.x > 0.5*numWidth && 
                  between((0.5*numWidth-tl.x)/(br.x-0.5*numWidth), 0.5, 2) ){
                if( tl.y + br.y < numHeight ) {
                  vote.up++;
                } else {
                  vote.down++;
                }
              } else if( tl.y<0.5*numHeight && br.y>0.5*numHeight &&
                         between((0.5*numHeight-tl.y)/(br.y-0.5*numHeight), 0.5, 2) ) {
                if( tl.x + br.x < numWidth ) {
                  vote.left++;
                } else {
                  vote.right++;
                }

              } else {
                cout << "Direction judgement failed at " << i << j <<endl;
              }

            } else {

              cout << "Number is not recognised at " << i << j <<endl;
              //return false;
            }

#ifdef DEBUG_EACH_PICS
            stringstream ss;
            ss << i << j;
            stringstream ssNum;
            ssNum << validBoundRect.size();
            putText(drawing, ssNum.str(), Point(10,10), FONT_HERSHEY_DUPLEX, 0.5, Scalar(255));
            namedWindow(ss.str(), CV_WINDOW_FREERATIO);
            imshow(ss.str(), drawing);
            waitKey(1);
            //imshow(ss.str()+"num", num);
            //waitKey(1);
#endif
	                
          }   // for loop of cols
        }   //for loop of rows

        // Recover corners.
        //cout << numGrid <<endl;
        Mat tempGrid = numGrid;
        vector<Point2f> tempCorners;

        Mat invRotMatrix(2,3,CV_64FC1);
        invertAffineTransform(rotMatrix, invRotMatrix);
        for(int i=0; i<groups.size(); ++i) {
          for(int j=0; j<groups[i].size(); ++j) {
            Point2f pt = groups[i][j];
            Point2f rotPt;
            rotPt.x = invRotMatrix.at<double>(0,0) * pt.x + invRotMatrix.at<double >(0,1)*pt.y + invRotMatrix.at<double>(0,2);
            rotPt.y = invRotMatrix.at<double>(1,0) * pt.x + invRotMatrix.at<double >(1,1)*pt.y + invRotMatrix.at<double>(1,2);
            tempCorners.push_back(rotPt);
          }
        }

        switch(vote.maxVote()) {
        case 'u':
          adjustCornersUp(tempCorners, corners, tempGrid, grid);
          break;
        case 'd':
          adjustCornersDown(tempCorners, corners, tempGrid, grid);
          break;
        case 'l':
          adjustCornersLeft(tempCorners, corners, tempGrid, grid);
          break;
        case 'r':
          adjustCornersRight(tempCorners, corners, tempGrid, grid);
          break;
        case 'n':
          cerr << "Direction is not clear!" <<endl;
          cout << "up: " <<vote.up <<endl;
          cout << "down: " <<vote.down <<endl;
          cout << "left: " <<vote.left <<endl;
          cout << "right: " <<vote.right << endl;
          break;
        default:
          cerr << "Error happends during voting for direction!" <<endl;
          break;
        }
	        
        cout << grid << endl;

#ifdef DEBUG_SHOW_IMAGES
        // Show final result.
        for(int i=0; i<pattern_size.width+1; ++i) {
          int color = (unsigned) rng;
          for(int j=0; j< pattern_size.width+1; ++j) {
            stringstream ssNum;
            ssNum<<j+i*(pattern_size.width+1);
            circle(src, corners[j+i*(pattern_size.width+1)], 7, Scalar(color&255, (color>>8)&255, (color>>16)&255), 2);
            putText(src, ssNum.str(), corners[j+i*(pattern_size.width+1)], FONT_HERSHEY_DUPLEX, 0.5, Scalar(255));
          }
        }
	        
        imshow(windowName+"_result", src);
        waitKey(0);
#endif

      } else {
        cout << "No corner found!" <<endl;
        return false;
      }
	    
	    #ifdef DEBUG_SHOW_IMAGES
	    imshow(windowName, rotSrc);
	    waitKey(0);
	    #endif

	    return true;
	}
}
