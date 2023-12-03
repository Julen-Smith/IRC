#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <vector>
#include <poll.h>
#include <ctime>

#include "UnvalidatedUser.hpp"
class Channel;

class User
{
    private:
        std::string _nickname;
        bool        _notices;
        int         _socket;
        size_t      _count;
        bool        _is_operator;

    public:
        //User();
        User(const std::string &nickname, int socket);
        User(UnvalidatedUser *);
        User(User &);

        ~User();
        std::vector<Channel *>  _channels;

        User& operator=(User &user);
        std::string         get_channels_str();
        const std::string&  get_nickname(void) const;
        const std::string&  get_login_name(void) const;
        bool                get_notices(void) const;
        int                 get_socket(void) const;
        size_t              get_count(void) const;
        bool                get_operator_status() const;

        void                set_notices(bool status);
        void                set_nickname(std::string);
        void                set_operator_status(bool status);
        void                add_channel(Channel *channel);
        void                add_channel_count();
        void                substract_channel_count();
        void                delete_channel(Channel *channel);

        std::string             _login_name;
        std::string             _realname;
        std::string             _level;
};


#endif
