#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <typeinfo>
#include <vector>

class FmtType {
public:
    using FmtColumn = std::pair<std::string, size_t>;

    FmtType();
    virtual ~FmtType() = default;

    // For use with std::set comparison and duplicate elimination
    bool operator<(const FmtType &other) const
    {
        if (typeid(*this) == typeid(other)) {
            return this->getCompareHash() < other.getCompareHash();
        }
        return typeid(*this).hash_code() < typeid(other).hash_code();
    }
    virtual std::string toString() const = 0;
    virtual void format(std::vector<FmtType::FmtColumn> &formattedCols, const std::string &value) = 0;
    virtual size_t getCompareHash() const = 0;
    virtual void getTitleRow(std::vector<FmtType::FmtColumn> &titleRow,
                             std::vector<FmtType::FmtColumn> &underscoreRow) const = 0;
};

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
    size_t width_;
};
