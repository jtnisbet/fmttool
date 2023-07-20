#pragma once

#include <bitset>
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
    // Convert to the target number type by calling appropriate std::sto* function.
    // Only supports int, long int, and long long int.
    // All other types are not allowed, therefore we mark the generic non-specialized version as deleted.
    template <typename I>
    I stringToNum(const std::string &value, bool &rangeError) = delete;

    template <typename T>
    void fmtNumToHex(std::vector<FmtType::FmtColumn> &formattedCols, T valueAsType);

    template <typename T, typename I>
    void format(std::vector<FmtType::FmtColumn> &formattedCols, const std::string &value);

    size_t width_;
    bool isSigned_;
};

#include "int_type.tpp"  // include the template implementation
