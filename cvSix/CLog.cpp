//
// Created by han on 16-10-23.
//

#include "CLog.h"
#include <iostream>
const string CLog::logFile = "../log/logs_backup";

CLog::CLog() {
    out.open(logFile, ios::out|ios::app);
}

CLog::~CLog() {
    if(out.is_open()) {
        out.close();
    }
}

bool CLog::record(const char *info) const{
    if(!out.is_open()) {
        cerr<< "Log File is not open!" <<endl;
        return false;
    }
    //-- todo: what happend?
    //out << info <<endl;
    return true;
}


