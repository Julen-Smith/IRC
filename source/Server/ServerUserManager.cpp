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

    for (size_t i = 0; i < to_erase.size(); i++) {
        if (source.find(to_erase[i]) != static_cast<size_t>(-1))
            erase_len++;
    }
    if (erase_len)
    {
        source.erase(source_len - erase_len, source_len);
        return true;
    }
    return false;
}

void    Server::_create_new_user(ssize_t rd_size, size_t clientIndex, std::string buffer)
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
    for(size_t cli = 0; cli != this->users.size(); cli++) {
        if (cli != clientIndex)
            send(this->fds[clientIndex].fd, server_stream.c_str(),server_stream.size(), 0);
    }
}

void    Server::accept_new_user()
{
    struct sockaddr_storage their_addr;
    socklen_t               addr_size;
    int                     client_socket;
    pollfd                  new_pollfd;

    addr_size = sizeof(their_addr);
    client_socket = accept(this->_socket, (struct sockaddr *)&their_addr, &addr_size);
    if (client_socket == -1)
    {
        std::cerr << "Failed to accept client (" << this->_socket << "): " << strerror(errno) << "\n";
        std::cout << "Bad client socket\n";
        return ;
    }
    this->add_unvalidated_user(client_socket);

    new_pollfd.fd = client_socket;
    new_pollfd.events = POLLIN;
    new_pollfd.revents = 1;
    this->fds.push_back(new_pollfd);
}
