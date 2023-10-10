#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <vector>
#include <poll.h>

#include "CreateUser.hpp"

class User
{
    private:
        std::string _nickname;
        bool        _notices;
        int         _socket;

    public:
        //User();
        User(const std::string &, int);
        User(CreateUser *);
        User(const User &);

        ~User();

        User& operator=(User &user);
        const std::string&  get_nickname(void) const;
        bool                get_notices(void) const;
        int                 get_socket(void) const;
        
        void                set_notices(void);
        void                set_nickname(std::string);

        std::string             _login_name;
        std::string             _realname;
        std::string             _level;
};


#endif
