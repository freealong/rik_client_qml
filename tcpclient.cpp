#include "tcpclient.h"

TcpClient::TcpClient() : sockfd(-1), load(false)
{
    memset(msg, 0x00, msg_size);
    memset(buf, 0x00, buf_size);
}

int TcpClient::connect_server(const char *address, int port)
{
    struct sockaddr_in serv_addr;
    struct hostent *server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        print("ERROR opening socket");
        return -1;
    }
    server = gethostbyname(address);
    if (server == NULL) {
        print("ERROR, no such host\n");
        sockfd = -1;
        return -1;
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(port);
//    qDebug() << "port = " << port;
//    qDebug() << "sockfd = " << sockfd;
    if (connect(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        print("ERROR tcp connecting");
        sockfd = -1;
        return -1;
    }
    print("connect to server successfully.");

    return 0;
}

int TcpClient::disconnect_server()
{
    close(sockfd);
    sockfd = -1;
    return 0;
}

int TcpClient::send_msg(std::string msg)
{
    if (!is_connected())
        return -1;

    write(sockfd, msg);
    return 0;
}

int TcpClient::load_robot()
{
    if (!is_connected())
        return -1;

    std::string msg("load robot");
    sleep(0.1);
    write(sockfd, msg);
//    print("reading msg...");
    sleep(1);
    read(sockfd, msg);
//    print("finish");
    if (msg == "error on load robot")
    {
        print("ERROR on load robot");
        load = false;
        return -1;
    }
    else
    {
        load = true;
        return 0;
    }
}

int TcpClient::release_robot()
{
    if (!is_connected())
        return -1;

    std::string msg("release robot");
    sleep(0.1);
    write(sockfd, msg);
    sleep(1);
    return 0;
}

int TcpClient::download_robot_info(dh_table &t, joints_limits &jl)
{
    if (!is_robot_ready())
        return -1;

    std::string msg("download robot info");
    write(sockfd, msg);
    read(sockfd, t);
    read(sockfd, jl);
    return 0;
}

int TcpClient::upload_robot_info(dh_table &t, joints_limits &jl)
{
    if (!is_robot_ready())
        return -1;

    std::string msg("upload DH table");
    write(sockfd, msg);
    write(sockfd, t);
    write(sockfd, jl);
    return 0;
}

int TcpClient::get_current_pose(Eigen::VectorXf &v)
{
    if (!is_robot_ready())
        return -1;

    std::string msg("get current pose");
    write(sockfd, msg);
    read(sockfd, v);
    return 0;
}

int TcpClient::get_current_joints(Eigen::VectorXf &v)
{
    if (!is_robot_ready())
        return -1;

    std::string msg("get current joints");
    write(sockfd, msg);
    read(sockfd, v);
    return 0;
}

int TcpClient::send_target(Eigen::VectorXf &target, int mode)
{
    int res;
    switch(mode)
    {
    case 0:
        res = send_target_joints(target);
        break;
    case 1:
        res = send_target_pose(target);
        break;
    case 2:
        res = send_target_path(target);
        break;
    default:
        res = -1;
        break;
    }
    return res;
}

int TcpClient::send_target_joints(Eigen::VectorXf &target)
{
    if (!is_robot_ready())
        return -1;

    std::string msg("set target joints");
    write(sockfd, msg);
    write(sockfd, target);
    return 0;
}

int TcpClient::send_target_pose(Eigen::VectorXf &target)
{
    if (!is_robot_ready())
        return -1;

    std::string msg("set target pose");
    write(sockfd, msg);
    write(sockfd, target);
    return 0;
}

int TcpClient::send_target_path(Eigen::VectorXf &target)
{
    if (!is_robot_ready())
        return -1;

    std::string msg("set task path");
    write(sockfd, msg);
    write(sockfd, target);
    return 0;
}

int TcpClient::send_mode(int mode)
{
    if (!is_robot_ready())
        return -1;

    std::string msg("set control mode");
    write(sockfd, msg);
    write(sockfd, &mode, sizeof(mode));
    return 0;
}

int TcpClient::send_increasing_mode(bool running, int mode, int num, float speed)
{
    if (!is_robot_ready())
        return -1;

    std::string msg("set increasing state");
    write(sockfd, msg);
    write(sockfd, &running, sizeof(running));
    write(sockfd, &mode, sizeof(mode));
    write(sockfd, &num, sizeof(num));
    write(sockfd, &speed, sizeof(speed));
    return 0;
}

void TcpClient::test()
{
    std::string msg("test");
    write(sockfd, msg);
    Eigen::VectorXf des;
    read(sockfd, des);
    qDebug() << des(5);
}
