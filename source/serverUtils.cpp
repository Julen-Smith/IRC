#include "Server.h"

/** @attention Main @param Argc,Port,Password @return exit mode */
void print_server_menu(Server *server)
{
        std::cerr << "El servidor ha sido iniciado ... \n"
        << " _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _\n" 
        << "| Puerto : " 
        << server->port << "                                 |" 
        << std::endl
        << "| Protocolo : " 
        << (server->ipv4 == 2 ? "Ipv4" : server->ipv4 == 1 ? "Ipv6" : "Uknown")
        << "                              |" 
        << std::endl
        << "| Tipo de conexión : " << (server->socket_type == 1 ? "Socket de conexiones" : "Uknown")
        << "       |"
        << std::endl 
        << " ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯\n";
}