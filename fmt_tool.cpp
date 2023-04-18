#include "fmt_tool.h"
#include <memory>
#include "cmd_arg.h"
#include "fmt_exception.h"

const std::string FmtTool::DFT_ARGS = "-i 32";

FmtTool::FmtTool() : iSStream_(nullptr), inStream_(nullptr), helpRequested_(false)
{
    using argT = CmdArg::CmdArgType;
    // Populate the formatting type map argument options.
    // This is done so that we may do switch during argument parsing of the input args
    cmdArgTypeMap_["-i"] = argT::INT;
    cmdArgTypeMap_["-a"] = argT::ASCII;
    cmdArgTypeMap_["-b"] = argT::BINARY;
    cmdArgTypeMap_["-h"] = argT::HELP;
}

void FmtTool::parseArgs(std::stringstream *argStream)
{
    using argT = CmdArg::CmdArgType;
    // populate both the vector of formatting types as well as the user values that we want to format.
    // For example, the user may specify a list of formats they want to see as well as a list of values they want to
    // format.
    // Example: -i 32 -i 64 -a 13 45 79
    // This means: format the values "13", "45" and "79" as 32 bit integer, 64 bit integer, and ascii characters

    // Iterate over the stream of arguments
    std::string userValues;
    std::string tok;
    int argsProcessed = 0;
    while (*argStream >> tok) {
        bool makeCmdArg = true;
        size_t typeWidth = 0;  // not all types need a width.  default of 0 is ok.
        argT currType = cmdArgTypeMap_[tok];
        switch(currType) {
            // -i <width>
            case (argT::INT): {
                // A width argument is required. Fetch it from the arg stream, converted to size_t
                if (!(*argStream >> typeWidth)) {
                    THROW_FMT_EXCEPTION("-i argument requires a width argument. (See fmttool -h for help)");
                }
                break;
            }
            // -h for help. Does not have any args.
            case (argT::HELP): {
                helpRequested_ = true;
                break;
            }
            // Assume any other arg data are the users data values to format. Append these to a string which we will
            // later convert into an istream for parsing.
            default: {
                userValues += " " + tok;
                makeCmdArg = false;
                break;
            }
        }

        if (makeCmdArg) {
            auto cmdArg = std::make_unique<CmdArg>(currType, typeWidth);
            cmdArgs_.insert(std::move(cmdArg));  // std::set eliminates duplicates
        }
    }

    // If there were no args given (only user values), then assign a default formatting config.
    if (cmdArgs_.empty()) {
        // For consistency, this should match the DFT_ARGS variable options
        auto cmdArg = std::make_unique<CmdArg>(argT::INT, 32);
        cmdArgs_.insert(std::move(cmdArg));
    }

    if (!userValues.empty()) {
        // Create an istringstream with unique ptr.  This will be destroyed by destructor.
        // Save a copy of this pointer into the inStream_ reference.  This does not get destroyed as it is a reference
        // only.
        // This is the user data that they gave on command line.
        iSStream_ = std::make_unique<std::istringstream>(userValues);
        inStream_ = iSStream_.get();
    } else {
        // user didn't give any data, so we assume the data is coming from live input, either from command line or from
        // an input pipe.
        // iSStream remains nullptr and is not used
        inStream_ = &std::cin;
    }
}

void FmtTool::showFormatRequests()
{
    std::cout << "Requested formatting types:\n";
    for (auto &cmdArg : cmdArgs_) {
        std::cout << cmdArg->toString() << "\n";
    }
    std::cout << std::endl;
}

bool FmtTool::showHelp()
{
    // If ANY of the arguments was the help arg then we ignore all args and just show the help
    // Complete this later once I know more about how I'm doing this.
    if (helpRequested_) {
        std::cout << "Description:\nfmttool formats input data into different output formats specified by the options.\n";
        std::cout << "Multiple options can be provided.\n";
        std::cout << "\nUsage: fmttool [ <options> ]\n";
        std::cout << "\nOptions:\n    [ -i <width> ]: Format the data as an integer type at the given bit width\n";
        std::cout << "\n    [ <value> ]: User provided data value to format.\n";
        std::cout << "\n    [ -h ]: Shows this help text.\n";
        std::cout << "\nExample: Format the numbers 12 and 78 as a 16-bit integer and a 64-bit integer:\n";
        std::cout << "fmttool -i 16 -i 64 12 78\n";
        std::cout << std::endl;
    }
    return helpRequested_;
}

void FmtTool::executeFormatting()
{
    // We have a list of values coming from our chosen input stream (it may be a istringtream or it might be std::cin).
    // For each value, execute the requested formatting against that value.
    bool moreData = true;
    std::string currValue;
    while (moreData) {
        if (*inStream_ >> currValue) {
            formatAndDisplay(currValue);
        } else if (inStream_->eof()) {
            // normal exit case.  We reached the end of the stream, or pipe/stream ended
            moreData = false;
        } else if (inStream_->bad()) {
            THROW_FMT_EXCEPTION("Input stream error.");
        } else {
            // Should not happen.
            THROW_FMT_EXCEPTION("Unexpected stream error.");
        }   
    }
}

void FmtTool::formatAndDisplay(const std::string &value)
{
    // for each format request, drive the formatting against the data.
    // Formatting the data produces a vector of pairs.
    std::vector<CmdArg::FmtColumn> outputCols;
    for (const auto &argCmd : cmdArgs_) {
        // given the value, format this into columns.  A single ArgCmd may result in different column outputs for a
        // value.  For example, formatting the number 99 as a 32-bit integer will produce the following row of data:
        // 
        argCmd->format(outputCols, value);
    }
}
