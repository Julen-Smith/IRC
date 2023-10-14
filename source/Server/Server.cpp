#include <stdlib.h>
#include "Server.hpp"
#include "defs.hpp"
#include "Channel.hpp"


Server::Server(const char *port) : max_clients(MAX_CLIENTS), _port(port)
{
    this->callback_map["NICK"] = &Server::nick_command;
    this->callback_map["USER"] = &Server::user_command;
    this->callback_map["LIST"] = &Server::list_command;
    this->callback_map["JOIN"] = &Server::join_command;
    this->callback_map["OPER"] = &Server::oper_command;
    this->callback_map["MODE"] = &Server::mode_command;
    this->callback_map["QUIT"] = &Server::quit_command;
    this->callback_map["PART"] = &Server::part_command;

   priv_list[0].user = "admin";
   priv_list[0].password = "admin";
   priv_list[0].level = "3";
   priv_list[1].user = "operator";
   priv_list[1].password = "operator";
   priv_list[1].level = "1";

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

//  DESTRUCTOR
Server::~Server() {}

// Tokenizer acepta como parametro un buffer de chars, utilizando getline 
void Server::tokenizer(Message &msg) {

    Message::command  command;
    std::string       token;
    
    msg.set_commands();
    for (command = msg.commands->begin(); command != msg.commands->end(); command++) {
        msg.set_params();
        token = msg.get_params_front();
        this->it = this->callback_map.find(token);

        if (this->it != this->callback_map.end())
            (this->*(it->second))(msg);
        else
            std::cerr << "Error: invalid commnad -> " << token << std::endl;

        delete msg.params;

    }
}

const int	Server::get_socket() const {return this->_socket;}

void Server::send_intro(int client_socket) {

    std::ifstream       inputFile("inc/Server.info");
    std::string         line;
    std::string         client_msg;
    std::stringstream   client_stream;
    ssize_t             rd_size;

    if (inputFile.is_open()) {
        while (std::getline(inputFile, line)) {
            client_stream << PRIVMSG << " " << MAIN_CHANNEL << " : " << line << MSG_END;
            client_msg = client_stream.str();
            rd_size = send(client_socket, client_msg.c_str(), client_msg.size(), 0);
            client_stream.str("");
            client_msg = "";
        }
        inputFile.close();
    }
    else
        std::cout << "No se pudo abrir el archivo." << std::endl;
}

bool Server::read_socket(Message &msg) {
    ssize_t read_size;

    read_size = recv(msg.client_socket, msg.buffer, BUFFER_SIZE, 0);

    //TODO handleear cuando read_size es 0
    if (read_size == -1) {
        std::cerr << "Error: read socket failed\n";
        return true;
    }
    msg.buffer[read_size] = 0;
    return false;
}

User    *Server::add_validated_user(Server::unvalidated_user unva_user) {
    User    *user;

    user = new User(unva_user->second);
    this->users.push_back(user);
    return user;
}