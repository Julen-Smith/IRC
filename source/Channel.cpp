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
        std::cout << msg.res.str();
        if (client_socket != msg.client_socket)
            send(client_socket, msg.get_res_str(), msg.get_res_size(), 0);
    }
}

Channel::Channel(const std::string& name, const std::string &topic) 
: _name(name), _topic(topic), _invite(false), _user_limit(STANDARD_LIMIT), _key_opt(KEY_NOT_SET)
{
    std::cout << "The channel " << name << " has been created." << std::endl;
    for(int i = 0; i != 5; i++) _channel_permissions.push_back(false);
    stdout_channel_permissions();
}

Channel::~Channel()
{
    std::cout << "The channel " << this->_name << " has been deleted." << std::endl;
}

Channel& Channel::operator=(const Channel &new_channel)
{
    return *this;
}

void Channel::welcome_msg() const
{
    std::cout << "Welcome to " << this->_name << std::endl;
}

Channel::Channel(){};

void Channel::join_channel(std::string buffer, User &user)
{
   // this->channel_users.push_back(&user);
   // this->channel_users.back().nickname = buffer;
   // std::cout << "User " << this->channel_users.back()->getName() << " has join the channel" << std::endl;
}

std::string Channel::get_user_list() const
{
    std::string user_list = "";
    
    for (int i = 0; i < this->_users.size(); i++)
        user_list += this->_users.at(i)->get_nickname() + " ";
    return (user_list);
}

void    Channel::add_user(User *user)
{

    user->add_channel_count();
  
    std::vector<char> permissions(6, '0');
    this->_user_permissions[user] = permissions;
    if (!this->_users.size())
        this->_user_permissions[user][2] = '1';
    this->_users.push_back(user);
    stdout_channel__users_permissions(user);
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
int Channel::get_users_size() const { return (this->_users.size()); }

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
    res << user->get_nickname() << "!" << user->get_login_name() << "@localhost ";
    res << "JOIN " << this->_name << MSG_END;
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

void    Channel::set_user_limit(int user_limit) { this->_user_limit = user_limit; }
void    Channel::set_key(const std::string &key) {
    this->_key_opt = KEY_SET;
    this->_key = key;
}
void    Channel::set_invite(const bool invite) { this->_invite = invite; }
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
        return INCORRECT_KEY;
    return CORRECT_KEY;
}

void    Channel::stdout_channel_permissions()
{
    std::cout << "Permisos [ " << this->get_name() <<" ]" << std::endl;
    std::cout << "Modo solo temas : " << this->_channel_permissions.at(0) << std::endl;
    std::cout << "Modo de clave : " << this->_channel_permissions.at(1) << std::endl;
    std::cout << "Modo de limite de usuarios : " << this->_channel_permissions.at(2) << std::endl;
    std::cout << "Modo de ban : " << this->_channel_permissions.at(3) << std::endl;
    std::cout << "Modo moderado : " << this->_channel_permissions.at(4) << std::endl;
}

void    Channel::stdout_channel__users_permissions(const User *user)
{
    std::cout << "Permisos [ " << user->get_nickname() <<" ]" << std::endl;
    std::cout << "Modo invisible : " << this->_user_permissions[user].at(0) << std::endl;
    std::cout << "Modo wallops : " << this->_user_permissions[user].at(1) << std::endl;
    std::cout << "Modo operator : " << this->_user_permissions[user].at(2) << std::endl;
    std::cout << "Modo silencioso : " << this->_user_permissions[user].at(3) << std::endl;
    std::cout << "Modo voz  : " << this->_user_permissions[user].at(4) << std::endl;
    std::cout << "Modo cuarentena  : " << this->_user_permissions[user].at(5) << std::endl;
}


std::string Channel::get_permissions_to_string()
{
    std::string holder = "";
    
    holder += this->_channel_permissions.at(0) == false ? "-t" : "+t";
    holder += this->_channel_permissions.at(0) == false ? "-k" : "+k";
    holder += this->_channel_permissions.at(0) == false ? "-l" : "+l";
    holder += this->_channel_permissions.at(0) == false ? "-b" : "+b";
    holder += this->_channel_permissions.at(0) == false ? "-m" : "+m";
    
    return  holder;
}