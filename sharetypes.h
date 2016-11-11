#ifndef SHARETYPES_H
#define SHARETYPES_H

#include <vector>
#include <string>

/*
 * client respond code meaning
 * command meaning
 * 100     asking for dh table
 *
 * server respond code meaning
 * 101     dh table send finished
 * */

enum JointT {
  REVOLUTE,
  PRISMATIC,
  CONSTANTJOINT,
  NOTSET
};
enum AxisT {
  AxisX,
  AxisY,
  AxisZ
};

struct dh_parametrs {
  float a;            //Length of common normal
  float alpha;        //Angle between zi and zi-1 along xi
  float d;      //distance between xi and xi-1 along zi (variable for prismatic)
  float theta;   //Angle between xi and xi-1 along zi    (variavle for revolute)
  JointT z_joint_type;        //Joint type at z-1 axis
  std::string joint_name;
};

struct limits {
  float min;
  float max;
};

typedef std::vector<limits> joints_limits;
typedef std::vector<dh_parametrs> dh_table;

#endif // SHARETYPES_H
