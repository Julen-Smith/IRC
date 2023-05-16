#include <iostream>
#include <cstring>
#include <unistd.h>

#include "../inc/Server.hpp"
#include "defs.hpp"
#include "../inc/Channel.hpp"
//#include "../inc/Server.h"

//Server::Server() : max_clients(MAX_CLIENTS)
//{
//	std::cout << "Default Server\n";
//}

Server::~Server() {}

Server::Server(const char *port) : _port(port), max_clients(MAX_CLIENTS)
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
void	Server::_init_cout()
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

