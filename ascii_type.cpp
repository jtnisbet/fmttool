#include "ascii_type.h"
#include <iomanip>
#include <string>
#include <sstream>
#include "fmt_exception.h"
#include "fmt_type.h"
#include "fmt_tool.h"


AsciiType::AsciiType(FmtTool *parent) : FmtType(parent)
{
}

std::string AsciiType::toString() const
{
    std::string retStr("ascii");
    return retStr;
}

void AsciiType::format(std::vector<FmtColumn> &formattedCols, const std::string &value)
{
    std::stringstream ss;
    std::string formattedData;
    ss << "0x" << std::hex;
    for (int i = 0; i < value.size();  ++i) {
        ss << static_cast<int>(value[i]);
    }
    formattedData = ss.str();
    formattedCols.emplace_back(formattedData, formattedData.size());

}

void AsciiType::getTitleRow(std::vector<FmtType::FmtColumn> &titleRow1, std::vector<FmtType::FmtColumn> &titleRow2,
                            std::vector<FmtType::FmtColumn> &underscoreRow) const
{
    const std::string BASE_16 = "Hex";
    std::string widthName = this->toString();
    
    // This type only provides 1 column: the hex bytes of the ascii string.
    // Empty string instructs the displayer code to write '-' characters to make an underline.
    titleRow1.emplace_back(BASE_16, BASE_16.size());
    titleRow2.emplace_back(widthName, widthName.size());
    underscoreRow.emplace_back("", BASE_16.size());
}
