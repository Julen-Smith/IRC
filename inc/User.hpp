#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <vector>
#include <poll.h>

#include "UnvalidatedUser.hpp"

class User
{
    private:
        std::string _nickname;
        bool        _notices;
        int         _socket;
        size_t      _count;

    public:
        //User();
        User(const std::string &, int);
        User(UnvalidatedUser *);
        User(User &);

        ~User();

        User& operator=(User &user);
        const std::string&  get_nickname(void) const;
        const std::string&  get_login_name(void) const;
        bool                get_notices(void) const;
        int                 get_socket(void) const;
        size_t              get_count(void) const;
        
        void                set_notices(void);
        void                set_nickname(std::string);
        void                add_channel_count();
        void                substract_channel_count();

        std::string             _login_name;
        std::string             _realname;
        std::string             _level;
};


#endif
