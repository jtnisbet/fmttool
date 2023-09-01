#include "int_type.h"
#include <climits>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>
#include "fmt_exception.h"
#include "fmt_type.h"
#include "fmt_tool.h"

// IntType methods
IntType::IntType(size_t width, bool isSigned, FmtTool *parent) : FmtType(parent), width_(width), isSigned_(isSigned)
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
                format<uint64_t, unsigned long long int>(formattedCols, value);
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
    if (!parentTool_->IsBinaryFmtSuppressed()) {
        // display binary formatting column if it has not been univesally suppressed via flag
        titleRow1.emplace_back(BASE_2, BASE_2.size());
        titleRow2.emplace_back(widthName, widthName.size());
        underscoreRow.emplace_back("", BASE_2.size());
    }

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
// Lastly, in most cases we'll use the non-unsigned versions of the call, but will ensure to use a larger bitwidth of
// the std::sto* functions.  For example, for 8, 16 bit, we can use the 32-bit std::stoi.  For 32, we'll use long int.
// The only challenge is the uint64_t conversion.  There is not larger type.  We can safely use std::stoll for the
// int64_t, but for uin64_t we need one more bit. This requries std::stoull function.  However, this function deploys
// integer wrapping. -1 does not yield out of range error naturally.  it just wraps it to the max postive value.
// Thus, manual detection of negative will be added for that case.

template <>
int IntType::stringToNum<int>(const std::string &value, ErrType &err)
{
    int intValue;
    try {
        intValue = std::stoi(value, nullptr, 0);
    }
    catch (std::invalid_argument const& ex)
    {
        err = ErrType::FmtErrInvalid;
    }
    catch(std::out_of_range const& ex)
    {
        // Don't throw an exception if we are out of range. Instead, we'll print a message in the formatted output.
        err = ErrType::FmtErrRange;
    }
    return intValue;
}

template <>
long int IntType::stringToNum<long int>(const std::string &value, ErrType &err)
{
    long int intValue;
    try {
        intValue = std::stol(value, nullptr, 0);
    }
    catch (std::invalid_argument const& ex)
    {
        err = ErrType::FmtErrInvalid;
    }
    catch(std::out_of_range const& ex)
    {
        // Don't throw an exception if we are out of range. Instead, we'll print a message in the formatted output.
        err = ErrType::FmtErrRange;
    }
    return intValue;
}

template <>
long long int IntType::stringToNum<long long int>(const std::string &value, ErrType &err)
{
    long long int intValue;
    try {
        intValue = std::stoll(value, nullptr, 0);
    }
    catch (std::invalid_argument const& ex)
    {
        err = ErrType::FmtErrInvalid;
    }
    catch(std::out_of_range const& ex)
    {
        // Special case for a hex number
        // When you read in a hex number, it treats the number as the numerical value itself, not the internal bit
        // representation of it.
        // For example, the number 0x8000000000000000 will be assumed to be huge positive number that is too big for the
        // 64-bit in type, so it gives out of range exeception.
        // The problem with this is that this same hex number IS a valid number for 64-bit int. Its the number
        // -9223372036854775808 which is in range.
        // We can use stoull though.  This will allow it, and then cast the number into its signed type.
        // I didn't bother to implmenet this logic for the other signed type conversions because there was always a
        // "bigger bitwidth" that we could use. no such luck for 64-bit dudes.  There isn't a 128 bit numeric native
        // type.
        if (value.compare(0,2, "0x") == 0) {
            try {
                unsigned long long int tempValue;
                tempValue = std::stoull(value, nullptr, 0);
                intValue = tempValue;  // purposely down cast.  will change the value to correct negative value
            }
            catch (std::invalid_argument const& ex)
            {
                err = ErrType::FmtErrInvalid;
            }
            catch(std::out_of_range const& ex)
            {
                err = ErrType::FmtErrRange;
            }
        } else {
            // Don't throw an exception if we are out of range. Instead, we'll print a message in the formatted output.
            err = ErrType::FmtErrRange;
        }
    }
    return intValue;
}

template <>
unsigned long long int IntType::stringToNum<unsigned long long int>(const std::string &value, ErrType &err)
{
    long long int intValue;
    // Manual detection of a negative input. Treat this as out of range rather than format the overlap.
    if (value[0] == '-') {
        err = ErrType::FmtErrRange;
        return 0;
    }
    try {
        intValue = std::stoull(value, nullptr, 0);
    }
    catch (std::invalid_argument const& ex)
    {
        err = ErrType::FmtErrInvalid;
    }
    catch(std::out_of_range const& ex)
    {
        // Don't throw an exception if we are out of range. Instead, we'll print a message in the formatted output.
        err = ErrType::FmtErrRange;
    }
    return intValue;
}
