#include "tcpcommon.h"
#include <QDebug>

void write(int sockfd, const std::string &str) {
  uint32_t len = str.length();
//  qDebug() << "write len";
  write(sockfd, &len, sizeof(len));
  sleep(0.1);
//  qDebug() << "write data";
  write(sockfd, str.data(), len);
}

void write(int sockfd, const dh_parametrs &p) {
  write(sockfd, &(p.a), sizeof(p.a));
  write(sockfd, &(p.alpha), sizeof(p.alpha));
  write(sockfd, &(p.d), sizeof(p.d));
  write(sockfd, p.joint_name);
  write(sockfd, &(p.z_joint_type), sizeof(p.z_joint_type));
}

void write(int sockfd, const dh_table &t) {
  uint32_t len = t.size();
  write(sockfd, &len, sizeof(len));
  for (dh_table::const_iterator it = t.begin(); it != t.end(); it++) {
    write(sockfd, *it);
  }
}

void write(int sockfd, const limits& l) {
  write(sockfd, &(l.min), sizeof(l.min));
  write(sockfd, &(l.max), sizeof(l.max));
}

void write(int sockfd, const joints_limits& jl) {
  uint32_t len = jl.size();
  write(sockfd, &len, sizeof(len));
  for (joints_limits::const_iterator it = jl.begin(); it != jl.end(); it++) {
    write(sockfd, *it);
  }
}

void write(int sockfd, const Eigen::VectorXf &v) {
  uint32_t len = v.size() * sizeof(float);
  write(sockfd, &len, sizeof(len));
  write(sockfd, v.data(), len);
}

void read(int sockfd, std::string &str) {
  uint32_t len;
  char* buf;

  read(sockfd, &len, sizeof(len));
  buf = new char[len];
  read(sockfd, buf, len);
  str.assign(buf, len);
  delete[] buf;
}

void read(int sockfd, dh_parametrs &p) {
  read(sockfd, &(p.a), sizeof(p.a));
  read(sockfd, &(p.alpha), sizeof(p.alpha));
  read(sockfd, &(p.d), sizeof(p.d));
  read(sockfd, p.joint_name);
  read(sockfd, &(p.z_joint_type), sizeof(p.z_joint_type));
}

void read(int sockfd, dh_table &t) {
  uint32_t len;
  read(sockfd, &len, sizeof(len));
  t.clear();
  dh_parametrs p;
  for (unsigned int i = 0; i < len; ++i) {
    read(sockfd, p);
    t.push_back(p);
  }
  t.reserve(len);
}

void read(int sockfd, limits& l) {
  read(sockfd, &(l.min), sizeof(l.min));
  read(sockfd, &(l.max), sizeof(l.max));
}

void read(int sockfd, joints_limits& jl) {
  uint32_t len;
  read(sockfd, &len, sizeof(len));
  jl.clear();
  limits l;
  for (unsigned int i = 0; i < len; ++i) {
    read(sockfd, l);
    jl.push_back(l);
  }
  jl.reserve(len);
}

void read(int sockfd, Eigen::VectorXf &v) {
  uint32_t len, sz;
  float* buf;

  read(sockfd, &len, sizeof(len));
  buf = new float[len];
  sz = len / sizeof(float);
  read(sockfd, buf, len);
  v.resize(sz);
  for (unsigned i = 0; i < sz; ++i) {
    v(i) = buf[i];
  }
  delete[] buf;
}
