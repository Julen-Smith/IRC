#ifndef MESSAGE_HPP
# define MESSAGE_HPP

#include <sstream>
#include <string>
#include <stack>

# define NOT_MODIFIED false
# define MODIFIED true
# define BUFFER_SIZE 1024

class Message
{
    public:
        std::stack<std::string> bucket;

        std::stringstream       ss;
        std::string             ss_string;
        size_t                  ss_size;

        std::stringstream       res;
        std::string             res_string;
        size_t                  res_size;
        
        ////std::string             message;
        ////bool                    modified;

        char                    buffer[BUFFER_SIZE];
        int                     client_socket;
        int                     client_index;

        const char      *get_res_str();
        size_t          get_res_size() const;
        void            load_stream();
        void            push_bucket(std::string src);
        std::string     pop_bucket();

        Message();
        ~Message();



    protected:

    private:

};

Message& operator << (Message &src, std::string &input);
Message& operator >> (Message &src, std::string &output);

# endif //MESSAGE_HPP