#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>
#include <string>

#include "Server.hpp"

class User;
class Channel
{
    private:
        std::string room_name;
      // Server *server;
        std::vector<User *> channel_users;
    public:   
      // Channel(Server *server);
        Channel(const std::string &room_name/* Server *server*/);
        ~Channel();
        Channel(Channel& new_channel);
        Channel& operator=(const Channel &new_channel);
        void Welcome_message() const;
        void join_channel(std::string buffer, User &user);
};


#endif
