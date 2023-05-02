#pragma once

#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <unordered_map>
#include <vector>
#include "fmt_type.h"

// A template specialization for std::less so that std::set can work with unique ptr's but uses
// the object itself for positioning and comparisons in the set.
template<>
struct std::less<std::unique_ptr<FmtType>> {
    bool operator()(const std::unique_ptr<FmtType> &a, const std::unique_ptr<FmtType> &b) const {
        return *a < *b;
    }
};

class FmtTool {
public:
    enum class CmdArg : int8_t {
        INT = 1,
        ASCII = 2,
        BINARY = 3,
        HELP = 4
    };

    static const std::string DFT_ARGS;
    FmtTool();
    ~FmtTool() = default;
    void parseArgs(std::stringstream *argStream);
    void showFormatRequests();
    bool showHelp();
    void addTitles();
    void executeFormatting();
private:
    using ResultTable = std::vector<std::vector<FmtType::FmtColumn>>;  // rows of columns
    void addToResultTable(const std::string &value);
    void prepareTableForDisplay();
    void displayResultsTable();
    std::unordered_map<std::string, CmdArg> cmdArgMap_;
    std::set<std::unique_ptr<FmtType>> fmtTypes_;
    std::unique_ptr<std::istringstream> iSStream_;
    std::istream *inStream_;
    bool helpRequested_;
    ResultTable results_;
};

