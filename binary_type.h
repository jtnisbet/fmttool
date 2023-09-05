#pragma once
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include "fmt_type.h"
#include "fmt_tool.h"

class BinaryType : public FmtType {
public:
    BinaryType(FmtTool *parent);
    ~BinaryType() = default;
    std::string toString() const override;
    void format(std::vector<FmtType::FmtColumn> &formattedCols, const std::string &value) override;
    size_t getCompareHash() const override
    {
        // Unlike the IntType which supports things like -i 16 -i 32 where you can have more than
        // one formatting request for the same type, for ascii, only one is possible.
        // ex: -a -a -a hello
        // We want to eliminate all the -a's so there is only one. The comparison hash is simple
        // then...its always the same.
        return std::hash<size_t>()(0);
    }
    void getTitleRow(std::vector<FmtType::FmtColumn> &titleRow1, std::vector<FmtType::FmtColumn> &titleRow2,
                     std::vector<FmtType::FmtColumn> &underscoreRow) const override;
private:

};
