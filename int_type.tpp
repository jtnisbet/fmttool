// included directly from int_type.h
// Put in this file to separate implementation from the class

template <typename T>
void IntType::format(std::vector<FmtType::FmtColumn> &formattedCols, const std::string &value)
{
    bool rangeError = false;
    T valueAsType;
    if (std::numeric_limits<T>::digits <= std::numeric_limits<int>::digits) {
        // If the number of bits for this type can fit in an integer, then we'll use the std::stoi
        // integer version of the code to convert the string to int.
        valueAsType = StringToNumUsingInt<T>(value, rangeError);
    } else if (std::numeric_limits<T>::digits <= std::numeric_limits<long int>::digits) {
        // If the number of bits for this type is too big for an int, the next size to use is the long int version.
        valueAsType = StringToNumUsingLongInt<T>(value, rangeError);
    } else if (std::numeric_limits<T>::digits <= std::numeric_limits<unsigned long long int>::digits) {
        // If the number of bits for this type is too big for an int, the next size to use is the long long int version.
        valueAsType = StringToNumUsingLongLongInt<T>(value, rangeError);
    } else {
        std::string errMsg("Type too large for formatting. Number type width: ");
        errMsg += std::to_string(std::numeric_limits<T>::digits) + " and std::stoll width: "
            + std::to_string(std::numeric_limits<long long int>::digits);
        // Shouldn't really be possible to hit this path.
        THROW_FMT_EXCEPTION(errMsg);
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

// A note on the formatting:
// std::stoi has the ability to read the negative sign, as well as 0x prefix to convert string hex intputs. The 3rd arg
// is the "base". Special value of 0 means it will try to auto-detect. For example, if the input is 0x12 it will format
// as base 16 (hex). If 012 it assumes octal etc. Otherwise it assume base 10.
// What happens if the value is too large for the defined type, or other invalid values?
// Exceptions:
// std::invalid_argument if no conversion could be performed
// std::out_of_range if the converted value would fall out of the range of the result type or if the underlying
// function sets errno to ERANGE.
// The conversion functions: std::stoi, std::stol, std::stoll convert to an integer width that might be different from
// the width we want.
// For example, std::stoi creates a signed 32-bit integer as a result. But if our target type is int16_t, then we
// cannot rely on the std::out_of_range exception to correctly catch range violations here.
// Thus, we also add additional checks on the limits if the out_of_range didn't catch it.
// Lastly, we provide 3 version:
// StringToNumUsingInt
// StringToNumUsingLongInt
// StringToNumUsingLongLongInt
// Caller must choose the correct version that uses a type that is large enough for the target type we are converting to
// The caller function IntType::format(...) computes the widths and choose correct function to call.

template <typename T>
T IntType::StringToNumUsingInt(const std::string &value, bool &rangeError)
{
    T valueAsType = 0;
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
    
    // The int value is bigger width than the types we are checking. Thus, manually check the limits of the type
    // here to produce range errors in case the "catch" didn't get it.
    if (intValue < std::numeric_limits<T>::min() || intValue > std::numeric_limits<T>::max()) {
        rangeError = true;
    }

    if (!rangeError) {
        valueAsType = intValue;  // range already checked, this is safe downcast
    }

    return valueAsType;
}

template <typename T>
T IntType::StringToNumUsingLongInt(const std::string &value, bool &rangeError)
{
    T valueAsType = 0;
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
    
    // The int value is bigger width than the types we are checking. Thus, manually check the limits of the type
    // here to produce range errors in case the "catch" didn't get it.
    if (intValue < std::numeric_limits<T>::min() || intValue > std::numeric_limits<T>::max()) {
        rangeError = true;
    }

    if (!rangeError) {
        valueAsType = intValue;  // range already checked, this is safe downcast
    }

    return valueAsType;
}

template <typename T>
T IntType::StringToNumUsingLongLongInt(const std::string &value, bool &rangeError)
{
    T valueAsType = 0;
    unsigned long long int intValue;
    // There does not exist any formatter that converts directly to an 8-bit int, so we use the int version first and
    // then assign it down if its in the valid range. We do this because we want to correctly show the range error
    // if the data given exceeds the type width.
    try {
        intValue = std::stoull(value, nullptr, 0);
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
    
    // The int value is bigger width than the types we are checking. Thus, manually check the limits of the type
    // here to produce range errors in case the "catch" didn't get it.
    if (intValue < std::numeric_limits<T>::min() || intValue > std::numeric_limits<T>::max()) {
        rangeError = true;
    }

    if (!rangeError) {
        valueAsType = intValue;  // range already checked, this is safe downcast
    }

    return valueAsType;
}
