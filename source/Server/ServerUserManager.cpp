#include "Server.hpp"
#include "defs.hpp"
#include "Channel.hpp"
#include "defs.hpp"

void    Server::accept_new_user()
{
    struct sockaddr_storage their_addr;
    socklen_t               addr_size;
    int                     client_socket;
    pollfd                  new_pollfd;

    addr_size = sizeof(their_addr);
    client_socket = accept(this->_socket, (struct sockaddr *)&their_addr, &addr_size);
    if (client_socket == -1) {

        std::cerr << "Socket failed\n";
        return ;
    }
    this->add_unvalidated_user(client_socket);
    std::cerr << "Unvalidated users size(accept new user): " << this->unvalidated_users.size() << std::endl;

    new_pollfd.fd = client_socket;
    new_pollfd.events = POLLIN;
    new_pollfd.revents = 1;
    this->fds.push_back(new_pollfd);
    std::cerr << "fds size: " << this->fds.size() << std::endl;
}
