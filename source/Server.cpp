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

    char buffer[BUFFER_SIZE];
    std::vector<pollfd> fds;
    std::vector<bool> notices;
    std::vector<std::string> message_list;
    std::string password  = "42Urduliz";
    Channel lobby(server);

    fds.push_back(pollfd());
    fds[0].fd = server->server_socket;
    fds[0].events = POLLIN;
    while (true)
    {
        int event_to_handle = poll(&fds[0], fds.size(), 0);
        for(int i = 0; i < fds.size(); i++)
        {
            if (fds[i].revents & POLLIN)  //Comprobación cambios fichero
            {
                if (fds[i].fd == server->server_socket)
                {
                    sockaddr_in clientes;
                    socklen_t client_address_size = sizeof(clientes);
                    int client_fd = accept(server->server_socket, (sockaddr *)&clientes, &client_address_size);
                    fds.push_back(pollfd());
                    fds.back().fd = client_fd;
                    fds.back().events = POLLIN;
                    notices.push_back(false);
                }
                else
                {
                    //ssize_t bytes_read = recv(fds[i].fd, buffer, BUFFER_SIZE, 0); 
            //        memset(buffer, 0, sizeof(buffer));
            //        recv(fds[1].fd, buffer, BUFFER_SIZE,0);

                    std::string saludo = "Welcome to the Server Lobby.";
                    std::string channel = "";
                    //std::string message = "SQUIT";
                    std::string ircMessage = "PRIVMSG " + channel + " :" + saludo + "\r\n";

                    send(fds[1].fd,ircMessage.c_str(),ircMessage.size(),0);
                    std::string format(buffer);

                    std::cout << buffer << std::endl;   
                    //format.clear();
                    std::string lobby_message = "<-Server-> Welcome to Lobby\n";
                    send(fds[i].fd, lobby_message.c_str(), lobby_message.length(), 0);
                    if (notices[i] == false)
                    {
                          notices[i] = true;

                      
                        
                        
                        //
                        //
                      /*
                      
                        ssize_t bytes_read = recv(fds[i].fd, buffer, BUFFER_SIZE, 0); 
                        std::cout << "Se recibe la password : " << buffer << std::endl;
                      
                        if (!password.compare(buffer))
                        {
                            memset(buffer, 0, sizeof(buffer));
                            std::cout << "Password confirm successfull" << std::endl;
                            std::string lobby_message = "<-Server-> Welcome to Lobby\n";
                            //Rutina de creación de usuarios
                            send(fds[i].fd, lobby_message.c_str(), lobby_message.length(), 0);

                            std::string nickname_question = "<-Server-> Please enter your nickname : ";
                            send(fds[i].fd, nickname_question.c_str(), nickname_question.length(), 0);
                            memset(buffer, 0, sizeof(buffer));

                            ssize_t bytes_read = recv(fds[i].fd, buffer, BUFFER_SIZE, 0);
                            std::cout << "User " << buffer << " has been logged :D" << std::endl;
                            User new_user(buffer, lobby);
                            lobby.join_channel(buffer, new_user);
                            //Creación de objeto usuario;
                        }
                        else
                        {
                            std::cout << "Se cierra el fd del cliente por contraseña erronea" << std::endl;
                            close(fds[i].fd);
                        }
                        */
                    }
                    else
                    {    
                    }
                }
            }
        }
    }
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
void generate_socket(Server *server,struct sockaddr_in& address)
{
    server->port = 4242;
    //address.sin_port = 4242;
    server->ipv4 = AF_INET; //2
    //address.sin_addr = AF_INET;
    server->socket_type = SOCK_STREAM; //1
    server->server_socket = socket(server->ipv4, server->socket_type, 0);
    if (server->server_socket == -1)
    {
        std::cerr << SOCKET_ERROR << std::endl;
        exit(0);
    }
    else
    {
        //  Añadir preferencias de conexión para que reutilice IPS
        int option = 1;
        //params
        //server fd, nivel de socket , función a utilizar, valor de la función a utilizar, longitud de la función
        while (setsockopt(server->server_socket, SOL_SOCKET,SO_REUSEADDR, &option,sizeof(option)) < 0)
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

