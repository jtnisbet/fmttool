#include "cmd_arg.h"
#include <string>
#include "fmt_exception.h"

CmdArg::CmdArg(CmdArgType argType, size_t width) : type_(argType), width_(width)
{
    switch (argType) {
       case CmdArgType::INT: {
           if (width_ != 8 && width != 16 && width_ != 32 && width_ != 64) {
               THROW_FMT_EXCEPTION("Invalid width value for integer format (-i <width>). Must be 8, 16, 32, or 64.");
           }
           break;
       }
       case CmdArgType::HELP: {
           if (width_ != 0) {
               THROW_FMT_EXCEPTION("Help command should not have been created with a non-zero width");
           }
           break;
       }
       default: {
           THROW_FMT_EXCEPTION("Invalid CmdArgArg");
           break;
       }
    }
}

std::string CmdArg::toString()
{
    std::string retStr;
    switch (type_) {
        case CmdArgType::INT: {
            retStr = "int (width = " + std::to_string(width_) + ")";
            break;
        }
        case CmdArgType::HELP: {
            retStr = "help command";
            break;
        }
        default: {
            THROW_FMT_EXCEPTION("Unknown command arg type.");
            break;
        }
    }
    return retStr;
}

void CmdArg::format(std::vector<FmtColumn> &formattedCols, const std::string &value)
{
    // Design is composition rather than inheritence. We have to execute the proper type formatter here.
    switch (type_) {
        case CmdArgType::INT: {
            intFormat(formattedCols, value);
            break;
        }
        default: {
            THROW_FMT_EXCEPTION("No format support for this type.");
            break;
        }
    }
}

