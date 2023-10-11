#ifndef MESSAGE_HPP
# define MESSAGE_HPP

#include <sstream>
#include <string>
#include <stack>
#include <deque>

#include "defs.hpp"

# define NOT_MODIFIED false
# define MODIFIED true
# define BUFFER_SIZE 1024

class Message
{
    public:
        typedef std::deque<std::string>::iterator  command;
        std::deque<std::string> *commands;
        std::deque<std::string> *params;
        std::deque<std::string> *holder;

        std::stack<std::string> bucket;

        std::stringstream       res;
        std::string             res_string;
        size_t                  res_size;
        
        char                    buffer[BUFFER_SIZE];
        int                     client_socket;
        int                     client_index;

        //void            set_holder(const std::string &holder, const std::string delimiter);
        //std::string     get_holder();

        std::string     get_params_front();
        void            set_commands();
        void            set_params();
        const char      *get_res_str();
        size_t          get_res_size() const;
        void            push_bucket(std::string src);
        std::string     pop_bucket();
        std::deque<std::string> *split(const std::string &buffer, const std::string &pattern);


        Message();
        ~Message();
    
    protected:

    private:
        size_t	_delimiter(const std::string &src, const std::string &pattern, size_t index, bool mode);

};

# endif //MESSAGE_HPP