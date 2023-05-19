#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <vector>

#include "Server.hpp"

class Channel;
class User
{
    private:
        const std::string   _nickname;
        const pollfd        _fd;
        bool                notices;              

    public:
        User(std::string, pollfd);
        User(const User &);

        ~User();

        User& operator=(User &user);
        const std::string& getName(void) const;
};


#endif
