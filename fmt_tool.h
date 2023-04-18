#pragma once

#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <unordered_map>
#include <vector>
#include "cmd_arg.h"

// A template specialization for std::less so that std::set can work with unique ptr's but uses
// the object itself for positioning and comparisons in the set.
template<>
struct std::less<std::unique_ptr<CmdArg>> {
    bool operator()(const std::unique_ptr<CmdArg> &a, const std::unique_ptr<CmdArg> &b) const {
        return *a < *b;
    }
};

class FmtTool {
public:
    static const std::string DFT_ARGS;
    FmtTool();
    ~FmtTool() = default;
    void parseArgs(std::stringstream *argStream);
    void showFormatRequests();
    bool showHelp();
    void executeFormatting();
private:
    void formatAndDisplay(const std::string &value);
    std::unordered_map<std::string, CmdArg::CmdArgType> cmdArgTypeMap_;
    std::set<std::unique_ptr<CmdArg>> cmdArgs_;
    std::unique_ptr<std::istringstream> iSStream_;
    std::istream *inStream_;
    bool helpRequested_;
    std::vector colWidths_;
};

