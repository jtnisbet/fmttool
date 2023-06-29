#include "fmt_tool.h"
#include <iomanip>
#include <memory>
#include "fmt_type.h"
#include "fmt_exception.h"
#include "int_type.h"

const std::string FmtTool::DFT_ARGS = "-i 32";

FmtTool::FmtTool() : iSStream_(nullptr), inStream_(nullptr), helpRequested_(false)
{
    // Populate the formatting type map argument options.
    // This is done so that we may do switch during argument parsing of the input args
    cmdArgMap_["-i"] = CmdArg::INT;
    cmdArgMap_["-a"] = CmdArg::ASCII;
    cmdArgMap_["-b"] = CmdArg::BINARY;
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
            case (CmdArg::INT): {
                // A width argument is required. Fetch it from the arg stream, converted to size_t
                if (!(*argStream >> typeWidth)) {
                    THROW_FMT_EXCEPTION("-i argument requires a width argument. (See fmttool -h for help)");
                }
                // base class pointer of derived class type
                newType = std::make_unique<IntType>(typeWidth);
                fmtTypes_.insert(std::move(newType));  // std::set eliminates duplicates
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
    if (fmtTypes_.empty()) {
        // For consistency, this should match the DFT_ARGS variable options
        newType = std::make_unique<IntType>(32);  // base class pointer of derived class type
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

void FmtTool::showFormatRequests()
{
    std::cout << "Requested formatting types:\n";
    for (auto &fmtType : fmtTypes_) {
        std::cout << fmtType->toString() << "\n";
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

void FmtTool::addTitles()
{
    const std::string INPUT_TITLE = "input";
    FmtColList titleRow;
    FmtColList underscoreRow;

    // for each format type request, do a lookup into the titles and widths for a title bar of the table.
    // The title consists of 2 lines, the titles themselves and the underscore characters as the second line.
    // Getting these titles produces a vector of pairs (data paired with display width).

    // First column, the user input.  We always show this as the first column
    titleRow.emplace_back(INPUT_TITLE, INPUT_TITLE.size());
    underscoreRow.emplace_back("", INPUT_TITLE.size());

    // Then, add the rest of the columns.  Each FmtType might add more than one
    for (const auto &fmtType : fmtTypes_) {
        fmtType->getTitleRow(titleRow, underscoreRow);
    }
    results_.push_back(titleRow);  // adds the title row
    results_.push_back(underscoreRow);  // adds the underscore row
}

void FmtTool::executeFormatting()
{
    // We have a list of values coming from our chosen input stream (it may be a istringtream or it might be std::cin).
    // For each value, execute the requested formatting against that value.
    bool moreData = true;
    std::string currValue;
    addTitles();
    while (moreData) {
        if (*inStream_ >> currValue) {
            addToResultTable(currValue);  // formats the value into the result table
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

void FmtTool::displayResultTable()
{
    const int COL_SPACE = 2;  // Provide 2 whitespaces in between each column
    auto resultsIter = std::cbegin(results_);

    // first row is the title
    for (const auto &colPair : *resultsIter) {
        std::cout << std::setw(colPair.second) << colPair.first << std::setfill(' ') << std::setw(COL_SPACE) << "";
    }
    std::cout << "\n";
    ++resultsIter;
    
    // second row is the underscore line
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
        for (const auto &colPair : *resultsIter) {
            std::cout << std::setw(colPair.second) << colPair.first << std::setfill(' ') << std::setw(COL_SPACE) << "";
        }
        std::cout << "\n";
        ++resultsIter;
    }

    std::cout << std::endl;
}
