#include <iostream>

#include "Channel.hpp"
#include "User.hpp"

void    Channel::send_msg(Message &msg) {
    std::stringstream body;
    std::vector<User *>::iterator   it;
    std::string nickname;
    int client_socket;


    it = this->_users.begin();
    for (size_t i = 0; i < msg.params->size(); i++) {
        body << (*msg.params)[i] << " ";
    }

    body << MSG_END;
    nickname = msg.user->get_nickname();

    for (; it != this->_users.end(); it++) {
        msg.res.str("");
        client_socket = (*it)->get_socket();
        msg.res << ":" << nickname << " PRIVMSG " << this->_name << " " << body.str();
        if (client_socket != msg.client_socket)
            send(client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);   
    }
}

Channel::Channel(const std::string& name, const std::string &topic)
: _name(name), _topic(topic), _invite(false), _user_limit(STANDARD_LIMIT), _key_opt(KEY_NOT_SET)
{
    for (size_t i = 0; i != 6; i++) _channel_permissions.push_back(false);
}

Channel::~Channel()
{
}

Channel& Channel::operator=(const Channel &new_channel)
{
    (void)new_channel;
    return *this;
}

Channel::Channel(){};

std::string Channel::get_user_list() {
    std::string user_list = "";
    std::map<const User*, std::vector<char> >::iterator it;

    for (size_t i = 0; i < this->_users.size(); i++) {

        it = this->get_user_permissions()->find(this->_users.at(i));
        std::vector<char>& permissions = it->second;
        if (permissions[2] == '1')
            user_list += "@";
        user_list += this->_users.at(i)->get_nickname() + " ";
    }
    return (user_list);
}

void    Channel::add_user(User *user)
{
    std::vector<User *>::iterator  inv_user;

    user->add_channel_count();

    std::vector<char> permissions(6, '0');
    this->_user_permissions[user] = permissions;
    if (!this->_users.size())
        this->_user_permissions[user][2] = '1';
    this->_users.push_back(user);
    for(inv_user= this->_invited_users.begin(); inv_user != this->_invited_users.end(); inv_user++)
    {
        if ((*inv_user) == user) {
            this->_invited_users.erase(inv_user);
            break ;
        }
    }

}

void    Channel::delete_user(const std::string &name)
{
    for (std::vector<User *>::iterator it = this->_users.begin(); it != this->_users.end();)
    {
        if ((*it)->get_nickname() == name)
        {
            (*it)->substract_channel_count();
            this->_users.erase(it);
            return ;
        }
        it++;
    }
}

const std::string &Channel::get_key() const { return this->_key; }
bool    Channel::get_invite() const { return this->_invite; }
const std::string&  Channel::get_name() const { return (this->_name );}
const std::string&  Channel::get_topic() const { return (this->_topic );}
size_t Channel::get_users_size() const { return (this->_users.size()); }
std::vector<User *> Channel::get_invited_users(){return this->_invited_users;}

//is

bool    Channel::is_banned(const std::string &nickname) {
    std::vector<std::string>::iterator it;

    for (it = this->_banned.begin(); it != this->_banned.end(); it++) {
        if (*it == nickname)
            return FOUND;
    }
    return NOT_FOUND;
}

bool    Channel::is_limit_raised() const {
    if (this->_users.size() >= this->_user_limit)
        return LIMIT_RAISED;
    return LIMIT_NOT_RAISED;
}

bool    Channel::is_already(const std::string &nickname) {
    std::vector<User *>::iterator   it;

    it = this->_users.begin();
    for (; it != this->_users.end(); it++) {
        if ((*it)->get_nickname() == nickname)
            return USER_ALREADY_IN;
    }
    return USER_NOT_ALREADY_IN;
}

std::string Channel::get_topic_msg(User *user) {
    std::stringstream res;

    //TODO hay que notificar al resto de una manera distinta
    res << ":" <<user->get_nickname(); //<< "!" << user->get_login_name() << "@localhost ";
    res << " JOIN " << this->_name << MSG_END;
    res << RPL_TOPIC << user->get_nickname() << " " << this->get_name() << " :" << this->get_topic() << MSG_END;
    return res.str();
}

std::string Channel::get_user_list_msg(User *user) {
    std::stringstream res;

    //TODO comprobar el nivel de privilegios del canal
    res << RPL_NAMREPLY << user->get_nickname() << " = " << this->_name << " :" << this->get_user_list() << MSG_END;
    res << RPL_ENDOFNAMES << user->get_nickname() << " " << this->_name << ENDOFNAMES;
    return res.str();
}

