#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <vector>

#include "Server.h"

class Channel;

class User
{
    private:

        std::string                 nickname;
        Channel                     *rooms;
        std::vector<std::string>    *message_list;

    protected:

    public:
        User();
        User(User&);
        User(Channel&);
        User(std::string);

        ~User();

        User &operator=(User&);
        const std::string& getName() const;
};

#endif
