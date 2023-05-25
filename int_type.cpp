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
    // to do
    std::cout << "doing an integer format(" << width_ << ") on value: " << value << std::endl;

    switch(width_) {
        case 8: {
            format8(formattedCols, value);
            break;
        }
        case 16: {
            format16(formattedCols, value);
            break;
        }
        case 32: {
            format32(formattedCols, value);            
            break;
        }
        case 64: {
            format64(formattedCols, value);            
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
    // do this later
    titleRow.emplace_back("title goes here", 15);
    underscoreRow.emplace_back("---------------", 15);
}

void IntType::format8(std::vector<FmtType::FmtColumn> &formattedCols, const std::string &value)
{
    int intValue;
    bool rangeError = false;

    // There does not exist any formatter that converts directly to an 8-bit int, so we use the int version first and
    // then assign it down if its in the valid range.
    // std::stoi has the ability to read the negative sign, as well as 0x prefix to convert string hex intputs. The 3rd
    // arg is the "base". Special value of 0 means it will try to auto-detect. For example, if the input is 0x12 it will
    // format as base 16 (hex). If 012 it assumes octal etc. Otherwise it assume base 10.

    // What happens if the value is too large for an 8-bit int, or other invalid values?
    // Exceptions:
    // std::invalid_argument if no conversion could be performed
    // std::out_of_range if the converted value would fall out of the range of the result type or if the underlying
    // function sets errno to ERANGE.
    try {
        intValue = std::stoi(value, nullptr, 0);
    }
    catch (std::invalid_argument const& ex)
    {
        // rethrow as my own exception    
        // Can dump this for more info: ex.what()
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

    // First column is the base 10 version of the data
    if (rangeError) {
        formattedCols.emplace_back(OUT_OF_RANGE, OUT_OF_RANGE.size());
    } else {
        valueAsType = intValue;  // safe down-cast, we already checked its range        
        std::string formattedData = std::to_string(valueAsType);
        formattedCols.emplace_back(formattedData, formattedData.size());
    }

    // The next column will be the hex format of the number. Ensure leading zeros match the bitwidth.
    if (rangeError) {
        formattedCols.emplace_back(OUT_OF_RANGE, OUT_OF_RANGE.size());
    } else {
        FmtNumToHex<int8_t>(formattedCols, valueAsType);
    }
}

void IntType::format16(std::vector<FmtType::FmtColumn> &formattedCols, const std::string &value)
{
}

void IntType::format32(std::vector<FmtType::FmtColumn> &formattedCols, const std::string &value)
{



    
}

void IntType::format64(std::vector<FmtType::FmtColumn> &formattedCols, const std::string &value)
{
}
