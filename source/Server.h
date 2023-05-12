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
    int port; // Puerto
    int server_socket; //Fd que utiliza el servidor como socket
    int ipv4;
    int socket_type;
} Server;

void main_loop(Server *server);
void print_server_menu(Server *server);

#endif