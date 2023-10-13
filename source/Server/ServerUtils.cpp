#include "Server.hpp"
#include "defs.hpp"
#include "Channel.hpp"

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

void    Server::erase_client(int socket)
{
    for (std::vector<User *>::iterator it = this->users.begin(); it != this->users.end();)
    {
        if ((*it)->get_socket() == socket)
        {
            std::cout << "Erase client\n";
            delete *it;
            this->users.erase(it);
            return ;
        }
        it++;
    }

    for (std::vector<pollfd>::iterator it = this->fds.begin(); it != this->fds.end();)
    {
        if (it->fd == socket)
        {
            std::cout << "Erase client\n";
            this->fds.erase(it);
            return ;
        }
        it++;
    }
}

void    Server::add_unvalidated_user(int client_socket) {
    UnvalidatedUser  *unvalidated_user;

    unvalidated_user = new UnvalidatedUser(client_socket);
    this->unvalidated_users[client_socket] = unvalidated_user;
}

bool	Server::delete_unvalidated_user(int client_socket) {
    unvalidated_user    user;

    user = this->unvalidated_users.find(client_socket);
    if (user != this->unvalidated_users.end()) {
        this->unvalidated_users.erase(user);
        return false;
    }
    return true;
}

bool    Server::find_unva_user_by_socket(int client_socket) {
    unvalidated_user    user;

    user = this->unvalidated_users.find(client_socket);
    if (user != this->unvalidated_users.end())
        return true;
    return false;
}

User    *Server::get_user_by_nickname(const std::string &nickname) {
    validated_user    user;

    for (user = this->users.begin(); user != this->users.end(); user++) {
        if ((*user)->get_nickname() == nickname)
            return (*user);
    }
    return NULL;
}

bool Server::check_validated_user(Server::validated_user user) {
    if ((*user)->get_notices())
        return USER_ACTIVE;
    return USER_INACTIVE;
}

bool    Server::delete_user_by_socket(int client_socket) {
    validated_user  user;

    user = this->users.begin();
    for (; user != this->users.end(); user++) {
        if ((*user)->get_socket() == client_socket) {
            this->users.erase(user);
            return true;
        }
    }
    return false;
}

User *  Server::get_user_by_socket(int client_socket) {
    Server::validated_user  it;

    for (it = this->users.begin(); it != this->users.end(); it++) {
        if ((*it)->get_socket() == client_socket)
            return (*it);
    }
    return NULL;
}

void    Server::notice_new_user(Message &msg) {

    std::string server_stream;

    //contruir mensaje de usuario nuevo al resto presente

    msg.res.str("");
    msg.res << PRIVMSG << " " << MAIN_CHANNEL << " : " << msg.user->get_nickname() << " " << JOIN_MSG << MSG_END;

    //mandar el mensaje
    for(int i = 0; i != this->fds.size(); i++) {
        if (this->fds[i].fd != msg.client_socket)
            send(this->fds[i].fd, msg.get_res_str(), msg.get_res_size(), 0);
    }
}

Channel    *Server::create_channel(const User *user, const std::string &room_name) {
    Channel *channel;

    channel = new Channel(room_name, "There is no topic"); 
    this->channels.push_back(channel);
    channel->add_user(user);
    std::cout << "New channel created: " << room_name << std::endl;
    return channel;
}
