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
            this->fds.erase(it);
            return ;
        }
        it++;
    }
}

void    Server::add_unvalidated_user(int client_socket) {
    UnvalidatedUser  *unvalidated_user;
    std::map<int, UnvalidatedUser *>::iterator it;

    unvalidated_user = new UnvalidatedUser(client_socket);

    it = this->unvalidated_users.find(client_socket);

    if (it != this->unvalidated_users.end()) {
        delete it->second;
        this->unvalidated_users.erase(it);
    }

    this->unvalidated_users[client_socket] = unvalidated_user;
}

bool	Server::delete_unvalidated_user(int client_socket) {
    unvalidated_user    user;

    user = this->unvalidated_users.find(client_socket);
    if (user != this->unvalidated_users.end()) {
        delete user->second;
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
    std::vector<Channel *>::iterator ch;
    validated_user  user;
    std::vector<pollfd>::iterator fds;

    user = this->users.begin();
    for (; user != this->users.end(); user++) {
        //Borrar el usuario de los canales donde este

        if ((*user)->get_socket() == client_socket) {

            ch = this->channels.begin();
            for (; ch != this->channels.end(); ch++) {
                if ((*ch)->is_already((*user)->get_nickname()))
                    (*ch)->delete_user((*user)->get_nickname());
            }

            delete (*user);
            this->users.erase(user);
            break ;
        }
    }

    fds = this->fds.begin();
    for (; fds != this->fds.end(); fds++) {

        if ((*fds).fd == client_socket) {
            std::cerr << "Socket(delete_user_by_socket): " << client_socket << " deleted\n";
            this->fds.erase(fds);
            break ;
        }
    }
    std::cout << "Unvalidaed users size: " << this->unvalidated_users.size() << std::endl;
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
    for(size_t i = 0; i != this->fds.size(); i++) {
        if (this->fds[i].fd != msg.client_socket)
            send(this->fds[i].fd,  msg.res.str().c_str(), msg.res.str().size(), 0);
    }
}

Channel    *Server::create_channel(User *user, const std::string &room_name) {
    Channel *channel;

    channel = new Channel(room_name, "There is no topic");
    this->channels.push_back(channel);
    channel->add_user(user);

    std::map<const User*, std::vector<char> >::iterator it;
    it = channel->get_user_permissions()->find(user);
    std::vector<char>& permissions = it->second;
    permissions[2] = '1';

    user->add_channel(channel);
    return channel;
}

bool Server::check_name(const std::string &name) {

    for (size_t i = 0; i < name.size(); i++) {
        if (!isalpha(name[i]) and name[i] != '_')
            return true;
    }
    return false;
}

bool Server::delete_channel(const std::string &name) {
    std::vector<Channel *>::iterator it;

    it = this->channels.begin();

    for (; it != this->channels.end(); it++) {

        if (name == (*it)->get_name()) {
            delete *it;
            this->channels.erase(it);
            return true;
        }
    }
    return false;
}
