//
// Created by han on 16-10-23.
//

#ifndef CVTEST_CLOG_H
#define CVTEST_CLOG_H
#include <string>
#include <fstream>
using namespace std;

class CLog {
public:
    CLog();
    ~CLog();
    bool record(const char* info) const;
private:
    const static string logFile;
    ofstream out;
};


#endif //CVTEST_CLOG_H
