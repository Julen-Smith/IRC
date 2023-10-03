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