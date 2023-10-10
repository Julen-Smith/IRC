#ifndef MESSAGE_HPP
# define MESSAGE_HPP

#include <sstream>
#include <string>
#include <stack>

# define NOT_MODIFIED false
# define MODIFIED true

class Message
{
    public:
        std::stack<std::string> bucket;
        std::stringstream       ss;
        std::string             message;
        size_t                  size;
        bool                    modified;

        size_t  get_size();
        void    clean();
        void    to_string();
        void    push_bucket(std::string src);
        std::string    pop_bucket();

        Message();
        ~Message();



    protected:

    private:

};

std::istream& operator << (std::istream& stream, Message &src);

# endif //MESSAGE_HPP