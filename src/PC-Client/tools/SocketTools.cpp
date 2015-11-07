#include "SocketTools.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 1024

using namespace std;

int SocketTools::create_socket()
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0)
    {
        fprintf(stderr, "Create socket failed:%s\n", strerror(errno));
        return -1;
    }
    return socket_fd;
}

bool SocketTools::is_local_port_available(uint16_t local_port)
{
    int test_socket = create_socket();
    if(test_socket < 0)
        return false;
    sockaddr_in test_addr;
    test_addr.sin_family = AF_INET;
    test_addr.sin_port = htons(local_port);
    test_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    int ret = bind(test_socket, (sockaddr*)&test_addr, sizeof(test_addr));
    close(test_socket);
    if(ret < 0)
    {
        fprintf(stderr, "Bind port failed:%s\n", strerror(errno));
        return false;
    }
    return true;

}

bool SocketTools::connect_to_server(int socket_fd, const string& ip_addr, uint16_t port)
{
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip_addr.c_str());
    if(connect(socket_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        fprintf(stderr, "Connect failed:%s\n", strerror(errno));
        return false;
    }
    return true;
}

bool SocketTools::receive_msg(int socket_fd, string& msg_recv)
{
    msg_recv = "";
    char buf[MAX_SIZE + 1];
    while(true)
    {
        int len = recv(socket_fd, buf, sizeof(buf) - 1, 0);
        if(len <= 0)
        {
            fprintf(stderr, "Receive message failed:%s\n", strerror(errno));
            return false;
        }
        buf[len] = '\0';
        msg_recv += buf;
        if(len < sizeof(buf) - 1)
            break;
    }
    return true;
}

bool SocketTools::send_msg(int socket_fd, const string& msg_str)
{
    if(send(socket_fd, msg_str.c_str(), msg_str.length(), 0) < 0)
    {
        fprintf(stderr, "Send message failed:%s\n", strerror(errno));
        return false;
    }
    return true;
}

bool SocketTools::send_urgent_data(int socket_fd)
{
    if(send(socket_fd, "0", 1, MSG_OOB) < 0)
    {
        fprintf(stderr, "Send urgent data failed:%s\n", strerror(errno));
        return false;
    }
    return true;
}
