#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include "Server.h"


class Channel
{
    private:
        std::string room_name;
        Server *server;
    public:
        Channel(Server *server);
        Channel(std::string room_name, Server *server);
        ~Channel();
        Channel(Channel& new_channel);
        Channel& operator=(Channel &new_channel);
        void Welcome_message();

};


#endif