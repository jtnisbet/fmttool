#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

class CmdArg {
public:
    using FmtColumn = std::pair<std::string, size_t>>;
    
    enum class CmdArgType : int8_t {
        INT = 1,
        ASCII = 2,
        BINARY = 3,
        HELP = 4
    };

    CmdArg(CmdArgType argType, size_t width);
    ~CmdArg() = default;
    bool operator<(const CmdArg &other) const
    {
        if (this->type_ == other.type_) {
            return this->width_ < other.width_;
        }
        return this->type_ < other.type_;
    }
    std::string toString();
    void format(std::vector<FmtColumn> &formattedCols, const std::string &value);
private:
    void intFormat(std::vector<FmtColumn> &formattedCols, const std::string &value)
    CmdArgType type_;
    size_t width_;
};
