#include <stdlib.h>
#include "Server.hpp"
#include "defs.hpp"
#include "Channel.hpp"


Server::Server(const char *port, const char *password): max_clients(MAX_CLIENTS), _port(port), _password(password)
{
    this->callback_map["NICK"] = &Server::nick_command;
    this->callback_map["USER"] = &Server::user_command;
    this->callback_map["LIST"] = &Server::list_command;
    this->callback_map["JOIN"] = &Server::join_command;
    this->callback_map["OPER"] = &Server::oper_command;
    this->callback_map["MODE"] = &Server::mode_command;
    this->callback_map["QUIT"] = &Server::quit_command;
    this->callback_map["PART"] = &Server::part_command;
    this->callback_map["PING"] = &Server::ping_command;
    this->callback_map["PASS"] = &Server::pass_command;
    this->callback_map["PRIVMSG"] = &Server::prvmsg_command;

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
    if (msg.commands == NULL)
        return ;
    if (msg.commands->size() == 0) {
        delete msg.commands;
        return ;
    }

    std::cout << "RAW msg: " << msg.buffer << std::endl;
    for (command = msg.commands->begin(); command != msg.commands->end(); command++) {
        msg.set_params();
        token = msg.get_params_front();
        this->it = this->callback_map.find(token);

        if (this->it != this->callback_map.end()) {
            std::cout << "Valid command -> " << token << std::endl;
            (this->*(it->second))(msg);
        }
        else {
            std::cerr << "Contents: " << msg.buffer << std::endl;
            std::cerr << "Error: invalid commnad -> " << token << std::endl;
        }
        if (msg.params->size() != -1)
            delete msg.params;
    }
    delete msg.commands;
}

const int	Server::get_socket() const {return this->_socket;}

void Server::send_intro(int client_socket) {

    std::ifstream       inputFile("inc/Server.info");
    std::string         line;
    std::string         client_msg;
    std::stringstream   client_stream;
    //ssize_t             rd_size;

    
    if (inputFile.is_open()) {
        while (std::getline(inputFile, line)) {
            client_stream << PRIVMSG << " " << MAIN_CHANNEL << " : " << line << MSG_END;
            client_msg = client_stream.str();
            send(client_socket, client_msg.c_str(), client_msg.size(), 0);
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
    std::cout << "read size: " << read_size << std::endl;

    //TODO handleear cuando read_size es 0
    if (read_size < 1) {
        this->delete_unvalidated_user(msg.client_socket);
        this->delete_user_by_socket(msg.client_socket);
        std::cerr << "Error: read socket failed\n";
        close(msg.client_socket);
        return true;
    }
    msg.buffer[read_size] = 0;
    return false;
}

User    *Server::add_validated_user(Server::unvalidated_user unva_user) {
    User    *user;

    user = new User(unva_user->second, this->_curr_time);
    this->users.push_back(user);
    return user;
}

void    Server::check_users() {
    validated_user  it;

    return ;
    it = this->users.begin();
    for (; it != this->users.end(); it++) {
        if (((this->_curr_time - (*it)->get_curr_time()) / 1000000) > 180 and (*it)->get_notices() == CONNECTED) {
            std::cout << "User: " << (*it)->get_nickname() << " deleted for inactivity\n";
            close((*it)->get_socket());
            (*it)->set_notices(false);
        }
    }
}

void	Server::set_curr_time(time_t curr_time) {this->_curr_time = curr_time;};
time_t  Server::get_curr_time() const {return this->_curr_time;};