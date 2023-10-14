#include "Server.hpp"





void    Server::mode_command(Message &msg)
{
    msg.holder = msg.split(msg.buffer," ");
    // Segundo parametro siempre es "+-iwso"
    

}
