#pragma once

#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>
#include <vector>
#include "fmt_exception.h"
#include "fmt_type.h"

class IntType : public FmtType {
public:
    IntType(size_t width, bool isSigned);
    ~IntType() = default;
    std::string toString() const override;
    void format(std::vector<FmtType::FmtColumn> &formattedCols, const std::string &value) override;
    size_t getCompareHash() const override
    {
        return std::hash<size_t>()(width_ + static_cast<size_t>(isSigned_));
    }
    void getTitleRow(std::vector<FmtType::FmtColumn> &titleRow1, std::vector<FmtType::FmtColumn> &titleRow2,
                     std::vector<FmtType::FmtColumn> &underscoreRow) const override;
private:
    template <typename T>
    T stringToNumUsingInt(const std::string &value, bool &rangeError);

    template <typename T>
    T stringToNumUsingLongInt(const std::string &value, bool &rangeError);

    template <typename T>
    T stringToNumUsingLongLongInt(const std::string &value, bool &rangeError);

    template <typename T>
    T stringToNum(const std::string &xvalue, bool &rangeError);
    
    template <typename T>
    void fmtNumToHex(std::vector<FmtType::FmtColumn> &formattedCols, T valueAsType);

    template <typename T>
    void format(std::vector<FmtType::FmtColumn> &formattedCols, const std::string &value);

    size_t width_;
    bool isSigned_;
};

#include "int_type.tpp"  // include the template implementation
