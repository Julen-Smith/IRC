#include "Server.hpp"
#include "defs.hpp"
#include "Channel.hpp"
#include "defs.hpp"

static std::string  parse_string(std::string source, size_t start, size_t end) {
    return source.substr(start, end);
}

//Borra el sufijo pasado como comando
//source -> string origen.
//to_erase -> string key, con los carcateres a borrar.
static bool erase_back_match(std::string &source, const std::string &to_erase) {

    size_t  erase_len = 0;
    size_t  source_len = source.size();
    
    for (int i = 0; i < to_erase.size(); i++) {
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

void    Server::_create_new_user(ssize_t rd_size, int clientIndex, std::string buffer)
{
    std::stringstream   string_builder;
    std::string         server_stream;

    //parsear el nombre del usuario
    std::string nickname = parse_string(buffer, buffer.find(NICK), buffer.find(MSG_END));
    erase_back_match(nickname, MSG_END_SPACE);

    //asignar nombre de usuario
    this->users[clientIndex]->set_nickname(nickname);

    //contruir mensaje de usuario nuevo al resto presente
    string_builder << PRIVMSG << " " << MAIN_CHANNEL << " : " << nickname << " " << JOIN_MSG << MSG_END;
    server_stream = string_builder.str();

    //mandar el mensaje
    for(int cli = 0; cli != this->users.size(); cli++) {
        if (cli != clientIndex)
            send(this->fds[clientIndex].fd, server_stream.c_str(),server_stream.size(), 0);
    }
}

void    Server::accept_new_user()
{
    int         client_socket;
    sockaddr_in client;
    socklen_t   size;
    pollfd      new_pollfd;

    client_socket = accept(this->_socket, (sockaddr *)&client, &size);
    this->add_unvalidated_user(client_socket);

    new_pollfd.fd = client_socket;
    new_pollfd.events = POLLIN;
    this->fds.push_back(new_pollfd);
}
