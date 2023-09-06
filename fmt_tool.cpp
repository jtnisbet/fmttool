#include "fmt_tool.h"
#include <iomanip>
#include <memory>
#include "ascii_type.h"
#include "binary_type.h"
#include "fmt_type.h"
#include "fmt_exception.h"
#include "int_type.h"

const std::string FmtTool::DFT_ARGS = "-i 32";
const int FmtTool::COL_SPACE = 2;  // Provide 2 whitespaces in between each column

FmtTool::FmtTool() : iSStream_(nullptr), inStream_(nullptr), helpRequested_(false)
{
    // Populate the formatting type map argument options.
    // This is done so that we may do switch during argument parsing of the input args
    cmdArgMap_["-i"] = CmdArg::INT;           // Input is assumed to be a signed int data
    cmdArgMap_["-u"] = CmdArg::UINT;          // Input is assumed to be an unsigned int data
    cmdArgMap_["-a"] = CmdArg::ASCII;         // Input is assumed to be a string
    cmdArgMap_["-b"] = CmdArg::BINARY;        // Input is assume to be an array of bytes in hex (prefixed with 0x..)
    cmdArgMap_["-nobin"] = CmdArg::SUPP_BIN;  // Supress binary ouput for integer types
    cmdArgMap_["-h"] = CmdArg::HELP;
}

