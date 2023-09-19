#include <stdlib.h>
#include "Server.hpp"
#include "defs.hpp"
#include "Channel.hpp"


Server::Server(const char *port) : max_clients(MAX_CLIENTS), _port(port)
{

   this->sv_socket_info.sin_port = htons(4242);
   this->sv_socket_info.sin_family = AF_INET; 
   this->sv_socket_info.sin_addr.s_addr = INADDR_ANY; 
   this->_socket = socket(this->sv_socket_info.sin_family, SOCK_STREAM, 0);
    if (this->_socket == -1)
    {
        std::cout << SOCKET_ERROR << std::endl;
        exit(1);
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
        exit(1);
    }
    listen(this->_socket,MAX_CLIENTS);
    _init_cout();
}

const int	Server::get_socket() const {return this->_socket;}
Server::~Server() {}

void    Server::enter_msg(int clientIndex)
{
    char                buffer[BUFFER_SIZE];
    ssize_t             rd_size;
    std::string         client_msg;
    std::stringstream   client_stream;
    std::string         line;

    this->users[clientIndex]->set_notices();

    //Recibir datos del usuario y crear una nueva instancia
    rd_size = recv(this->fds[clientIndex].fd, buffer, BUFFER_SIZE, 0);
    this->_create_new_user(rd_size, clientIndex, buffer);

    if (rd_size == -1)
        exit(121);
    this->buffer[rd_size] = 0;
    std::ifstream inputFile("inc/Server.info");
    if (inputFile.is_open()) {
        while (std::getline(inputFile, line)) {
            client_stream << PRIVMSG << " " << MAIN_CHANNEL << " : " << line << MSG_END;
            client_msg = client_stream.str();
            rd_size = send(this->fds[clientIndex].fd, client_msg.c_str(), client_msg.size(), 0);
            client_stream.str("");
            client_msg = "";
        }
        inputFile.close();
    }
    else
        std::cout << "No se pudo abrir el archivo." << std::endl;
}

