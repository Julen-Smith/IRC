#include <stdlib.h>
#include "Server.hpp"
#include "defs.hpp"
#include "Channel.hpp"


Server::Server(const char *port) : max_clients(MAX_CLIENTS), _port(port)
{
    this->callback_map["NICK"] = &Server::test_create_new_user;

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
static bool erase_back_match(std::string &source, const std::string &to_erase) {

    size_t  erase_len = 0;
    size_t  source_len = source.size();
    
    //for (int i = 0; i < to_erase.size(); i++) {
    while (1) {
        if (source.find(to_erase[i]) != -1)
            erase_len++;
    }
    if (erase_len)
    {
        source.erase(source_len - erase_len, source_len);
        return true;
    }
    return false;        
}

# define SPACE ' '

// Tokenizer acepta como parametro un buffer de chars, utilizando getline 
void Server::tokenizer(const char *buffer, int user_index) {

    std::stringstream   input(buffer);
    std::string         token;

    std::cout << "Tokenizer\n";
    this->it = this->callback_map.end();
    while (std::getline(input, token, SPACE)) {
        erase_back_match(token, MSG_END_SPACE);
        if (this->it != this->callback_map.end())
            (this->*(it->second))(token, 1);

        this->it = this->callback_map.find(token);
    }
}

const int	Server::get_socket() const {return this->_socket;}

void    Server::test_create_new_user(std::string nickname, int clientIndex) {
    std::stringstream   string_builder;
    std::string         server_stream;

    //asignar nombre de usuario
    this->users[clientIndex]->set_nickname(nickname);
    std::cout << "test create new user: " << nickname << std::endl;

    //contruir mensaje de usuario nuevo al resto presente
    string_builder << PRIVMSG << " " << MAIN_CHANNEL << " : " << nickname << " " << JOIN_MSG << MSG_END;
    server_stream = string_builder.str();

    //mandar el mensaje
    for(int cli = 0; cli != this->users.size(); cli++) {
        if (cli != clientIndex)
            send(this->fds[clientIndex].fd, server_stream.c_str(),server_stream.size(), 0);
    }
}

void Server::send_intro(ssize_t rd_size, int client_index) {

    std::ifstream       inputFile("inc/Server.info");
    std::string         line;
    std::string         client_msg;
    std::stringstream   client_stream;

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

void    Server::enter_msg(int client_index)
{
    char                buffer[BUFFER_SIZE];
    ssize_t             rd_size;
    std::string         line;

    this->users[client_index]->set_notices();

    //Recibir datos del usuario y crear una nueva instancia
    rd_size = recv(this->fds[client_index].fd, buffer, BUFFER_SIZE, 0);
    if (rd_size == -1)
        exit(121);

    this->buffer[rd_size] = 0;
    tokenizer(buffer, client_index);
    send_intro(rd_size, client_index);
}

