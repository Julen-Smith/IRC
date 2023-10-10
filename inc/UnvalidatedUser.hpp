#ifndef UNVALIDATEDUSER_HPP
#define UNVALIDATEDUSER_HPP

#include <string>

class UnvalidatedUser
{
    public:
        int         socket;
        std::string nickname;
        std::string login_name;
        std::string realname;
        std::string level;

        UnvalidatedUser(int client_socket) : socket(client_socket) {}
        ~UnvalidatedUser() {}

    protected:

    private:
};

#endif //UNVALIDATEDUSER_HPP