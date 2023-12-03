#include <cstring>
#include <iostream>

#include "Message.hpp"

Message::Message(){
    this->commands = NULL;
    this->params = NULL;
    this->holder = NULL;
    this->user = NULL;

    this->res.clear();
    this->res_string = "";
    this->res_size = 0;
    std::memset(this->buffer, 0, BUFFER_SIZE);
}

Message::~Message(){}

size_t  Message::get_res_size() const { return this->res_size; }

const char    *Message::get_res_str() {

    this->res_string = this->res.str();
    this->res_size = this->res_string.size();
    return this->res_string.c_str();
}

size_t	Message::_delimiter(const std::string &src, const std::string &pattern, size_t index, bool mode) {
	size_t	j;

	for (j = index; j < src.size()
            and (pattern.find(src[j]) == static_cast<size_t>(-1)) == mode;) {
		j++;
	}
	return j;
}

std::deque<std::string>	*Message::split(const std::string &buffer, const std::string &pattern) {
	size_t	start;
	size_t	end;
	std::deque<std::string>	*bucket;

	start = 0;
    bucket = new std::deque<std::string>;

	for (size_t i = 0; i < buffer.size(); i++) {
		start = this->_delimiter(buffer, pattern, i, 0);
		end = this->_delimiter(buffer, pattern, start, 1);

		if (start >= end)
			break ;

		std::string token(buffer, start, end - start);
        bucket->push_back(token);
		i = end;
	}

    if (bucket->size() == 0) {
        delete bucket;
        return NULL;
    }
    return bucket;
}

void    Message::set_commands() {
    this->commands = this->split(this->buffer, MSG_END);
}

void    Message::set_params() {
    this->params = this->split(this->commands->front(), PARAMS_PATTERN);
    this->commands->pop_front();
}

std::string Message::get_params_front() {
    std::string token;

    token = this->params->front();
    this->params->pop_front();
    return token;
}

void    Message::set_user(User *user) { this->user = user; }

/*
void            Message::set_holder(const std::string &holder, const std::string delimiter) {

    if (this->holder != NULL)
        delete this->holder;
    this->holder = this->split(holder, delimiter);
}
std::string     Message::get_holder() {
    std::string token;

    token = this->holder->front();
    this->holder->pop_front();
    return token;
}*/
