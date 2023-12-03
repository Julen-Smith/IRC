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
    this->callback_map["MOTD"] = &Server::motd_command;
    this->callback_map["INVITE"] = &Server::invite_command;
    this->callback_map["PRIVMSG"] = &Server::prvmsg_command;
    this->callback_map["WHOIS"] = &Server::whois_command;
    this->callback_map["TOPIC"] = &Server::topic_command;
    this->callback_map["KICK"] = &Server::kick_command;
    this->callback_map["CLOSE"] = &Server::close_command;

    this->loop = 1;

    priv_list[0].user = "admin";
    priv_list[0].password = "admin";
    priv_list[0].level = "3";
    priv_list[1].user = "operator";
    priv_list[1].password = "operator";
    priv_list[1].level = "1";

    std::memset(&this->_hints, 0, sizeof(this->_hints));
    this->_hints.ai_family = AF_UNSPEC;
    this->_hints.ai_socktype = SOCK_STREAM;
    this->_hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, port, &this->_hints, &this->_res);
    this->_socket = socket(this->_res->ai_family, this->_res->ai_socktype, this->_res->ai_protocol);

    //this->sv_socket_info.sin_port = htons(4242);
    //this->sv_socket_info.sin_family = AF_INET;
    //this->sv_socket_info.sin_addr.s_addr = INADDR_ANY;
    //this->_socket = socket(this->sv_socket_info.sin_family, SOCK_STREAM, 0);
    if (this->_socket == -1) //this->_socket == -1)
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

    if (bind(this->_socket, this->_res->ai_addr, this->_res->ai_addrlen) == -1) {//->_socket, (sockaddr *) &this->sv_socket_info, sizeof(sv_socket_info)) < 0) {
    //if (bind(this->_socket, (sockaddr *) &this->sv_socket_info, sizeof(sv_socket_info)) < 0) {
        std::cerr << "Error al asignar la dirección al socket\n";
        exit(1);
    }
    listen(this->_socket, MAX_CLIENTS);
    //listen(this->_socket,MAX_CLIENTS);
    _init_cout();
}

//  DESTRUCTOR
Server::~Server() {
    std::map<int, UnvalidatedUser *>::iterator unva_it;
    std::vector<Channel *>::iterator ch_it;
    std::vector<User *>::iterator user_it;

    freeaddrinfo(this->_res);
    unva_it = this->unvalidated_users.begin();
    for (; unva_it != this->unvalidated_users.end(); unva_it++) {
        delete unva_it->second;
    }

    ch_it = this->channels.begin();
    for (; ch_it != this->channels.end(); ch_it++) {
        delete (*ch_it);
    }

    user_it = this->users.begin();
    for (; user_it != this->users.end(); user_it++) {
        delete (*user_it);
    }
}

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

        if (msg.params and msg.params->size() != -1) {
            delete msg.params;
            msg.params = NULL;
        }
        if (msg.holder and msg.holder->size() != -1) {
            delete msg.holder;
            msg.holder = NULL;
        }
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


    if (!inputFile.is_open()) {
        std::cout << "No se pudo abrir el archivo." << std::endl;
        return ;
    }

    client_stream << RPL_MOTDSTART << MOTDSTART;
    while (std::getline(inputFile, line)) {
        client_stream << RPL_MOTD << line << MSG_END;
    }

    client_msg = client_stream.str();
    send(client_socket, client_msg.c_str(), client_msg.size(), 0);
    inputFile.close();
}

bool Server::read_socket(Message &msg) {
    ssize_t read_size;

    //std::cout << "f: " << MSG_DONTWAIT << std::endl;
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

    user = new User(unva_user->second);
    this->users.push_back(user);
    return user;
}

bool    Server::is_already(const std::string &nickname) {
    std::vector<User *>::iterator   it;

    it = this->users.begin();
    for (; it != this->users.end(); it++) {
        std::cout << "NEW: " << nickname << "- OG: " << (*it)->get_nickname() << MSG_END;
        std::cout << "OG: " << (*it)->get_notices() << MSG_END;
        if (nickname == (*it)->get_nickname() and (*it)->get_notices())
            return true;
    }
    return false;
}

void Server::broadcast(Message &msg) {
    std::vector<User *>::iterator it;

    it = this->users.begin();
    for (; it != this->users.end(); it++) {
        if ((*it)->get_nickname() != "Bot")
            send((*it)->get_socket(), msg.get_res_str(), msg.get_res_size(), 0);
    }
}
