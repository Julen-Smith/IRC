#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Server.h"
#include <vector>
#include <string>

class User;

class Channel
{
    private:
        std::string room_name;
        Server *server;
        std::vector<User> channel_users;
    public:
        Channel(Server *server);
        Channel(std::string room_name, Server *server);
        ~Channel();
        Channel(Channel& new_channel);
        Channel& operator=(Channel &new_channel);
        void Welcome_message();
        void join_channel(std::string buffer);
};


#endif
