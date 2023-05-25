#pragma once

#include <cstddef>
#include <cstdint>
#include <iomanip>
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
    void FmtNumToHex(std::vector<FmtType::FmtColumn> &formattedCols, T valueAsType);
    void format8(std::vector<FmtType::FmtColumn> &formattedCols, const std::string &value);
    void format16(std::vector<FmtType::FmtColumn> &formattedCols, const std::string &value);
    void format32(std::vector<FmtType::FmtColumn> &formattedCols, const std::string &value);
    void format64(std::vector<FmtType::FmtColumn> &formattedCols, const std::string &value);    
    size_t width_;
};

template <typename T>
void IntType::FmtNumToHex(std::vector<FmtType::FmtColumn> &formattedCols, T valueAsType)
{
    std::stringstream ss;
    // Each byte of the integer type takes 2 characters of width
    // Examples:
    // The number 4 in hex for an int8_t is: 0x04  (width of 2 characters
    // The number 4 in hex for an int32_t is: 0x00000004 (width of 8 characters)
    // So the width is computed as sizeof(T)*2
    ss << std::hex << std::showbase << std::setw(sizeof(T)*2) << std::setfill('0') << valueAsType;
    std::string formattedData = ss.str();
    formattedCols.emplace_back(formattedData, formattedData.size());
}
