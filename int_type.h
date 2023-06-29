#pragma once

#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>
#include <vector>
#include "fmt_type.h"

class IntType : public FmtType {
public:
    IntType(size_t width);
    ~IntType() = default;
    std::string toString() const override;
    void format(std::vector<FmtType::FmtColumn> &formattedCols, const std::string &value) override;
    size_t getCompareHash() const override
    {
        return std::hash<size_t>()(width_);
    }
    void getTitleRow(std::vector<FmtType::FmtColumn> &titleRow,
                     std::vector<FmtType::FmtColumn> &underscoreRow) const override;
private:
    template <typename T>
    T StringToNum(const std::string &value, bool &rangeError);
    
    template <typename T>
    void FmtNumToHex(std::vector<FmtType::FmtColumn> &formattedCols, T valueAsType);

    template <typename T>
    void format(std::vector<FmtType::FmtColumn> &formattedCols, const std::string &value);

    size_t width_;
};

template <typename T>
void IntType::format(std::vector<FmtType::FmtColumn> &formattedCols, const std::string &value)
{
    bool rangeError = false;
    int8_t valueAsType = StringToNum<T>(value, rangeError);
    
    // First column is the base 10 version of the data
    if (rangeError) {
        formattedCols.emplace_back(OUT_OF_RANGE, OUT_OF_RANGE.size());
    } else {
        std::string formattedData = std::to_string(valueAsType);
        formattedCols.emplace_back(formattedData, formattedData.size());
    }

    // The next column will be the hex format of the number. Ensure leading zeros match the bitwidth.
    if (rangeError) {
        formattedCols.emplace_back(OUT_OF_RANGE, OUT_OF_RANGE.size());
    } else {
        FmtNumToHex<T>(formattedCols, valueAsType);
    }   
}

template <typename T>
void IntType::FmtNumToHex(std::vector<FmtType::FmtColumn> &formattedCols, T valueAsType)
{
    std::stringstream ss;
    // Each byte of the integer type takes 2 characters of width
    // Examples:
    // The number 4 in hex for an int8_t is: 0x04  (width of 2 characters)
    // The number 4 in hex for an int32_t is: 0x00000004 (width of 8 characters)
    // So the width is computed as sizeof(T)*2 which will create the leading zeros for us.
    // Since the hex format will also contain the characters "0x", that's 2 more bytes to add.
    ss << "0x" << std::hex << std::setw(sizeof(T)*2) << std::setfill('0') << valueAsType;
    std::string formattedData = ss.str();
    formattedCols.emplace_back(formattedData, formattedData.size());
}
