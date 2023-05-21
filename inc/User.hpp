#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <vector>
#include <poll.h>

class User : public pollfd
{
    private:
        std::string _nickname;
        bool        _notices;

    public:
        //User();
        User(const std::string &);
        User(const User &);

        ~User();

        User& operator=(User &user);
        const std::string&  get_nickname(void) const;
        bool                get_notices(void) const;

        void                set_pollfd(int, int);
        void                set_notices();
};


#endif
