#include "fmt_type.h"
#include <string>
#include "fmt_exception.h"


// Base methods
FmtType::FmtType()
{
}

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
    std::cout << "doing an integer format (" << width_ << ") on value: " << value << std::endl;
}

void IntType::getTitleRow(std::vector<FmtType::FmtColumn> &titleRow,
                          std::vector<FmtType::FmtColumn> &underscoreRow) const
{
}
