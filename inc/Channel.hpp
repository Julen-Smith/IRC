#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>
#include <string>

#include "Server.hpp"

class Channel
{
    private:
        std::string                 _name;
        std::string                 _topic;
        std::vector<const User *>   _users;

    public:
        Channel(const std::string &, const std::string &);
        Channel(Channel& new_channel);
        Channel();
        ~Channel();

        Channel& operator=(const Channel &new_channel);

        const std::string& get_name() const;
        const std::string& get_topic() const;
        int     get_users_size() const;
        void    welcome_msg() const;
        void    join_channel(std::string buffer, User &user);
        void    add_user(const User *);
        void    delete_user(const std::string &);
};


#endif