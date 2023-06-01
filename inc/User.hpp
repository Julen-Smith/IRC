#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <vector>
#include <poll.h>

class User
{
    private:
        std::string _nickname;
        bool        _notices;
        int         _socket;

    public:
        //User();
        User(const std::string &, int);
        User(const User &);

        ~User();

        User& operator=(User &user);
        const std::string&  get_nickname(void) const;
        bool                get_notices(void) const;
        int                 get_socket(void) const;
        
        void                set_notices(void);
        void                set_nickname(std::string);

};


#endif
