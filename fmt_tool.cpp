#include "fmt_tool.h"
#include "fmt_type.h"

FmtTool::FmtTool()
{
    // Populate the formatting type map argument options.
    fmtTypeArgMap_["-i"] = FmtTypeArg::kInt;
    fmtTypeArgMap_["-a"] = FmtTypeArg::kAscii;
    fmtTypeArgMap_["-b"] = FmtTypeArg::kBinary;
}

void FmtTool::parseArgStream(std::stringstream *arg_stream)
{
}

void FmtTool::showHelp()
{
}
