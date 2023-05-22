#include <iostream>
#include <cstring>
#include <sstream>
#include <unistd.h>

#include "Server.hpp"
#include "defs.hpp"
#include "Channel.hpp"

Server::~Server() {}

Server::Server(const char *port) : max_clients(MAX_CLIENTS), _port(port)
{

   this->sv_socket_info.sin_port = htons(4242);//atoi(port)); //
   this->sv_socket_info.sin_family = AF_INET; //ipv4
   this->sv_socket_info.sin_addr.s_addr = INADDR_ANY; //Aceptar cualquier dirección //inet_addr("127.0.0.1"); //modificar a ser necesario
   this->_socket = socket(this->sv_socket_info.sin_family, SOCK_STREAM, 0);
    if (this->_socket == -1)
    {
        std::cout << SOCKET_ERROR << std::endl;
        exit (0);
    }
    int option = 1;
    while (setsockopt(this->_socket,SOL_SOCKET,SO_REUSEADDR, &option, sizeof(option)) < 0)
    {
        std::cerr << SOCKET_PREF_ERROR << std::endl;
        std::cerr << "Intentandolo de nuevo . . ." << std::endl;
        usleep(1000000);
    }
    if (bind(this->_socket, (sockaddr *) &this->sv_socket_info, sizeof(sv_socket_info)) < 0) {
        std::cerr << "Error al asignar la dirección al socket\n";
        exit (0);
    }
    listen(this->_socket,MAX_CLIENTS);
    _init_cout();
}


/**
 * @attention Funciones destacadas
 * socket : Crea un endpoint
 *
 *
 *
 * setsockopt :
 *
 * @param Argc,Port,Password
 * @return Void
 * */
void	Server::_init_cout() const
{
        std::cerr << "El servidor ha sido iniciado ... \n"
        << " _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _\n"
        << "| Puerto : "
        << this->_port << "                                 |"
        << std::endl
        << "| Protocolo : "
        << (AF_INET == 2 ? "Ipv4" : AF_INET6 == 1 ? "Ipv6" : "Uknown")
        << "                              |"
        << std::endl
        << "| Tipo de conexión : " << (SOCK_STREAM == 1 ? "Socket de conexiones" : "Uknown")
        << "       |"
        << std::endl
        << " ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯\n";
}

const int	Server::get_socket() const {return this->_socket;}

void    Server::_create_new_user(ssize_t rd_size)
{
    char        *start_nick;
    char        *end_nick;
    size_t      len;

    start_nick = strstr(this->buffer, NICK);//, rd_size);
    end_nick = strstr(start_nick, MSG_END);//, rd_size);
    len = end_nick - start_nick - 1;
    std::string nickname(start_nick + OFF_NICK, len);
    std::cout << nickname;
}

void    Server::enter_msg(int client)
{
    char    send_buffer[BUFFER_SIZE];
    ssize_t rd_size;

    this->users[client]->set_notices();
    rd_size = recv(this->fds[client].fd, this->buffer, BUFFER_SIZE, 0);

    this->_create_new_user(rd_size);
    std::cout << this->buffer << std::endl;

    if (rd_size == -1)
        exit(0);
    this->buffer[rd_size] = 0;

    rd_size = sprintf(send_buffer, "%s %s : %s\r\n", PRIVMSG, MAIN_CHANNEL, WELCOME_MSG);
    send(this->fds[client].fd, send_buffer, rd_size, 0);

}

void    Server::send_msg(int client)
{
    char                send_buffer[BUFFER_SIZE];
    ssize_t             rd_size;
    std::stringstream   client_stream;
    std::string         client_msg;

    rd_size = recv(this->fds[client].fd, send_buffer, BUFFER_SIZE, 0);

    if (rd_size == -1)
        exit(1);
    send_buffer[rd_size] = 0;

    for(int iter = 0; iter != this->users.size(); iter++)
    {
        if (iter != client)
            client_stream << PRIVMSG << " " << MAIN_CHANNEL << " :" << OTHER << send_buffer << MSG_END;
        else
            client_stream << PRIVMSG << " " << MAIN_CHANNEL << " :" << YOU << send_buffer << MSG_END;

        client_msg = client_stream.str();
        std::cout << client_msg;
        send(this->fds[iter].fd, client_msg.c_str(), rd_size, 0);
    }
}

void    Server::accept_new_user()
{
    int         client_socket;
    sockaddr_in client;
    socklen_t   size;
    User        *new_user;
    pollfd      new_pollfd;

    client_socket = accept(this->_socket, (sockaddr *)&client, &size);
    new_user = new User("ANON");
    this->users.push_back(new_user);

    new_pollfd.fd = client_socket;
    new_pollfd.events = POLLIN;
    this->fds.push_back(new_pollfd);
}
