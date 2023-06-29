#include "int_type.h"
#include <climits>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>
#include "fmt_exception.h"
#include "fmt_type.h"

// IntType methods
IntType::IntType(size_t width) : FmtType(), width_(width)
{
    if (width_ != 8 && width != 16 && width_ != 32 && width_ != 64) {
        THROW_FMT_EXCEPTION("Invalid width value for integer format (-i <width>). Must be 8, 16, 32, or 64.");
    }
}

std::string IntType::toString() const
{
    std::string retStr;
    retStr = "int (width = " + std::to_string(width_) + ")";
    return retStr;
}

void IntType::format(std::vector<FmtColumn> &formattedCols, const std::string &value)
{
    switch(width_) {
        case 8: {
            format<int8_t>(formattedCols, value);
            //format8(formattedCols, value);
            break;
        }
        case 16: {
//            format<int16_t>(formattedCols, value);
            break;
        }
        case 32: {
//            format<int32_t>(formattedCols, value);            
            break;
        }
        case 64: {
//            format<int64_t>(formattedCols, value);            
            break;
        }
        default: {
            // not possible becuse we already checked this. but leave the check here anyway.
            THROW_FMT_EXCEPTION("Invalid width value for integer format (-i <width>). Must be 8, 16, 32, or 64.");
            break;
        }
    }
}

void IntType::getTitleRow(std::vector<FmtType::FmtColumn> &titleRow,
                          std::vector<FmtType::FmtColumn> &underscoreRow) const
{
    const std::string BASE_10 = "Base 10";
    const std::string BASE_16 = "Hex";
    // This type provides 2 columns: decimal formatted and hex formatted.
    // Empty string instructs the displayer code to write '-' characters to make an underline.
    titleRow.emplace_back(BASE_10, BASE_10.size());
    underscoreRow.emplace_back("", BASE_10.size());
    titleRow.emplace_back(BASE_16, BASE_16.size());
    underscoreRow.emplace_back("", BASE_16.size());
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

// Specialization for the int8_t
template <>
int8_t IntType::StringToNum<int8_t>(const std::string &value, bool &rangeError)
{
    int intValue;
    // There does not exist any formatter that converts directly to an 8-bit int, so we use the int version first and
    // then assign it down if its in the valid range. We do this because we want to correctly show the range error
    // if the data given exceeds the type width.
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

    // We know that the range of an 8 bit signed int is -128 to 127 (we use the signed char limit constant for this)
    if (intValue < SCHAR_MIN || intValue > SCHAR_MAX) {
        rangeError = true;
    }

    int8_t valueAsType = 0;
    if (!rangeError) {
        valueAsType = intValue;  // range already checked, this is safe downcast
    }
        
    return valueAsType;
}

// Specialization for the int32_t
template <>
int32_t IntType::StringToNum<int32_t>(const std::string &value, bool &rangeError)
{
    int32_t valueAsType = 0;
    try {
        // stoi returns regular int. Not very portable here, but lets just assume that int32_t and int
        // are the same thing and just do it.
        valueAsType = std::stoi(value, nullptr, 0);
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
    return valueAsType;
}

// specialization for int8_t. The general method for converting to hex doesnt' work for this one.
// cast the single byte to int32_t first.
template <>
void IntType::FmtNumToHex<int8_t>(std::vector<FmtType::FmtColumn> &formattedCols, int8_t valueAsType)
{
    std::stringstream ss;
    // std::show_base doesn't work well along with setw. it always put the 0x in the wrong place.
    // Thus just manually put 0x in there for now.
    ss << std::hex << "0x" << std::setw(2) << std::setfill('0') << static_cast<int32_t>(valueAsType);
    std::string formattedData = ss.str();
    formattedCols.emplace_back(formattedData, formattedData.size());
}
