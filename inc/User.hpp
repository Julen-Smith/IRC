#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <vector>

#include "Server.hpp"

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
        User(const std::string &nickname, Channel &channel);
        ~User();
        User(const User &obj);
        User& operator=(User &user);
        const std::string& getName(void) const;
};


#endif
