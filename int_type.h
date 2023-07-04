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
    // common code for ints that are smaller than the 4 byte type.
    // i.e. uint8 and int8, uint16 and int16.
    // It is safe to use the bigger type to format them using intermediate larger width type (not to mention,
    // there is no conversion function directly to those smaller types.
    template <typename T>
    T StringToNumUsingInt(const std::string &value, bool &rangeError);

    template <typename T>
    T StringToNum(const std::string &value, bool &rangeError);
    
    template <typename T>
    void FmtNumToHex(std::vector<FmtType::FmtColumn> &formattedCols, T valueAsType);

    template <typename T>
    void format(std::vector<FmtType::FmtColumn> &formattedCols, const std::string &value);

    size_t width_;
    bool isSigned_;
};

template <typename T>
void IntType::format(std::vector<FmtType::FmtColumn> &formattedCols, const std::string &value)
{
    bool rangeError = false;
    T valueAsType = StringToNum<T>(value, rangeError);
    
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

template <typename T>
T IntType::StringToNumUsingInt(const std::string &value, bool &rangeError)
{
    T valueAsType = 0;
    int intValue;
    // There does not exist any formatter that converts directly to an 8-bit int, so we use the int version first and
    // then assign it down if its in the valid range. We do this because we want to correctly show the range error
    // if the data given exceeds the type width.
    try {
        intValue = std::stoi(value, nullptr, 0);
    }
    catch (std::invalid_argument const& ex)
    {
//        THROW_FMT_EXCEPTION("Invalid data for formatting: " + value);
        std::cout << "fix me.  can't process the macro or something?" << std::endl;
    }
    catch(std::out_of_range const& ex)
    {
        // Don't throw an exception if we are out of range. Instead, we'll print a message in the formatted output.
        rangeError = true;
    }
    
    // The int value is bigger width than the types we are checking. Thus, manually check the limits of the type
    // here to produce range errors
    if (intValue < std::numeric_limits<T>::min() || intValue > std::numeric_limits<T>::max()) {
        rangeError = true;
    }

    if (!rangeError) {
        valueAsType = intValue;  // range already checked, this is safe downcast
    }

    return valueAsType;
}
