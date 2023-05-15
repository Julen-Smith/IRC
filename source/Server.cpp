#include <iostream>
#include <cstring>
#include <unistd.h>

#include "Server.hpp"
#include "defs.hpp"

Server::Server() : max_clients(MAX_CLIENTS)
{
	std::cout << "Default Server\n";
}

Server::~Server() {}

Server::Server(const char *port) : _port(port), max_clients(MAX_CLIENTS)
{
	int	retval;

	std::cout << "Serve instated, asigned port is: " << this->_port;
	memset(&this->_hint, 0, sizeof(this->_hint));

	this->_hint.ai_family = AF_INET;
	this->_hint.ai_socktype = SOCK_STREAM;
	this->_hint.ai_flags = AI_PASSIVE;
	this->_hint.ai_protocol = IPPROTO_TCP;

	retval = getaddrinfo(NULL, port, &this->_hint, &this->_serv_info);
	this->_socket = socket(this->_serv_info->ai_family, this->_serv_info->ai_socktype, this->_serv_info->ai_protocol);
	if (this->_socket < 0)
	{
		std::cerr << SOCKET_ERROR << std::endl;
		//Soltar excepción y propagarla o gestionarla de alguna manera
		exit(0);
	}
	retval = 1;

        while (setsockopt(this->_socket, SOL_SOCKET, SO_REUSEADDR, &retval, sizeof(retval)) == -1)
        {
            std::cerr << SOCKET_PREF_ERROR << std::endl;
            std::cerr << "Intentandolo de nuevo . . ." << std::endl;
            usleep(500000);
            //Soltar excepción y propagarla o gestionarla de alguna manera
            //  exit(0);
        }

	this->_init_cout();
	retval = bind(this->_socket, this->_serv_info->ai_addr, this->_serv_info->ai_addrlen);
	if (retval == -1)
	{
		std::cerr << "Error al asignar la dirección al socket\n";
		exit (0);
	}

    	retval = listen(this->_socket, max_clients);
	if (retval == -1)
	{
		std::cerr << "Error al escuchar la dirección al socket\n";
		exit (0);
	}
}

void	Server::_init_cout()
{
        std::cerr << "El servidor ha sido iniciado ... \n"
        << " _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _\n"
        << "| Puerto : "
        << this->_port << "                                 |"
        << std::endl
        << "| Protocolo : "
        //<< (server->ipv4 == 2 ? "Ipv4" : server->ipv4 == 1 ? "Ipv6" : "Uknown")
        << "                              |"
        << std::endl
        << "| Tipo de conexión : " << (this->_hint.ai_socktype == 1 ? "Socket de conexiones" : "Uknown")
        << "       |"
        << std::endl
        << " ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯\n";
}

const int	Server::get_socket() const {return this->_socket;}
