#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <vector>

#include "Server.hpp"

class Channel;

class User
{
    private:

        std::string                 nickname;
        Channel                     &rooms;
        std::vector<std::string>    message_list;

    protected:

    public:

        User(const std::string&, Channel&);
        User(const User&);

        ~User();

        User &operator=(User&);
        const std::string& getName() const;
};

#endif