void FmtTool::parseArgs(std::stringstream *argStream)
{
    // populate both the vector of formatting types as well as the user values that we want to format.
    // For example, the user may specify a list of formats they want to see as well as a list of values they want to
    // format.
    // Example: -i 32 -i 64 -a 13 45 79
    // This means: format the values "13", "45" and "79" as 32 bit integer, 64 bit integer, and ascii characters

    // Iterate over the stream of arguments
    std::string userValues;
    std::string tok;
    int argsProcessed = 0;
    std::unique_ptr<FmtType> newType = nullptr;
    while (*argStream >> tok) {
        size_t typeWidth = 0;  // not all types need a width.  default of 0 is ok.
        CmdArg currArg = cmdArgMap_[tok];
        switch(currArg) {
            // -i <width>
            case (CmdArg::INT):
            case (CmdArg::UINT): {
                // A width argument is required. Fetch it from the arg stream, converted to size_t
                if (!(*argStream >> typeWidth)) {
                    THROW_FMT_EXCEPTION("-i and -u types require a width argument. (See fmttool -h for help)");
                }
                bool isSigned = (currArg == CmdArg::INT) ? true : false;
                // base class pointer of derived class type
                newType = std::make_unique<IntType>(typeWidth, isSigned, this);
                fmtTypes_.insert(std::move(newType));  // std::set eliminates duplicates
                break;
            }
            case (CmdArg::ASCII): {
                newType = std::make_unique<AsciiType>(this);
                fmtTypes_.insert(std::move(newType));  // std::set eliminates duplicates
                break;
            }
            case (CmdArg::BINARY): {
                newType = std::make_unique<BinaryType>(this);
                fmtTypes_.insert(std::move(newType));  // std::set eliminates duplicates
                break;
            }
            // -nobin option suppresses the binary output column display for integer types (because it can be long and
            // maybe the user doesn't want it)
            case (CmdArg::SUPP_BIN): {
                noBin_ = true;
                break;
            }
            // -h for help. Does not have any args.
            case (CmdArg::HELP): {
                helpRequested_ = true;
                break;
            }
            // Assume any other arg data are the users data values to format. Append these to a string which we will
            // later convert into an istream for parsing.
            default: {
                userValues += " " + tok;
                break;
            }
        }
    }

    // If there were no args given for type format requests (only user values), then assign a dft formatting config.
    // User will get failures though if the data isn't the default here (say its ascii or something)
    if (fmtTypes_.empty()) {
        // For consistency, this should match the DFT_ARGS variable options
        newType = std::make_unique<IntType>(32, true, this);  // base class pointer of derived class type
        fmtTypes_.insert(std::move(newType));     // std::set eliminates duplicates
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

bool FmtTool::showHelp()
{
    // If ANY of the arguments was the help arg then we ignore all args and just show the help
    // Complete this later once I know more about how I'm doing this.
    if (helpRequested_) {
        std::cout << "Description:\nfmttool formats input data into different output formats specified by the options.\n"
                  << "Multiple options and input data can be provided.\n"
                  << "Supports piped input data (See examples)\n"
                  << "\nUsage: fmttool [ <options> ... ] [ user_data ... ]\n"
                  << "\noptions:\n"
                  << "    -i width\n"
                  << "       Format the data as a signed integer type at the given bit width.\n"
                  << "       (Supported bit-widths: 8,16,32,64)\n"
                  << "    -u width\n"
                  << "       Format the data as an unsigned integer type at the given bit width\n"
                  << "       (Supported bit-widths: 8,16,32,64)\n"
                  << "    -a\n"
                  << "       Format the data as input ascii characters, showing their hexadecimal values for each character.\n"
                  << "       Assumes single byte ascii characters. UTF8 or graphic/multi-byte characters not suppored.\n"
                  << "       From the shell, enclose bigger strings in \" characters if there are whitespace characters in the data.\n"
                  << "    -b\n"
                  << "       Formats the data into ascii characters. Input must be in the format of hexademical data prefixed with 0x\n"
                  << "       Input data must contain even number of charactes so that bytes are well-formed (nibbles are not suppported).\n"
                  << "       Correct example: 0x51    Invalid example: 0x4\n"
                  << "    -h\n"
                  << "       Shows this help text.\n"
                  << "\nuser_data\n"
                  << "    Any data can be input.\n"
                  << "\nExamples:\n"
                  << "    Format the numbers 12 and 78 as 16-bit signed integer and 64-bit unsigned integer:\n"
                  << "       fmttool -i 16 -u 64 12 78\n"
                  << "    Format the strings \"hello\" and \"world\" individually, given as input from a pipe in ascii mode\n"
                  << "       echo \"hello world\" | fmttool -a\n"
                  << std::endl;
    }
    return helpRequested_;
}

void FmtTool::addTitles()
{
    const std::string INPUT_TITLE = "input";
    FmtColList titleRow1;
    FmtColList titleRow2;
    FmtColList underscoreRow;

    // for each format type request, do a lookup into the titles and widths for a title bar of the table.
    // The title consists of 3 lines, the titles themselves have 2 lines, and the 3rd line is an underscore line.
    // Getting these titles produces a vector of pairs (data paired with display width).

    // First column, the user input.  We always show this as the first column. The first row of this title is
    // empty string.
    titleRow1.emplace_back("", INPUT_TITLE.size());
    titleRow2.emplace_back(INPUT_TITLE, INPUT_TITLE.size());
    underscoreRow.emplace_back("", INPUT_TITLE.size());

    // Then, add the rest of the columns.  Each FmtType might add more than one
    for (const auto &fmtType : fmtTypes_) {
        fmtType->getTitleRow(titleRow1, titleRow2, underscoreRow);
    }
    results_.push_back(titleRow1);  // adds the title row
    results_.push_back(titleRow2);  // adds the title row
    results_.push_back(underscoreRow);  // adds the underscore row
}

void FmtTool::executeFormatting()
{
    // We have a list of values coming from our chosen input stream (it may be a istringtream or it might be std::cin).
    // For each value, execute the requested formatting against that value.
    bool moreData = true;
    bool enclosedData = false;
    std::string currValue;
    std::string compoundString;
    addTitles();
    while (moreData) {
        if (*inStream_ >> currValue) {
            if (!enclosedData) {
                if (currValue[0] == '\2') {
                    // An enclosed string.  Strip the STX character and start creating our compound string.
                    enclosedData = true;
                    compoundString = currValue.substr(1, currValue.size() - 1);
                } else {
                    // Normal case, we are not in an enclused string and its just a new single token of data.
                    addToResultTable(currValue);  // formats the value into the result table
                }
            } else {
                // We are already in enclosed data string. if the token does not end in the ETX, append it.
                // It the token ends in ETX, append it without the ETX, and then drive the format work.
                if (currValue[currValue.size() - 1] == '\3') {
                    compoundString += " " + currValue.substr(0, currValue.size() - 1);
                    addToResultTable(compoundString);
                } else {
                    compoundString += " " + currValue;
                }
            }
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
    // The table of formatted data is created. Now, do a pass through it to compute column widths for nice display.
    prepareTableForDisplay();
}

void FmtTool::addToResultTable(const std::string &value)
{
    // for each format type request, drive the formatting against the data.
    // Formatting of each fmt type appends to a vector of pairs (data paired with display width)
    FmtColList outputCols;
    outputCols.emplace_back(value, value.size());  // Always add the user input string as first column
    for (const auto &fmtType : fmtTypes_) {
        fmtType->format(outputCols, value);
    }
    results_.push_back(outputCols);  // adds this formatted row to the result table
}

void FmtTool::prepareTableForDisplay()
{
    // Loops over all the data and find a common width for each column displaying the table
    auto resultsIter = std::begin(results_);
    std::vector<size_t> savedWidths(resultsIter->size(), 0);  // all saved widths have 0 width to start.
    while (resultsIter != std::end(results_)) {
        // for each column of the row, check if the width is greater than the saved width for that column
        auto savedWidthsIter = std::begin(savedWidths);
        auto colIter = std::cbegin(*resultsIter);
        while (colIter != std::cend(*resultsIter)) {
            if (colIter->second > *savedWidthsIter) {
                (*savedWidthsIter) = colIter->second;  // assign the larger value to the final width
            }
            ++savedWidthsIter;
            ++colIter;
        }
        ++resultsIter;
    }

    // Now that the optimal widths are computed, change all the widths of each value
    // Technically this is a bit wasteful since we only have one row of final widths, but its easier for the
    // displayer code if doesn't have to iterate a second vector.
    for (auto &currentRow : results_) {
        auto colIter = std::begin(currentRow);
        auto savedWidthsIter = std::begin(savedWidths);
        while (colIter != std::end(currentRow)) {
            colIter->second = *savedWidthsIter;
            ++savedWidthsIter;
            ++colIter;
        }
    }
}

void FmtTool::showRow(ResultTable::const_iterator &resultsIter)
{
    for (const auto &colPair : *resultsIter) {
        std::cout << std::setw(colPair.second) << colPair.first << std::setfill(' ') << std::setw(COL_SPACE) << "";
    }
    std::cout << "\n"; 
}

void FmtTool::displayResultTable()
{
    auto resultsIter = std::cbegin(results_);

    // first 2 rows are the title
    showRow(resultsIter);
    ++resultsIter;
    showRow(resultsIter);
    ++resultsIter;
    
    // 3rd row is the underscore lines
    for (const auto &colPair : *resultsIter) {
        // empty string with fill character to make the underscores
        if (!colPair.first.empty()) {
            THROW_FMT_EXCEPTION("Underscore line expected to have empty value.");
        }
        std::cout << std::setfill('-') << std::setw(colPair.second) << colPair.first << std::setfill(' ')
                  << std::setw(COL_SPACE) << "";
    }
    std::cout << "\n";
    ++resultsIter;

    while (resultsIter != std::cend(results_)) {
        showRow(resultsIter);
        ++resultsIter;
    }

    std::cout << std::endl;
}
