// included directly from int_type.h
// Put in this file to separate implementation from the class

// A note on the formatting:
// The conversion functions: std::stoi, std::stol, std::stoull convert to an integer width that might be different from
// the width we want.
// For example, std::stoi creates a signed 32-bit integer as a result. But if our target type is int16_t, then we
// cannot rely on the std::out_of_range exception to correctly catch range violations here.
// Thus, we also add additional checks on the limits if the out_of_range didn't catch it.
// Caller must choose the correct version that uses a type that is large enough for the target type we are converting to
// The caller function IntType::format(...) computes the widths and choose correct function to call, using the second
// template arg to decide which std::sto* function to call.
// Example: format<int16_t, int>(..) will create a target type of int32_t, and it will use stoi function that returns
// "int" datatype as an intermediate value.
// Thus, after capturing the number into the intermediate type, it must do range checking before finalizing the value
// to the real target type.
//
// Rules for negative numbers when user provides hex input:
// If the number was a hex input, we allow the user to produce a negative number if the hex input has the correct byte
// size for the type (and the number is in fact a negative number). For example:
// 0xfe is -2 for int8_t. But, 0x00fe is 254 for int16_t, even though 0xfe and 0x00fe is the same number numerically.
// In other words, assume that the user wants to see the negative if they give the exact byte size matching.

template <typename T, typename I>
void IntType::format(std::vector<FmtType::FmtColumn> &formattedCols, const std::string &value)
{
    ErrType err = ErrType::FmtErrNone;
    T valueAsType = 0;

    if (std::numeric_limits<T>::digits > std::numeric_limits<I>::digits) {
        std::string errMsg("Type too large for formatting. Number type width: ");
        errMsg += std::to_string(std::numeric_limits<T>::digits) + " and std::sto* width: "
            + std::to_string(std::numeric_limits<I>::digits);
        THROW_FMT_EXCEPTION(errMsg);        
    }

    // call the atoi wrapper using the type size specification template arg.
    // This is not the final number, just the result of the atoi call representated as the type from the atoi call.
    I intValue = stringToNum<I>(value, err);

    // down cast the intermediate value to the target type.  This is only safe if its in the valid type range, and
    // also if we meet the conditions for hex input bit width.
    if (err == ErrType::FmtErrNone) {
        if ((isSigned_ && value.compare(0,2, "0x") == 0 && value[2] != '0' && ((value.size() - 2) / 2) == sizeof(T)) ||
            intValue >= std::numeric_limits<T>::min() && intValue <= std::numeric_limits<T>::max()) {
            valueAsType = intValue;
        } else {
            err = ErrType::FmtErrRange;
        }
    }

    // First column is the base 10 version of the data
    if (err == ErrType::FmtErrRange) {
        formattedCols.emplace_back(OUT_OF_RANGE, OUT_OF_RANGE.size());
    } else if (err == ErrType::FmtErrInvalid) {
        formattedCols.emplace_back(INVALID, INVALID.size());
    } else {
        std::string formattedData = std::to_string(valueAsType);
        formattedCols.emplace_back(formattedData, formattedData.size());
    }

    // The next column will be the hex format of the number. Ensure leading zeros match the bitwidth.
    if (err == ErrType::FmtErrRange) {
        formattedCols.emplace_back(OUT_OF_RANGE, OUT_OF_RANGE.size());
    } else if (err == ErrType::FmtErrInvalid) {
        formattedCols.emplace_back(INVALID, INVALID.size());
    } else {
        fmtNumToHex<T>(formattedCols, valueAsType);
    }

    if (!parentTool_->IsBinaryFmtSuppressed()) {
        // Third column is the binary representation of the number
        if (err == ErrType::FmtErrRange) {
            formattedCols.emplace_back(OUT_OF_RANGE, OUT_OF_RANGE.size());
        } else if (err == ErrType::FmtErrInvalid) {
            formattedCols.emplace_back(INVALID, INVALID.size());    
        } else {
            // Quick way to see the binary is to convert to bitset and then display that.
            std::bitset<sizeof(T) * 8> binValue(valueAsType);
            std::string formattedData = binValue.to_string();
            formattedCols.emplace_back(formattedData, formattedData.size());        
        }
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
