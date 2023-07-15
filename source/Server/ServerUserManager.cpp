#include "Server.hpp"
#include "defs.hpp"
#include "Channel.hpp"

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