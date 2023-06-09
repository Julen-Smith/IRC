#include <iostream>
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>

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



void    Server::_create_new_user(ssize_t rd_size, int client )
{
    char        *start_nick;
    char        *end_nick;
    size_t      len;
    std::stringstream   string_builder;
    std::string         server_stream;

    //Falta parsear más data
    start_nick = strnstr(this->buffer, NICK, rd_size);
    if (!start_nick)
        return ;
    end_nick = strnstr(start_nick, MSG_END, rd_size);
    if (!end_nick)
        return ;
    len = end_nick - start_nick - 1;
    std::string nickname(start_nick + OFF_NICK, len);
    nickname.erase(remove(nickname.begin(), nickname.end(), '\r'), nickname.end());
    nickname.erase(remove(nickname.begin(), nickname.end(), '\n'), nickname.end());
    nickname.erase(remove(nickname.begin(), nickname.end(), ' '), nickname.end());
    this->users.at(client)->set_nickname(nickname);
    string_builder << PRIVMSG << " " << MAIN_CHANNEL << " : " << nickname << " " << JOIN_MSG << MSG_END;
    server_stream = string_builder.str();
    for(int cli = 0; cli != this->users.size(); cli++)
        if (cli != client)
            send(this->fds[client].fd, server_stream.c_str(),server_stream.size(), 0);
}

void    Server::enter_msg(int client)
{
    ssize_t             rd_size;
    std::string         client_msg;
    std::stringstream   client_stream;

    this->users[client]->set_notices();
    rd_size = recv(this->fds[client].fd, this->buffer, BUFFER_SIZE, 0);
    this->_create_new_user(rd_size,client);
    if (rd_size == -1)
        exit(121);
    this->buffer[rd_size] = 0;

    std::ifstream inputFile("inc/Server.info");
    if (inputFile.is_open()) {
        std::string line;
        while (std::getline(inputFile, line)) {
            client_stream << PRIVMSG << " " << MAIN_CHANNEL << " : " << line << MSG_END;
            client_msg = client_stream.str();
            rd_size = send(this->fds[client].fd, client_msg.c_str(), client_msg.size(), 0);
            client_stream.str("");
            client_msg = "";
        }
        inputFile.close();
    }
    else
        std::cout << "No se pudo abrir el archivo." << std::endl;
}


void    Server::send_msg(int client)
{
    ssize_t             rd_size;
    std::stringstream   client_stream;
    std::string         client_msg;


    rd_size = recv(this->fds[client].fd, this->buffer, BUFFER_SIZE, 0);
    if (rd_size == -1)
        return ;
    this->buffer[rd_size] = 0;
    std::string compare(this->buffer);
    std::cout << "BUFF: " << compare << std::endl;
    int opt = command_checker(compare); 
    std::string respuesta1 = ":Server 321 : Channels Users Topic \r\n";  
    switch(opt)
    {
        case 1:            
            send(fds[client].fd,respuesta1.c_str(),respuesta1.size(),0);
            for (int channels = 0; channels != this->channels.size(); channels++)
            {
                std::string response;
                std::string message = ":Server 322 ";
                message += this->users.at(client)->get_nickname() + " ";
                message += this->channels.at(channels)->get_name() + " ";
                message += std::to_string(this->channels.at(channels)->get_users_size()) + " ";
                message += this->channels.at(channels)->get_topic() + MSG_END;
                response.append(message);
                std::cout.width(response.size());
                std::cout << response << std::endl;
                send(fds[client].fd,response.c_str(),response.size(),0); 
            }
        break;
       /* case 2:
            respuesta1 = :Server
                    353     RPL_NAMREPLY "<canal> :[[@|+]<nick> [[@|+]<nick> [...]]]"
                    366     RPL_ENDOFNAMES "<canal> :Fin de la lista /NAMES"   
        */
        default: 
            std::cout << "Comando no encontrado option : "<< opt << std::endl; 
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
    new_user = new User("ANON", client_socket);
    this->users.push_back(new_user);

    new_pollfd.fd = client_socket;
    new_pollfd.events = POLLIN;
    this->fds.push_back(new_pollfd);
}

void    Server::erase_client(int socket)
{
    for (std::vector<User *>::iterator it = this->users.begin(); it != this->users.end();)
    {
        if ((*it)->get_socket() == socket)
        {
            delete *it;
            this->users.erase(it);
            return ;
        }
        it++;
    }
}

void Server::generate_default_channels()
{
    Channel * lobby = new Channel("#Lobby","Sala principal del servidor");
    Channel * lukas = new Channel("#La_Guarida_de_Lukas","El mismo la ha denominado asi quien sabe que podrias encontrarte dentro.");
    Channel * test_room = new Channel("#Test_Room","Todas las salas son de prueba, pero ponerle un nombre a una hace que lo sea mas.");
    Channel * AAAA = new Channel("#AAAA","Asii es, AAAA.");

    this->channels.push_back(lobby);
    this->channels.push_back(lukas);
    this->channels.push_back(AAAA);
    this->channels.push_back(test_room);
}

int Server::command_checker(std::string &cmd)
{
    std::string commands[4]=
    {
        "INVITE",
        "NAMES",
        "MODE",
        "LIST"
    };
    cmd.erase(remove(cmd.begin(), cmd.end(), '\r'), cmd.end());
    cmd.erase(remove(cmd.begin(), cmd.end(), '\n'), cmd.end());
    cmd.erase(remove(cmd.begin(), cmd.end(), ' '), cmd.end());

    for(int counter = 0; counter < 4; counter++)
    {
        if (commands[counter] == cmd)
            return (1);
      //  if (commands[counter] == "NAMES")
      //      return (2);
    }
    return (0);
}

