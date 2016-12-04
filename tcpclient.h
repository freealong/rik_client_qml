#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "tcpcommon.h"

#include <QDebug>

class TcpClient
{
public:
    TcpClient();

    inline void print(const char *m)
    {
        qDebug() << m;
    }
    inline bool is_connected()
    {
        return sockfd > 0;
    }
    inline bool is_loaded()
    {
        return load;
    }
    inline bool is_robot_ready()
    {
        return load && is_connected();
    }

    int connect_server(const char* address, int port);
    int disconnect_server();
    int send_msg(std::string msg);
    int load_robot();
    int release_robot();
    int download_robot_info(dh_table& t, joints_limits& jl);
    int upload_robot_info(dh_table& t, joints_limits& jl);
    int set_motors(Eigen::VectorXf &t);
    int get_current_pose(Eigen::VectorXf &v);
    int get_current_joints(Eigen::VectorXf &v);
    int send_target(Eigen::VectorXf &target, int mode);
    int send_target_joints(Eigen::VectorXf &target);
    int send_target_pose(Eigen::VectorXf &target);
    int send_target_path(Eigen::VectorXf &target);
    int send_mode(int mode);
    int send_increasing_mode(bool running, int mode, int num, float speed);
    int send_task(std::vector<Eigen::VectorXf> &t, int n);
    int send_demo(int type);
    void test();


private:
    char msg[msg_size];
    char buf[buf_size];
    int sockfd;
    bool load;
};

#endif // TCPCLIENT_H
