#ifndef CREATEUSER_HPP
#define CREATEUSER_HPP

#include <string>

class CreateUser
{
    public:
        int         socket;
        std::string nickname;
        std::string login_name;
        std::string realname;
        std::string level;

        CreateUser(int client_socket) : socket(client_socket) {}
        ~CreateUser() {}

    protected:

    private:
};

#endif //CREATEUSER_HPP