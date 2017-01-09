#ifndef RIK_CLIENT_H
#define RIK_CLIENT_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <QVector3D>

#include "udpclient.h"
#include "tcpclient.h"
#include "robotinfo.h"

#include "fstream"
using namespace  std;

class RIK_Client : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(RIK_Client)
    RIK_Client() {cnt = 0;}

public:
    static QObject *GetInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
    {
        Q_UNUSED(engine);
        Q_UNUSED(scriptEngine);

        return new RIK_Client;
    }

    int cnt;

signals:

public slots:
    int connect_server(QString address, int port);
    int disconnect_server();
    int switch_robot(bool on);
    int switch_motor(bool on);
    int set_motors(int j1, int j2, int j3, int j4, int j5, int j6);
    int send_mode(int m);
    int send_target_joints(int j1, int j2, int j3, int j4, int j5, int j6);
    int send_target_pose(QVector3D v1, QVector3D v2);
    int send_target_path(QVector3D v1, QVector3D v2, int t);
    int send_target_path(QVector3D t1, QVector3D t2, QVector3D m1, QVector3D m2, int m_spd, int m_acc);
    void get_joints();
    void get_pose();
    int send_increasing_mode(bool running, int mode, int num, float speed);
    int send_task(QString s1, QString s2, int loop_num);
    int send_msg(QString m);
    int send_demo(int t);

private:
    int download_robot_info();

    UdpClient ucli;
    TcpClient cli;
    RobotInfo robot;

    fstream file;
};

#endif // RIK_CLIENT_H
