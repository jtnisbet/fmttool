#include "int_type.h"
#include <climits>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>
#include "fmt_exception.h"
#include "fmt_type.h"

// IntType methods
IntType::IntType(size_t width, bool isSigned) : FmtType(), width_(width), isSigned_(isSigned)
{
    if (width_ != 8 && width != 16 && width_ != 32 && width_ != 64) {
        THROW_FMT_EXCEPTION("Invalid width value for integer format (-i <width>). Must be 8, 16, 32, or 64.");
    }
}

std::string IntType::toString() const
{
    std::string retStr;
    switch (width_) {
        case 8: {
            retStr = (isSigned_) ? "int8_t" : "uint8_t";
            break;
        }
        case 16: {
            retStr = (isSigned_) ? "int16_t" : "uint16_t";
            break;
        }
        case 32: {
            retStr = (isSigned_) ? "int32_t" : "uint32_t";
            break;
        }
        case 64: {
            retStr = (isSigned_) ? "int64_t" : "uint64_t";
            break;    
        }
        default: {
            // not possible because we already checked this. but leave the check here anyway.
            THROW_FMT_EXCEPTION("Invalid width value for integer format (-i <width>). Must be 8, 16, 32, or 64.");
            break;
        }
    }
    return retStr;
}

void IntType::format(std::vector<FmtColumn> &formattedCols, const std::string &value)
{
    switch(width_) {
        case 8: {
            if (isSigned_) {
                format<int8_t, int>(formattedCols, value);
            } else {
                format<uint8_t, int>(formattedCols, value);
            }
            break;
        }
        case 16: {
            if (isSigned_) {
                format<int16_t, int>(formattedCols, value);
            } else {
                format<uint16_t, int>(formattedCols, value);
            }
            break;
        }
        case 32: {
            if (isSigned_) {
                format<int32_t, long int>(formattedCols, value);
            } else {
                format<uint32_t, long int>(formattedCols, value);
            }
            break;
        }
        case 64: {
            if (isSigned_) {
                format<int64_t, long long int>(formattedCols, value);
            } else {
                format<uint64_t, long long int>(formattedCols, value);
            }
            break;
        }
        default: {
            // not possible because we already checked this. but leave the check here anyway.
            THROW_FMT_EXCEPTION("Invalid width value for integer format (-i <width>). Must be 8, 16, 32, or 64.");
            break;
        }
    }
}

void IntType::getTitleRow(std::vector<FmtType::FmtColumn> &titleRow1, std::vector<FmtType::FmtColumn> &titleRow2,
                          std::vector<FmtType::FmtColumn> &underscoreRow) const
{
    const std::string BASE_10 = "Base 10";
    const std::string BASE_16 = "Hex";
    const std::string BASE_2 = "Bin";
    std::string widthName = this->toString();
    
    // This type provides 2 columns: decimal formatted and hex formatted.
    // Empty string instructs the displayer code to write '-' characters to make an underline.
    titleRow1.emplace_back(BASE_10, BASE_10.size());
    titleRow2.emplace_back(widthName, widthName.size());
    underscoreRow.emplace_back("", BASE_10.size());

    titleRow1.emplace_back(BASE_16, BASE_16.size());
    titleRow2.emplace_back(widthName, widthName.size());
    underscoreRow.emplace_back("", BASE_16.size());

    titleRow1.emplace_back(BASE_2, BASE_2.size());
    titleRow2.emplace_back(widthName, widthName.size());
    underscoreRow.emplace_back("", BASE_2.size());

}

// specialization for int8_t. The general method for converting to hex doesnt' work for this one.
// cast the single byte to int32_t first.
template <>
void IntType::fmtNumToHex<int8_t>(std::vector<FmtType::FmtColumn> &formattedCols, int8_t valueAsType)
{
    std::stringstream ss;
    // For negative numbers, casting to 4 byte type adds lots of 0xFF's. But there's only one byte
    // here.  Thus, we need to truncate
    ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int32_t>(valueAsType);
    std::string formattedData = ss.str();
    int strSize = formattedData.size();
    if (strSize > 2) {
        formattedData = formattedData.substr(strSize - 2, strSize);
    }
    formattedData = "0x" + formattedData;
    formattedCols.emplace_back(formattedData, formattedData.size());
}

// specialization for uint8_t. The general method for converting to hex doesnt' work for this one.
// cast the single byte to uint32_t first.
template <>
void IntType::fmtNumToHex<uint8_t>(std::vector<FmtType::FmtColumn> &formattedCols, uint8_t valueAsType)
{
    std::stringstream ss;
    ss << std::hex << "0x" << std::setw(2) << std::setfill('0') << static_cast<uint32_t>(valueAsType);
    std::string formattedData = ss.str();
    formattedCols.emplace_back(formattedData, formattedData.size());
}

// A note on the formatting:
// std::stoi has the ability to read the negative sign, as well as 0x prefix to convert string hex intputs. The 3rd arg
// is the "base". Special value of 0 means it will try to auto-detect. For example, if the input is 0x12 it will format
// as base 16 (hex). If 012 it assumes octal etc. Otherwise it assume base 10.
// What happens if the value is too large for the defined type, or other invalid values?
// Exceptions:
// std::invalid_argument if no conversion could be performed
// std::out_of_range if the converted value would fall out of the range of the result type or if the underlying
// function sets errno to ERANGE.

template <>
int IntType::stringToNum<int>(const std::string &value, bool &rangeError)
{
    int intValue;
    try {
        intValue = std::stoi(value, nullptr, 0);
    }
    catch (std::invalid_argument const& ex)
    {
        THROW_FMT_EXCEPTION("Invalid data for formatting: " + value);
    }
    catch(std::out_of_range const& ex)
    {
        // Don't throw an exception if we are out of range. Instead, we'll print a message in the formatted output.
        rangeError = true;
    }
    return intValue;
}

template <>
long int IntType::stringToNum<long int>(const std::string &value, bool &rangeError)
{
    long int intValue;
    try {
        intValue = std::stol(value, nullptr, 0);
    }
    catch (std::invalid_argument const& ex)
    {
        THROW_FMT_EXCEPTION("Invalid data for formatting: " + value);
    }
    catch(std::out_of_range const& ex)
    {
        // Don't throw an exception if we are out of range. Instead, we'll print a message in the formatted output.
        rangeError = true;
    }
    return intValue;
}

template <>
long long int IntType::stringToNum<long long int>(const std::string &value, bool &rangeError)
{
    long long int intValue;
    try {
        intValue = std::stoll(value, nullptr, 0);
    }
    catch (std::invalid_argument const& ex)
    {
        THROW_FMT_EXCEPTION("Invalid data for formatting: " + value);
    }
    catch(std::out_of_range const& ex)
    {
        // Don't throw an exception if we are out of range. Instead, we'll print a message in the formatted output.
        rangeError = true;
    }
    return intValue;
}
