#include "SocketTools.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include "CommandTools.h"

#define MAX_SIZE 2048

using namespace std;

const char *MESSAGE_RECEIVED = "msg:received";

int SocketTools::create_socket() {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        CommandTools::output_debug_info(string("Create socket failed:") + strerror(errno));
        return -1;
    }
    //int n_zero=0;
    //setsockopt(socket_fd, 0, SO_SNDBUF, (char*)&n_zero, sizeof(n_zero));
    return socket_fd;
}

bool SocketTools::is_local_port_available(uint16_t local_port) {
    int test_socket = create_socket();
    if (test_socket < 0)
        return false;
    sockaddr_in test_addr;
    test_addr.sin_family = AF_INET;
    test_addr.sin_port = htons(local_port);
    test_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    int ret = bind(test_socket, (sockaddr *) &test_addr, sizeof(test_addr));
    close(test_socket);
    if (ret < 0) {
        CommandTools::output_debug_info(string("Bind port failed: ") + strerror(errno));
        return false;
    }
    return true;

}

bool SocketTools::connect_to_server(int socket_fd, const string &ip_addr, uint16_t port) {
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip_addr.c_str());
    if (connect(socket_fd, (sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        CommandTools::output_debug_info(string("Connect failed:") + strerror(errno));
        return false;
    }
    return true;
}

bool SocketTools::receive_msg(int socket_fd, string &msg_recv) {
    msg_recv = "";
    char buf[MAX_SIZE + 1];
    while (true) {
        int len = recv(socket_fd, buf, sizeof(buf) - 1, 0);
        if (len <= 0) {
            CommandTools::output_debug_info(string("Receive message failed:") + strerror(errno));
            send_msg(socket_fd, MESSAGE_RECEIVED);
            return false;
        }
        buf[len] = '\0';
        msg_recv += buf;
        if (len < sizeof(buf) - 1)
            break;
    }
    send_msg(socket_fd, MESSAGE_RECEIVED);
    return true;
}

bool SocketTools::send_msg(int socket_fd, const string &msg_str) {
    string send_msg = msg_str + '\n';
    if (send(socket_fd, send_msg.c_str(), send_msg.length(), 0) < 0) {
        CommandTools::output_debug_info(string("Send message failed:") + strerror(errno));
        return false;
    }
    return true;
}

bool SocketTools::send_urgent_data(int socket_fd) {
    if (send(socket_fd, "0", 1, MSG_OOB) < 0) {
        CommandTools::output_debug_info(string("Send urgent data failed:") + strerror(errno));
        return false;
    }
    return true;
}

bool SocketTools::receive_bytes(int socket_fd, GBytes **bytes, int bytes_length) {
    GByteArray *bytes_array = g_byte_array_new();
    guint8 buf[MAX_SIZE * 10];
    int total_len = 0;
    while (total_len < bytes_length) {
        int len = recv(socket_fd, buf, sizeof(buf), 0);
        if (len <= 0) {
            CommandTools::output_debug_info(string("Receive bytes failed:") + strerror(errno));
            send_msg(socket_fd, MESSAGE_RECEIVED);
            return false;
        }
        bytes_array = g_byte_array_append(bytes_array, buf, len);
        total_len += len;
    }
    (*bytes) = g_byte_array_free_to_bytes(bytes_array);
    send_msg(socket_fd, MESSAGE_RECEIVED);
    return true;
}

bool SocketTools::receive_file(int socket_fd, const string &file_path, int64_t file_length) {
    char temp[1024];
    sprintf(temp, "File length=%ld", file_length);
    CommandTools::output_debug_info(temp);
    int file_fd = open(file_path.c_str(), O_WRONLY | O_CREAT, 0775);
    if (file_fd < 0) {
        CommandTools::output_debug_info(string("File open failed:") + strerror(errno));
        send_msg(socket_fd, MESSAGE_RECEIVED);
    }

    char *buf[MAX_SIZE * 10];
    int64_t total_len = 0;
    while (total_len < file_length) {
        int len = recv(socket_fd, buf, sizeof(buf), 0);
        if (len <= 0) {
            CommandTools::output_debug_info(string("Re failed:") + strerror(errno));
            fprintf(stderr, "Receive file failed:%s\n", strerror(errno));
            send_msg(socket_fd, MESSAGE_RECEIVED);
            close(file_fd);
            return false;
        }
        write(file_fd, buf, len);
        total_len += len;
    }
    if (file_fd < 0)
        return false;
    close(file_fd);
    send_msg(socket_fd, MESSAGE_RECEIVED);
    sprintf(temp, "File has been downloaded to %s.", file_path.c_str());
    CommandTools::output_debug_info(temp);
    sprintf(temp, "Received file length=%ld", total_len);
    CommandTools::output_debug_info(temp);
    return true;
}
