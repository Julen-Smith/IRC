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

bool Server::contains(std::string& ref, const char *check)
{
    size_t found = 0;

    if (!ref.empty())
    {
        ref = response_cleaner(ref);
        found = ref.find(check);
        if (found != std::string::npos)
            return true;
    }
    return (false);
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
}



