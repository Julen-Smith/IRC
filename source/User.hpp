#ifndef USER_HPP
#define USER_HPP

#include <iostream>
#include "Server.h"
#include "Channel.hpp"

class Channel;
class User
{
    private:
        char * nickname;
        Channel *rooms;
        std::vector<std::string> *message_list;
    public:
        User();
        User(Channel &channel);
        User(std::string nickname);
        User(char *nickname, Channel &channel);
        ~User();
        User(User& user);
        User &operator=(User &user);
        char * getName(void);
};

#endif