#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>
#include <string>

#include "Message.hpp"
#include "Server.hpp"

class Channel
{
    private:
        std::string                 _name;
        std::string                 _topic;
        std::string                 _key;
        std::vector<User *>         _users;
        std::vector<User *>         _visible_users;
        std::vector<std::string>    _banned;

        std::map<const User*,  std::vector<char> > _user_permissions; //iwsovq
        std::vector<bool>           _channel_permissions; //tklbm

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

        const std::string                           &get_key() const;
        bool                                        get_invite() const;
        const std::string&                          get_name() const;
        const std::string&                          get_topic() const;
        std::string                                 get_user_list() const;
        std::string                                 get_visible_user_list() const;
        int                                         get_users_size() const;
        std::string                                 get_permissions_to_string();
        std::vector<User *>                         get_users();
        std::vector<User *>                         get_visible_users();
        std::map<const User*,  std::vector<char> > *get_user_permissions();

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
        void                send_msg(Message &msg);
        void                notice_join(Message &msg);
        void                notice_part(Message &msg, const std::string &topic);


        //Debug and stuff
        void                stdout_channel_permissions();
        void                stdout_channel__users_permissions(const User *user);
       
};


#endif