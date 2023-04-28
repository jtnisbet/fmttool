#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <typeinfo>
#include <vector>
#include "fmt_type.h"

class IntType : public FmtType {
public:
    IntType(size_t width);
    ~IntType() = default;
    std::string toString() const override;
    void format(std::vector<FmtType::FmtColumn> &formattedCols, const std::string &value) override;
    size_t getCompareHash() const override
    {
        return std::hash<size_t>()(width_);
    }
    void getTitleRow(std::vector<FmtType::FmtColumn> &titleRow,
                     std::vector<FmtType::FmtColumn> &underscoreRow) const override;
private:
    void format8(std::vector<FmtType::FmtColumn> &formattedCols, const std::string &value);
    void format16(std::vector<FmtType::FmtColumn> &formattedCols, const std::string &value);
    void format32(std::vector<FmtType::FmtColumn> &formattedCols, const std::string &value);
    void format64(std::vector<FmtType::FmtColumn> &formattedCols, const std::string &value);    
    size_t width_;
};
