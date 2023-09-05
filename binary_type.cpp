#include "binary_type.h"
#include <iomanip>
#include <string>
#include <sstream>
#include "fmt_exception.h"
#include "fmt_type.h"
#include "fmt_tool.h"


BinaryType::BinaryType(FmtTool *parent) : FmtType(parent)
{
}

std::string BinaryType::toString() const
{
    std::string retStr("binary");
    return retStr;
}

void BinaryType::format(std::vector<FmtColumn> &formattedCols, const std::string &value)
{
    const int MAX_STR_LEN = 160; // limit the length of input string to 160 characters (80 bytes)
    // Data must start with 0x and have even number of bytes. Otherwise it is not valid.
    if (value.compare(0,2, "0x") != 0 || (value.size() % 2 != 0) || value.size() > MAX_STR_LEN ) {
        formattedCols.emplace_back(INVALID, INVALID.size());
        return;
    }

    std::stringstream ss;  // build the string in this stream

    // length is already sanity checked to be even, and starts with 0x.
    // iterate every 2 characters to get the bytes.
    // example 0x123456 is processing 12, 34, 56  (as hex numbers)
    // std::stoi() has built in logic to parse hex digits and convert to the numeric format.
    // so for example, "0x55" as a string will return the decimal number 85 as an int
    // This is what we want, and then cast it to its ascii char.
    for (size_t i = 2; i < value.size(); i +=2) {
        std::string byteStr = value.substr(i, 2);
        int charAsInt = std::stoi(byteStr, nullptr, 16);
        // Sanity check that the byte falls within the range of printable ascii characters.
        // If it does not, then we will write a white space character in its place
        // Here for printable characters we'll use extended ascii that goes up to 0xff.
        // No support for different multi-byte characters and codepages. Seems my own terminal isn't showing
        // UTF-8 anyway, not sure how to fix it.
        if (charAsInt > 31 && charAsInt < 256) {
            ss << static_cast<char>(charAsInt);
        } else {
            ss << " ";
        }
    }

    std::string formattedData;
    formattedData = ss.str();
    formattedCols.emplace_back(formattedData, formattedData.size());

}

void BinaryType::getTitleRow(std::vector<FmtType::FmtColumn> &titleRow1, std::vector<FmtType::FmtColumn> &titleRow2,
                            std::vector<FmtType::FmtColumn> &underscoreRow) const
{
    const std::string ASCII = "ascii from";
    std::string widthName = this->toString();
    
    // This type only provides 1 column: the ascii string from the input binary data.
    // Empty string instructs the displayer code to write '-' characters to make an underline.
    titleRow1.emplace_back(ASCII, ASCII.size());
    titleRow2.emplace_back(widthName, widthName.size());
    underscoreRow.emplace_back("", ASCII.size());
}
