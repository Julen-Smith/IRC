#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Server.h"
#include <vector>
#include <string>

class User;

class Channel
{
    private:

        std::string         room_name;
        Server              *server;
        std::vector<User *> channel_users;

    public:

        Channel();
        Channel(Server *);
        Channel(Channel&);
        Channel(std::string, Server *);

        ~Channel();

        Channel& operator=(Channel &);
        void Welcome_message();
        void join_channel(std::string);
};

#endif
