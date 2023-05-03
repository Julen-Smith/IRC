#ifndef USER_HPP
#define USER_HPP

#include "Server.h"

class User
{
    private:
        std::string nickname;
        Channel *rooms;
        std::vector<std::string> *message_list;
    public:
        User();
        User(User& user);
        User(Channel &channel);
        User(std::string nickname);
        ~User();
        User &operator=(User &user);
        std::string& getName(void);
};

#endif
