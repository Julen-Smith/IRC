#include "Message.hpp"

Message::Message(){}

Message::~Message(){}

size_t  Message::get_res_size() const { return this->res_size; }

void    Message::load_stream() { 
    this->ss.clear();
    this->ss << this->buffer;
}

const char    *Message::get_res_str() {
    this->res_string = this->res.str();
    this->res_size = this->res_string.size();
    return this->res_string.c_str();
}

void    Message::push_bucket(std::string src) { this->bucket.push(src); }
std::string    Message::pop_bucket() { return this->bucket.top(); }

Message&   operator<< (Message &instance, std::string &input) {
   instance.ss << input;
   return instance;
}

Message&   operator>> (Message &instance, std::string &output) {
   instance.ss >> output;
   return instance;
}