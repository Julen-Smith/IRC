#include <iostream>

#include "Channel.hpp"
#include "User.hpp"
#include "defs.hpp"

User::User(const std::string &nickname, int socket):
_nickname(nickname), _notices(true), _socket(socket), _count(EMPTY),_is_operator(false) {
    if (nickname == "julen" || nickname == "jul" || nickname == "lukas")
        this->_is_operator = true;
}

User::User(UnvalidatedUser *unva_user):
    _nickname(unva_user->nickname), _notices(true), _socket(unva_user->socket),
    _count(EMPTY) {
    this->_login_name = unva_user->login_name;
    this->_level = unva_user->level;
    this->_realname = unva_user->realname;
}

User::User(User &obj) // : rooms(obj.rooms)
{
    //Cambiar más adelante
    this->_notices = obj.get_notices();
    this->_nickname = obj.get_nickname();
    std::cout << "Guest has join the channel [ " << "Lobby"<< " ]" << std::endl;
}

User::~User()
{
    std::cout << " - User " << this->_nickname << " has been deleted." << std::endl;
}

User& User::operator=(User &user)
{
     std::cout << "Operator placeholder" << std::endl;
     return *this;
}

std::string         User::get_channels_str() {
    std::vector<Channel *>::iterator    it;
    std::string result = " ";

    it = this->_channels.begin();
    for (; it != this->_channels.end(); it++) {
        result += (*it)->get_name() + " ";
        std::cout << "Whois channel: " << result << std::endl;
    }
    result[result.size() - 1] = 0;
    return result;
}
bool                User::get_operator_status() const{return this->_is_operator;}   
const std::string&  User::get_nickname(void) const {return this->_nickname;}
const std::string&  User::get_login_name(void) const {return this->_login_name;}
bool                User::get_notices(void) const {return this->_notices;}
int                 User::get_socket(void) const {return this->_socket;}
size_t              User::get_count(void) const {return this->_count; }

void                User::set_notices(bool state) {this->_notices = state; }
void                User::set_nickname(std::string nick) { this->_nickname = nick; }
void                User::set_operator_status(bool status) { this->_is_operator = status;} 

void    User::add_channel_count() { this->_count++; }
void    User::add_channel(Channel *channel) { this->_channels.push_back(channel);}
void    User::substract_channel_count() { this->_count--; }

void    User::delete_channel(Channel *channel) {
    std::vector<Channel *>::iterator it;

    it = this->_channels.begin();
    for (; it != this->_channels.end(); it++) {
        if ((*it) == channel) {
            this->_channels.erase(it);
            return ;
        }
    }
}