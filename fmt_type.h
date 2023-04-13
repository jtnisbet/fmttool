#pragma once

#include "fmt_tool.h"

class FmtType {
public:
    enum class FmtTypeArg : int16_t {
        kInt = 1,
        kAscii = 2,
        kBinary = 3    
    };

    FmtType(FmtTypeArg type, size_t width);
    ~FmtType() = default;
private:
    FmtTypeArg type_;
    size_t width_;
};
