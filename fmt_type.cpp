#include "fmt_type.h"
#include <string>
#include "fmt_exception.h"
#include "fmt_tool.h"

const std::string FmtType::OUT_OF_RANGE = "<out_of_range>";
const std::string FmtType::INVALID = "<invalid>";

// Base methods
FmtType::FmtType(FmtTool *parent) : parentTool_(parent)
{
}
