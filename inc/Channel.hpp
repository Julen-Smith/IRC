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
        Channel(const Server&);
        Channel(const std::string&, const Server&);

        ~Channel();
        Channel(Channel&);

        Channel& operator=(const Channel&);
        void Welcome_message() const;
        void join_channel(std::string, User&);
};


#endif
