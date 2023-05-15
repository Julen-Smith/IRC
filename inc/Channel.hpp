#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>
#include <string>

#include "Server.hpp"

class User;

class Channel
{
    private:
        const Server        &server;
        std::string         room_name;
        std::vector<User *>   channel_users;
    public:
        Channel(const Server &server);
        Channel(const std::string& room_name, const Server &server);

        ~Channel();
        Channel(Channel& new_channel);

        Channel& operator=(const Channel &new_channel);
        void Welcome_message() const;
        void join_channel(std::string buffer);
};


#endif
