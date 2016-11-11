#include "udpclient.h"

UdpClient::UdpClient() : sockfd(-1), isTcpListen(false), tcp_port(0)
{
    memset(msg, 0x00, msg_size);
    memset(buf, 0x00, buf_size);
}

void UdpClient::connect_server(const char *address, int port)
{
    struct sockaddr_in serv_addr;
    struct hostent *server;
    isTcpListen = false;
    tcp_port = 0;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
//        print("ERROR opening socket");
        return;
    }
    server = gethostbyname(address);
    if (server == NULL) {
//        print("ERROR, no such host\n");
        sockfd = -1;
        return;
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(port);
    if (connect(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
//        print("ERROR connecting");
        sockfd = -1;
        return;
    }
    char requestToConnect[] = "new\n";
    char receiveData[80];
    write(sockfd, requestToConnect, strlen(requestToConnect));
    read(sockfd, receiveData, sizeof(receiveData));
    memcpy(&tcp_port, receiveData, sizeof(tcp_port));
    if(tcp_port > 1024 && tcp_port < 65535)
    {
        isTcpListen = true;
        qDebug() << "tcp port = " << tcp_port;
    }
    else
    {
        qDebug() << "udp connect ERROR.";
    }
    return;
}

void UdpClient::disconnect_server()
{
    close(sockfd);
    sockfd = -1;
}
