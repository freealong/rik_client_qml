//
// Created by han on 16-11-17.
//

#include "CPosTable.h"
#include <fstream>

CPosTable::CPosTable() {
    pTable = make_shared< unordered_map<CMyMat, Point> >();
}


bool CPosTable::loadTable() {
    ifstream in(tableDir, ios::in);
    vector<string> strVec(50);
    for(int i=0; i<strVec.size(); i++) {
        getline(in, strVec[i]);
    }
    for(int i=0; i<43; ++i) {
        for(int j=0; j<43; ++j) {
            Mat tempMat(8,8,CV_8UC1);
            for(int ii=0; ii<8; ++ii) {
                for(int jj=0; jj<8; ++jj) {
                    tempMat.at<char>(ii,jj) = strVec[i+ii][j+jj] - '0';
                }
            }
            CMyMat mmat(tempMat);
            if( !pTable->insert(unordered_map<CMyMat, Point>::value_type(mmat, Point(i,j))).second ) {
                cout << tempMat <<endl;
                cout << i <<", "<< j <<endl;
                auto it = pTable->find(tempMat);
                cout << it->second <<endl;
                cout << "Error happens when load table" <<endl;
                in.close();
                return false;
            }
        }
    }
    in.close();
    return true;
}

bool CPosTable::find(const Mat &grid, Point& pt) {
    pt = Point(0,0);
    CMyMat mmat(grid);
    unordered_map<CMyMat, Point>::iterator it = pTable->find(mmat);
    if( it!= pTable->end() ) {
        pt = it->second;
        return true;
    } else {
        cout << "Grid is not found in table" <<endl;
        return false;
    }
}
