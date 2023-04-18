#pragma once

#include <iostream>
#include <stdexcept>

#define THROW_FMT_EXCEPTION(msg) \
    throw FmtException(msg, __FILE__, __LINE__);

// A quick class here for my own exceptions, inherit from std exceptions runtime_error
class FmtException : public std::runtime_error {
public:
    FmtException(const std::string &msg, const std::string &fileName, int line)
        : std::runtime_error(msg), file_(fileName), line_(line)
    {
        msg_ = "Exception:\n" + std::string(std::runtime_error::what());
        msg_ += "\nFile: " + file_ + " Line: " + std::to_string(line_);        
    }

    virtual const char* what() const throw() override
    {
        return msg_.c_str();
    }
private:
    int line_;
    std::string file_;
    std::string msg_;
};
