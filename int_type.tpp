// included directly from int_type.h
// Put in this file to separate implementation from the class

// A note on the formatting:
// std::stoi has the ability to read the negative sign, as well as 0x prefix to convert string hex intputs. The 3rd arg
// is the "base". Special value of 0 means it will try to auto-detect. For example, if the input is 0x12 it will format
// as base 16 (hex). If 012 it assumes octal etc. Otherwise it assume base 10.
// What happens if the value is too large for the defined type, or other invalid values?
// Exceptions:
// std::invalid_argument if no conversion could be performed
// std::out_of_range if the converted value would fall out of the range of the result type or if the underlying
// function sets errno to ERANGE.
// The conversion functions: std::stoi, std::stol, std::stoull convert to an integer width that might be different from
// the width we want.
// For example, std::stoi creates a signed 32-bit integer as a result. But if our target type is int16_t, then we
// cannot rely on the std::out_of_range exception to correctly catch range violations here.
// Thus, we also add additional checks on the limits if the out_of_range didn't catch it.
// Caller must choose the correct version that uses a type that is large enough for the target type we are converting to
// The caller function IntType::format(...) computes the widths and choose correct function to call, using the second
// template arg to decide which std::sto* function to call.
//
// Rules for negative numbers when user provides hex input:
// If the number was a hex input, we allow the user to produce a negative number if the hex input has the correct byte
// size for the type (and the number is in fact a negative number). For example:
// 0xfe is -2 for int8_t. But, 0x00fe is 254 for int16_t, even though 0xfe and 0x00fe is the same number numerically.
// In other words, assume that the user wants to see the negative if they give the exact byte size matching.

template <typename T, typename I>
void IntType::format(std::vector<FmtType::FmtColumn> &formattedCols, const std::string &value)
{
    bool rangeError = false;
    T valueAsType = 0;

    if (std::numeric_limits<T>::digits > std::numeric_limits<I>::digits) {
        std::string errMsg("Type too large for formatting. Number type width: ");
        errMsg += std::to_string(std::numeric_limits<T>::digits) + " and std::sto* width: "
            + std::to_string(std::numeric_limits<I>::digits);
        THROW_FMT_EXCEPTION(errMsg);        
    }

    // call the atoi wrapper using the type size specification template arg.
    // This is not the final number, just the result of the atoi call representated as the type from the atoi call.
    I intValue = stringToNum<I>(value, rangeError);

    // clean this up later.  the if statements can be reworked I think...notice the duplicate value as type and
    // funny range error check things
    if (isSigned_ && value.compare(0,2, "0x") == 0 && value[2] != '0' && ((value.size() - 2) / 2) == sizeof(T)) {
        valueAsType = intValue;
    } else {
        if (intValue < std::numeric_limits<T>::min() || intValue > std::numeric_limits<T>::max()) {
            // The int value is bigger width than the types we are checking. Thus, manually check the limits of the type
            // here to produce range errors in case the "catch" didn't get it.
            rangeError = true;
        }

        if (!rangeError) {
            valueAsType = intValue;  // range already checked, this is safe downcast
        }
    }

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
        fmtNumToHex<T>(formattedCols, valueAsType);
    }   
}

template <typename T>
void IntType::fmtNumToHex(std::vector<FmtType::FmtColumn> &formattedCols, T valueAsType)
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

template <typename I>
I IntType::stringToNum(const std::string &value, bool &rangeError)
{
    // I should change this to a compile time error using template skills
    // basically, we only want to support the 3 known type which are each specialized.
    std::string errMsg("Invalid numeric type conversion call. Current support for stoi, stol, and stoul");
    errMsg += " (int, long int, and unsigned long long int respectively)";
    THROW_FMT_EXCEPTION(errMsg);
}
