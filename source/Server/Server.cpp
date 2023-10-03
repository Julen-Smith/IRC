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

//Borra el sufijo pasado como comando
//source -> string origen.
//to_erase -> string key, con los carcateres a borrar.
void Server::erase_match(std::string &source, const std::string &to_erase) {
    std::string::reverse_iterator    it;
    
    for (it = source.rbegin(); it != source.rend(); it++) {
        if (to_erase.find(*it) != -1)
            source.pop_back();
    }
}

# define SPACE ' '

std::istream&    Server::get_token(
    std::stringstream &key_stream,
    std::string &token,
    char delimiter,
    const std::string &to_erase) {

    std::istream &res = std::getline(key_stream, token, delimiter);
    if (res)
        erase_match(token, to_erase);
    return res;
}

// Tokenizer acepta como parametro un buffer de chars, utilizando getline 
void Server::tokenizer(int user_index, const char *buffer) {

    std::stringstream   input(buffer);
    std::string         token;

    this->it = this->callback_map.end();
    while (get_token(input, token, SPACE, MSG_END_SPACE)) {
        this->it = this->callback_map.find(token);

        if (this->it != this->callback_map.end())
            (this->*(it->second))(input, 1);
    }
}

const int	Server::get_socket() const {return this->_socket;}

void    Server::user_command(std::stringstream &key_stream, int client_index) {
    std::string login;
    std::string tmp;
    std::string realname;

    get_token(key_stream, login, SPACE, MSG_END_SPACE);
    get_token(key_stream, tmp, SPACE, MSG_END_SPACE);
    get_token(key_stream, tmp, SPACE, MSG_END_SPACE);
    get_token(key_stream, realname, SPACE, MSG_END_SPACE);
    std::cout << "User command\n" << " - login: " << login << "\n - realname: " << realname << std::endl;
}

void    Server::nick_command(std::stringstream &key_stream, int client_index) {
    std::stringstream   string_builder;
    std::string         server_stream;
    std::string         nickname;

    //asignar nombre de usuario
    get_token(key_stream, nickname, SPACE, MSG_END_SPACE);
    std::cout << "Nick command\n" << " - nickname: " << nickname << std::endl;
    this->users[client_index]->set_nickname(nickname);

    //contruir mensaje de usuario nuevo al resto presente
    string_builder << PRIVMSG << " " << MAIN_CHANNEL << " : " << nickname << " " << JOIN_MSG << MSG_END;
    server_stream = string_builder.str();

    //mandar el mensaje
    for(int cli = 0; cli != this->users.size(); cli++) {
        if (cli != client_index)
            send(this->fds[client_index].fd, server_stream.c_str(),server_stream.size(), 0);
    }
}

void Server::send_intro(int client_index) {

    std::ifstream       inputFile("inc/Server.info");
    std::string         line;
    std::string         client_msg;
    std::stringstream   client_stream;
    ssize_t             rd_size;

    if (inputFile.is_open()) {
        while (std::getline(inputFile, line)) {
            client_stream << PRIVMSG << " " << MAIN_CHANNEL << " : " << line << MSG_END;
            client_msg = client_stream.str();
            rd_size = send(this->fds[client_index].fd, client_msg.c_str(), client_msg.size(), 0);
            client_stream.str("");
            client_msg = "";
        }
        inputFile.close();
    }
    else
        std::cout << "No se pudo abrir el archivo." << std::endl;
}

bool Server::read_socket(int client_index, char buffer[BUFFER_SIZE]) {
    ssize_t read_size;

    read_size = recv(this->fds[client_index].fd, buffer, BUFFER_SIZE, 0);
    if (read_size == -1)
        return true;
    this->buffer[read_size] = 0;
    return false;
}

void    Server::enter_msg(int client_index)
{
    char                buffer[BUFFER_SIZE];

    this->users[client_index]->set_notices();
    //Recibir datos del usuario y crear una nueva instancia
    if (this->read_socket(client_index, buffer))
        exit(121);
    this->tokenizer(client_index, buffer);
    this->send_intro(client_index);
}