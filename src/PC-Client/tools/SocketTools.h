#ifndef SOCKETTOOLS_H
#define SOCKETTOOLS_H

#include <string>
#include <stdint.h>
#include <glib.h>

using namespace std;

class SocketTools {
public:
    static const int CONNECT_TIMEOUT = 15;
    static const int RECEIVE_TIMEOUT = 10;

    static int create_socket();

    static bool is_local_port_available(uint16_t local_port);

    static bool connect_to_server(int socket_fd, const string &ip_addr, uint16_t port);

    static bool send_msg(int socket_fd, const string &msg_str);

    static bool receive_msg(int socket_fd, string &msg_recv);

    static bool send_urgent_data(int socket_fd);

    static bool receive_bytes(int socket_fd, GBytes **bytes, int bytes_length);

    static bool receive_file(int socket_fd, const string &file_path, int64_t file_length);
};

#endif // SOCKETTOOLS_H
