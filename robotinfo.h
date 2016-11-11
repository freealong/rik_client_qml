#ifndef ROBOTINFO_H
#define ROBOTINFO_H

#include "sharetypes.h"
#include <Eigen>

class RobotInfo
{
public:
    RobotInfo();
    void update_table(const dh_table& t);
    void update_joints_limits(const joints_limits& jl);
    void update_joints(const Eigen::VectorXf& j);
    void update_pose(const Eigen::VectorXf& p);
    void print_info(void);
    bool is_initialized() {
        return joints_num >= 0;
    }
    void clear() {
        joints_num = -1;
        table.clear();
        limit.clear();
    }

    int get_joints_num()
    {
        return joints_num;
    }
    dh_table get_dh_table()
    {
        return table;
    }
    joints_limits get_joints_limits()
    {
        return limit;
    }
    Eigen::VectorXf get_joints()
    {
        return joints;
    }
    Eigen::VectorXf get_pose()
    {
        return pose;
    }

private:
    int joints_num;
    dh_table table;
    joints_limits limit;
    Eigen::VectorXf joints;
    Eigen::VectorXf pose;
};

#endif // ROBOTINFO_H
