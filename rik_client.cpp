#include "rik_client.h"

#include <QDebug>

int RIK_Client::connect_server(QString address, int port)
{
    const char * addr = address.toStdString().c_str();
    ucli.connect_server(addr, port);
    if(!ucli.isTcpListen)
    {
        qDebug() << "Error on UDP connecting";
        return -1;
    }
    int tcp_port = ucli.tcp_port;
    if (cli.connect_server(addr, tcp_port) < 0)
    {
        qDebug() << "Error on TCP connecting";
        return -1;
    }
    return 0;
}

int RIK_Client::disconnect_server()
{
    cli.disconnect_server();
    cli = TcpClient();
    qDebug() << "disconnected";
    return 0;
}

int RIK_Client::switch_robot(bool on)
{
    if (on)
        return cli.load_robot() && download_robot_info() == 0;
    else
    {
        robot.clear();
        return cli.release_robot();
    }
}

int RIK_Client::switch_motor(bool on)
{
    if (on)
        return cli.send_msg("start motor");
    else
        return cli.send_msg("stop motor");
}

int RIK_Client::send_mode(int m)
{
    return cli.send_mode(m);
}

int RIK_Client::send_target_joints(int j1, int j2, int j3, int j4, int j5, int j6)
{
    Eigen::VectorXf t(6);
    t << j1, j2, j3, j4, j5, j6;
    qDebug() << j1;
    return cli.send_target_joints(t);
}

int RIK_Client::send_target_pose(QVector3D v1, QVector3D v2)
{
    Eigen::VectorXf t(6);
    t << v1.x(), v1.y(), v1.z(), v2.x(), v2.y(), v2.z();
    return cli.send_target_pose(t);
}

int RIK_Client::send_target_path(QVector3D v1, QVector3D v2, int t)
{
    Eigen::VectorXf p(7);
    p << v1.x(), v1.y(), v1.z(), v2.x(), v2.y(), v2.z(), t;
    return cli.send_target_path(p);
}

int RIK_Client::send_target_path(QVector3D t1, QVector3D t2, QVector3D m1, QVector3D m2, int m_spd, int m_acc)
{
    Eigen::VectorXf t(14);
    t << t1.x(), t1.y(), t1.z(), t2.x(), t2.y(), t2.z(),
         m1.x(), m1.y(), m1.z(), m2.x(), m2.y(), m2.z(),
         m_spd, m_acc;
    return cli.send_target_path(t);
}

void RIK_Client::get_joints()
{
    Eigen::VectorXf js;
    cli.get_current_joints(js);
    robot.update_joints(js);
    qDebug() << "current joints:";
    for (int i = 0; i < js.size(); ++i) {
        qDebug() << js(i);
    }
//    return js;
}

void RIK_Client::get_pose()
{
    Eigen::VectorXf p;
    cli.get_current_pose(p);
    robot.update_pose(p);
    qDebug() << "current pose:";
    for (int i = 0; i < p.size(); ++i) {
        qDebug() << p(i);
    }
//    return p;
}

int RIK_Client::download_robot_info()
{
    dh_table t;
    joints_limits jl;
    if (cli.download_robot_info(t, jl) < 0)
    {
        qDebug() << "ERROR on download robot info";
        return -1;
    }
    robot.update_table(t);
    robot.update_joints_limits(jl);
    return 0;
}