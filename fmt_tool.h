#pragma once

#include <vector>
#include <sstream>
#include "fmt_type.h"

class FmtTool {
public:
    FmtTool();
    ~FmtTool() = default;
    void parseArgStream(std::stringstream *arg_stream);
    void showHelp();
private:
    std::unordered_map<std::string, FmtTypeArg> fmtTypeArgMap_;
    std::stringstream userValues_;
    std::vector<FmtType> fmtTypes_;
};
