//
// Created by han on 16-11-17.
//

#ifndef CVTEST_CPOSTABLE_H
#define CVTEST_CPOSTABLE_H

#include <memory>
#include <unordered_map>
#include <cv.h>
using namespace std;
using namespace cv;

struct CMyMat {
    CMyMat(){}
    CMyMat(const Mat& m):mat(m){}
    Mat mat;

    friend bool operator==(const CMyMat& m1, const CMyMat& m2) {
        Mat diff = (m1.mat!=m2.mat);
        return countNonZero(diff)==0;
    }
};



namespace std {
    template <>
    struct hash<CMyMat> {
        size_t operator()(const CMyMat& mmat) const {
            size_t seed =0;
            for(int i=0; i<mmat.mat.cols; ++i) {
                int temp = 0;
                for(int j=0; j<mmat.mat.rows; ++j) {
                    temp = (temp<<1) + mmat.mat.at<char>(i,j);
                }
                size_t tempHash = hash<int>{}(temp);
                seed = ( seed ) ^ (  tempHash<<1);
            }
            return seed;
        }
    };



}


class CPosTable {
private:
    shared_ptr<unordered_map<CMyMat, Point>> pTable;
    const string tableDir = "../config/code.txt";
public:
    CPosTable();

    bool loadTable();
    bool find(const Mat& grid, Point& pt);
};


#endif //CVTEST_CPOSTABLE_H
