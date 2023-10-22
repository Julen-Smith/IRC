#include <iostream>

#include "User.hpp"
#include "defs.hpp"

User::User(const std::string &nickname, int socket, time_t curr_time):
_nickname(nickname), _notices(true), _socket(socket), _count(EMPTY), _curr_time(curr_time) {
    std::cout << "Nickname: " << nickname << std::endl;
}

User::User(UnvalidatedUser *unva_user, time_t curr_time):
    _nickname(unva_user->nickname), _notices(true), _socket(unva_user->socket),
    _count(EMPTY), _curr_time(curr_time) {
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

const std::string&  User::get_nickname(void) const {return this->_nickname;}
const std::string&  User::get_login_name(void) const {return this->_login_name;}
bool                User::get_notices(void) const {return this->_notices;}
int                 User::get_socket(void) const {return this->_socket;}
size_t              User::get_count(void) const {return this->_count; }
time_t              User::get_curr_time() const { return this->_curr_time; }

void                User::set_curr_time(time_t curr_time) { this->_curr_time = curr_time;}
void                User::set_notices(bool state) {this->_notices = state; }
void                User::set_nickname(std::string nick) { this->_nickname = nick; }

void    User::add_channel_count() { this->_count++; }
void    User::substract_channel_count() { this->_count--; }
