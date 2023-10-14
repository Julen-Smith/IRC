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
        std::string                 _key;
        std::vector<User *>   _users;
        std::vector<std::string>    _banned;
        bool                        _invite;
        int                         _user_limit;
        bool                        _key_opt;

    public:

        Channel(const std::string &, const std::string &);
        Channel(Channel& new_channel);
        Channel();
        ~Channel();

        Channel& operator=(const Channel &new_channel);

        //setters
        void                set_user_limit(int user_limit);
        void                set_key(const std::string &key);
        void                set_invite(const bool invite);
        void                set_user_ban(const std::string &nickname);
        void                unset_user_ban(const std::string &nickname);

        //getters

        const std::string   &get_key() const;
        bool                get_invite() const;
        const std::string&  get_name() const;
        const std::string&  get_topic() const;
        std::string         get_user_list() const;
        int                 get_users_size() const;

        //is functions

        bool                is_limit_raised() const;
        bool                is_banned(const std::string &nickname);
        bool                is_already(const std::string &nickname);

        std::string         get_user_list_msg(User *user);
        std::string         get_topic_msg(User *user);
        void                welcome_msg() const;
        void                join_channel(std::string buffer, User &user);
        void                add_user(User *);
        void                delete_user(const std::string &);
        bool                enter_key(const std::string &key);
};


#endif