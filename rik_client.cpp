#include "rik_client.h"

#include <QDebug>

int RIK_Client::connect_server(QString address, int port)
{
    std::string addr = address.toStdString();
    ucli.connect_server(addr.c_str(), port);
    if(!ucli.isTcpListen)
    {
        qDebug() << "Error on UDP connecting";
        return -1;
    }
    int tcp_port = ucli.tcp_port;
    if (cli.connect_server(addr.c_str(), tcp_port) < 0)
    {
        qDebug() << "Error on TCP connecting";
        return -1;
    }

    std::string cv_addr = "127.0.0.1";
    int cv_port = 9000;
    if (cv_cli.connect_server(cv_addr.c_str(), cv_port) < 0)
    {
        qDebug() << "Error on cv_TCP connecting";
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
        return cli.load_robot() && download_robot_info() == 0 && send_mode(0);
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

int RIK_Client::set_motors(int j1, int j2, int j3, int j4, int j5, int j6)
{
    Eigen::VectorXf t(6);
    t << j1, j2, j3, j4, j5, j6;
    return cli.set_motors(t);
}

int RIK_Client::send_mode(int m)
{
    return cli.send_mode(m);
}

int RIK_Client::send_target_joints(int j1, int j2, int j3, int j4, int j5, int j6)
{
    Eigen::VectorXf t(6);
    t << j1, j2, j3, j4, j5, j6;
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
    //joints
    Eigen::VectorXf js;
    cli.get_current_joints(js);
    robot.update_joints(js);
    qDebug() << "current joints:";
    for (int i = 0; i < js.size(); ++i) {
        qDebug() << js(i);
    }

    //pose
    Eigen::VectorXf p;
    cli.get_current_pose(p);
    robot.update_pose(p);
    qDebug() << "current pose:";
    for (int i = 0; i < p.size(); ++i) {
        qDebug() << p(i);
    }

    //wirte file
    file.open("log_joints_and_pose.txt",ios_base::app);
    if(file.is_open()){
        for(int i=0;i<js.size();++i){
            file<<js(i)<<",";
        }
        for(int i=0;i<p.size();++i){
            if(i<p.size()-1){
                file<<p(i)<<",";
            }
            else{
                file<<p(i);
            }
        }
        file<<endl;
        file.close();
    }
    else{
        qDebug() << "file open fail";
    }
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

int RIK_Client::send_increasing_mode(bool running, int mode, int num, float speed)
{
    return cli.send_increasing_mode(running, mode, num, speed);
}

int RIK_Client::send_task(QString s1, QString s2, int loop_num)
{
    qDebug() << "s1 = "<< s1 << ", s2 = "<< s2 ;
    std::vector<Eigen::VectorXf> tasks;
    QStringList type_list = s1.split(";");
    QStringList params_list = s2.split(";");

    QStringList tmp;
    for (int i = 0; i < type_list.size()-1; i++)
    {
        tmp = params_list.at(i).split(",");
        if (type_list.at(i) == "0")
        {
            Eigen::VectorXf t(tmp.size());
            for (int i = 0; i < t.size(); ++i)
            {
                t(i) = tmp.at(i).toInt();
            }
            tasks.push_back(t);
        }
        else if (type_list.at(i) == "1")
        {
            Eigen::VectorXf t(14);
            t << tmp.at(0).toInt(), tmp.at(1).toInt(), tmp.at(2).toInt(), tmp.at(3).toInt(),
                    tmp.at(4).toInt(), tmp.at(5).toInt(), tmp.at(6).toInt(),
                    tmp.at(7).toInt(), tmp.at(8).toInt(), tmp.at(9).toInt(),
                    tmp.at(10).toInt(), tmp.at(11).toInt(), tmp.at(12).toInt(), tmp.at(13).toInt();
            tasks.push_back(t);
        }
    }
    cli.send_task(tasks, loop_num);
}

int RIK_Client::send_msg(QString m)
{
    return cli.send_msg(m.toStdString());
}

int RIK_Client::send_demo(int t)
{
    return cli.send_demo(t);
}

int RIK_Client::send_save_img_msg()
{
    qDebug() << "save..";
    get_joints();
    QTimer::singleShot(3000, this, SLOT(auto_start()));
    return cv_cli.send_msg("save");
}

int RIK_Client::auto_start()
{
    qDebug() << "auto_start..";
    if(stop_flag){
        qDebug() << "return 2";
        return 2;
    }

    //send_task(QString s1, QString s2, int loop_num);
    ++pose_num;
    QString s1 = "";
    s1 += "0";
    s1 += ";";
    QString s2 = "";
    int runTime = 5000;
    switch(pose_num){
//    case 1:
//        s2 = "860,0,846,0,0,0,4000;";
//        break;
    case 1:
        s2 = "860,0,446,0,0,0,10000;";
        runTime = 11000;
        break;
    case 2:
        s2 = "860,0,446,0,0,30,4000;";
        break;
    case 3:
        s2 = "860,0,446,0,0,60,4000;";
        break;
    case 4:
        s2 = "860,0,446,0,0,90,4000;";
        break;
    case 5:
        s2 = "860,0,446,0,0,120,4000;";
        break;
    case 6:
        s2 = "860,0,446,0,0,110,4000;";
        break;
    case 7:
        s2 = "860,0,446,0,0,80,4000;";
        break;
    case 8:
        s2 = "860,0,446,0,0,50,4000;";
        break;
    case 9:
        s2 = "860,0,446,0,0,20,4000;";
        break;
    case 10:
        s2 = "860,0,446,0,0,-10,4000;";
        break;
    case 11:
        s2 = "860,0,446,0,0,-40,4000;";
        break;
    case 12:
        s2 = "860,0,446,0,0,-70,4000;";
        break;
    case 13:
        s2 = "860,0,446,0,0,-100,4000;";
        break;
    case 14:
        s2 = "860,0,446,0,0,-80,4000;";
        break;
    case 15:
        s2 = "860,0,446,0,0,-50,4000;";
        break;
    case 16:
        s2 = "860,0,446,0,0,-20,4000;";
        break;
    case 17:
        s2 = "860,0,446,0,0,0,4000;";
        break;
    case 18:
        s2 = "880,0,446,0,0,0,4000;";
        break;
    case 19:
        s2 = "900,0,446,0,0,0,4000;";
        break;
    case 20:
        s2 = "920,0,446,0,0,0,4000;";
        break;
    case 21:
        s2 = "940,0,446,0,0,0,4000;";
        break;
    case 22:
        s2 = "960,0,446,0,0,0,4000;";
        break;
    case 23:
        s2 = "950,0,446,0,0,0,4000;";
        break;
    case 24:
        s2 = "930,0,446,0,0,0,4000;";
        break;
    case 25:
        s2 = "910,0,446,0,0,0,4000;";
        break;
    case 26:
        s2 = "890,0,446,0,0,0,4000;";
        break;
    case 27:
        s2 = "870,0,446,0,0,0,4000;";
        break;
    case 28:
        s2 = "850,0,446,0,0,0,4000;";
        break;
    case 29:
        s2 = "830,0,446,0,0,0,4000;";
        break;
    case 30:
        s2 = "810,0,446,0,0,0,4000;";
        break;
    case 31:
        s2 = "790,0,446,0,0,0,4000;";
        break;
    case 32:
        s2 = "800,0,446,0,0,0,4000;";
        break;
    case 33:
        s2 = "820,0,446,0,0,0,4000;";
        break;
    case 34:
        s2 = "840,0,446,0,0,0,4000;";
        break;
    case 35:
        s2 = "860,0,446,0,0,0,4000;";
        break;
    case 36:
        s2 = "860,10,446,0,0,0,4000;";
        break;
    case 37:
        s2 = "860,20,446,0,0,0,4000;";
        break;
    case 38:
        s2 = "860,30,446,0,0,0,4000;";
        break;
    case 39:
        s2 = "860,25,446,0,0,0,4000;";
        break;
    case 40:
        s2 = "860,15,446,0,0,0,4000;";
        break;
    case 41:
        s2 = "860,5,446,0,0,0,4000;";
        break;
    case 42:
        s2 = "860,-5,446,0,0,0,4000;";
        break;
    case 43:
        s2 = "860,-15,446,0,0,0,4000;";
        break;
    case 44:
        s2 = "860,-25,446,0,0,0,4000;";
        break;
    case 45:
        s2 = "860,-30,446,0,0,0,4000;";
        break;
    case 46:
        s2 = "860,-20,446,0,0,0,4000;";
        break;
    case 47:
        s2 = "860,-10,446,0,0,0,4000;";
        break;
    case 48:
        s2 = "860,0,446,0,0,0,4000;";
        break;
    case 49:
        s2 = "860,0,436,0,0,0,4000;";
        break;
    case 50:
        s2 = "860,0,426,0,0,0,4000;";
        break;
    case 51:
        s2 = "860,0,416,0,0,0,4000;";
        break;
    case 52:
        s2 = "860,0,421,0,0,0,4000;";
        break;
    case 53:
        s2 = "860,0,431,0,0,0,4000;";
        break;
    case 54:
        s2 = "860,0,441,0,0,0,4000;";
        break;
    case 55:
        s2 = "860,0,451,0,0,0,4000;";
        break;
    case 56:
        s2 = "860,0,461,0,0,0,4000;";
        break;
    case 57:
        s2 = "860,0,471,0,0,0,4000;";
        break;
    case 58:
        s2 = "860,0,476,0,0,0,4000;";
        break;
    case 59:
        s2 = "860,0,466,0,0,0,4000;";
        break;
    case 60:
        s2 = "860,0,456,0,0,0,4000;";
        break;
    case 61:
        s2 = "860,0,446,0,0,0,4000;";
        break;
    default:
        qDebug() << "return 1";
        return 1;
    }
    qDebug()<< "send_task..";
    send_task(s1, s2, 1);
    QTimer::singleShot(runTime, this, SLOT(send_save_img_msg()));
    return 0;
}

int RIK_Client::disable_stop_flag()
{
    pose_num = 0;
    stop_flag = false;
    return 0;
}

int RIK_Client::enable_stop_flag()
{
    stop_flag = true;
    return 0;
}

int RIK_Client::get_pose_num()
{
    return pose_num;
}

int RIK_Client::increase_pose_num()
{
    ++pose_num;
    return 0;
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
