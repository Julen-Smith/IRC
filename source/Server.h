#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <unistd.h> 
#include <cstring>
#include <exception>

typedef struct server
{
    int server_fd;
    int new_socket;
    int valred;
    struct sockaddr_in address;
    int addrlen;
    char *buffer;
} Server_data;

void start_server(Server_data *server);
void init_socket(Server_data *server);
void bind_socket(Server_data *server);
void listen_cli(Server_data *server);
void incoming(Server_data *server);
void send_message(Server_data *server);
void recibir_mensaje(Server_data *server);

#endif