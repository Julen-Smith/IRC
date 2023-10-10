#include "Message.hpp"

Message::Message(){}

Message::~Message(){}

void    Message::to_string(void) {
    this->message = this->ss.str();
}

void    Message::clean() {
    this->ss.str();
    this->message.clear();
}

size_t  Message::get_size() {
    if (this->modified == NOT_MODIFIED)
        return this->size;

    this->to_string();
    return this->message.size();
}

void    Message::push_bucket(std::string src) { this->bucket.push(src); }
std::string    Message::pop_bucket() { return this->bucket.top(); }

std::istream&   operator<< (std::istream& stream, Message &src) {
   src.ss << stream;
   return stream;
}