void    Channel::set_key_opt(const bool key_opt) {this->_key_opt = key_opt;}
void    Channel::set_topic(std::string msg) { this->_topic = msg; }
void    Channel::set_key(const std::string &key) {
    this->_key_opt = KEY_SET;
    this->_key = key;
}
void    Channel::set_invite(const bool invite) { this->_invite = invite; }
void    Channel::set_user_limit(const int user_limit){this->_user_limit = user_limit;}
void    Channel::set_user_ban(const std::string &nickname) {

    if (this->is_banned(nickname) == FOUND)
        return ;

    this->_banned.push_back(nickname);
}

void    Channel::unset_user_ban(const std::string &nickname) {
    std::vector<std::string>::iterator  it;

    for (it = this->_banned.begin(); it != this->_banned.end(); it++) {
        if (*it == nickname)
            this->_banned.erase(it);
    }
}


bool    Channel::enter_key(const std::string &key) {
    if (this->_key_opt == KEY_NOT_SET and key.size() == NO_KEY)
        return CORRECT_KEY;
    else if (this->_key_opt == KEY_NOT_SET and key.size() != NO_KEY)
        return INCORRECT_KEY;
    else if (this->_key_opt == KEY_SET and this->get_key() != key)
    {
        return INCORRECT_KEY;
    }
    return CORRECT_KEY;
}

std::vector<User *> Channel::get_visible_users()
{
    return this->_visible_users;
}

std::string Channel::get_visible_user_list() const
{
    std::string user_list = "";

    for (size_t i = 0; i < this->_visible_users.size(); i++)
        user_list += this->_visible_users.at(i)->get_nickname() + " ";
    return (user_list);
}

std::string Channel::get_permissions_to_string()
{
    std::string holder = "";

    holder += this->_channel_permissions.at(0) == false ? "-t" : "+t";
    holder += this->_channel_permissions.at(1) == false ? "-k" : "+k";
    holder += this->_channel_permissions.at(2) == false ? "-l" : "+l";
    holder += this->_channel_permissions.at(3) == false ? "-b" : "+b";
    holder += this->_channel_permissions.at(4) == false ? "-m" : "+m";
    holder += this->_channel_permissions.at(5) == false ? "-i" : "+i";

    return  holder;
}

std::map<const User*,  std::vector<char> > * Channel::get_user_permissions()
{
    return &this->_user_permissions;
}


std::vector<User *> Channel::get_users()
{
    return this->_users;
}

void    Channel::notice_join(Message &msg) {
    std::stringstream   ss;
    std::vector<User *>::iterator it;

    ss.str("");

    ss << ":" << msg.user->get_nickname() << " JOIN :" << this->_name << MSG_END;
    for (it = this->_users.begin(); it != this->_users.end(); it++) {
        send((*it)->get_socket(), ss.str().c_str(), ss.str().size(), 0);
    }
}

void    Channel::notice_part(Message &msg, const std::string &topic) {
    std::stringstream   ss;
    std::vector<User *>::iterator it;

    ss << ":" << msg.user->get_nickname() << " PART " << this->_name << " " << topic << MSG_END;
    for (it = this->_users.begin(); it != this->_users.end(); it++) {
        send((*it)->get_socket(), ss.str().c_str(), ss.str().size(), 0);
    }
}

std::vector<bool> *Channel::get_channel_permissions()
{
    return &_channel_permissions;
}

bool Channel::is_operator(User *user) {
    std::map<const User*, std::vector<char> >::iterator it;

    it = this->get_user_permissions()->find(user);
    std::vector<char>& permissions = it->second;

    if (!user)
        return false;

    return permissions[2] == '1';
}

void Channel::broadcast_msg(Message &msg) {
    std::vector<User *>::iterator it;

    it = this->_users.begin();
    for (; it != this->_users.end(); it++)
        send((*it)->get_socket(),  msg.res.str().c_str(), msg.res.str().size(), 0);

}

bool Channel::is_flag(int type) {
    return this->_channel_permissions[type];
}

bool Channel::is_invited(User *user)
{
    for (size_t i = 0; i < this->_invited_users.size();i++)
    {
        if (user == this->_invited_users.at(i))
            return true;
    }
    return false;
}

void                Channel::add_invited_user(User* user)
{
    for (size_t i = 0; i < this->_invited_users.size();i++)
    {
        if (user == this->_invited_users.at(i))
            return;
    }
    this->_invited_users.push_back(user);
}
