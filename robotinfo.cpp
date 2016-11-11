#include "robotinfo.h"

#include <QDebug>

RobotInfo::RobotInfo():
    joints_num(-1)
{
}

void RobotInfo::update_table(const dh_table &t)
{
    table = t;
    joints_num = t.size();
}

void RobotInfo::update_joints_limits(const joints_limits &jl)
{
    limit = jl;
}

void RobotInfo::update_joints(const Eigen::VectorXf &j)
{
    joints = j;
}

void RobotInfo::update_pose(const Eigen::VectorXf &p)
{
    pose = p;
}

void RobotInfo::print_info()
{
    qDebug() << "robot joints num: " << joints_num;
    qDebug() << "dh table size: " << table.size();
    qDebug() << "joints limits size: " << limit.size();
    qDebug() << "current joints: ";
    for (int i = 0; i < joints.rows(); ++i)
    {
        qDebug() << joints(i);
    }
    qDebug() << "current pose: ";
    for (int i = 0; i < pose.rows(); ++i)
    {
        qDebug() << pose(i);
    }
}
