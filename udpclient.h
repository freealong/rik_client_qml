#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include "tcpcommon.h"

#include <QDebug>

class UdpClient
{
public:
    UdpClient();

    inline void print(const char *m)
    {
        qDebug() << m;
    }
    inline bool is_connected()
    {
        return sockfd > 0;
    }

    void connect_server(const char* address, int port);
    void disconnect_server();
    void test();
    bool isTcpListen;
    int tcp_port;

private:
    char msg[msg_size];
    char buf[buf_size];
    int sockfd;
};

#endif // UDPCLIENT_H
