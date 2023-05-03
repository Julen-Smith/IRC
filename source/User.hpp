#ifndef USER_HPP
#define USER_HPP

#include <iostream>
#include "Server.h"
#include "Channel.hpp"

class Channel;
class User
{
    private:
        std::string nickname;
        Channel *rooms;
        std::vector<std::string> *message_list;
    public:
        User();
        User(Channel &channel);
        User(std::string nickname);
        ~User();
        User(User& user);
        User &operator=(User &user);
        std::string& getName(void);
};

#endif