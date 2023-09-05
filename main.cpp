#include <cctype>
#include <iostream>
#include <memory>
#include <string>
#include "fmt_tool.h"
#include "fmt_exception.h"

bool IsAnySpace(const std::string value)
{
    for (int i = 0; i< value.size(); ++i) {
        if (std::isspace(value[i])) {
            return true;
        }
    }
    return false;
}

int main(int argc, char **argv)
{
    std::stringstream args;
    auto fmtTool = std::make_unique<FmtTool>();    
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            std::string tok = argv[i];
            // When a quoted string is inputted from the shell, the quotes are gone but argv[i]
            // can contain white space characters.
            if (IsAnySpace(tok)) {
                // There was a space in this argument.
                // wrap this string into an enclosed delimiter. Later when this is parsed properly, it will remain
                // together as a compound string.  We'll use STX and ETX non-printable characters for this so that
                // that the user wont have those inside their strings naturally
                std::string newTok("\2");
                newTok += tok + "\3";
                args << " " << newTok;
            } else {
                args << " " << tok;
            }
        }
    } else {
        // Make a default set of formatting rules if the user did not give any args
        args << FmtTool::DFT_ARGS;
    }

    try {
        fmtTool->parseArgs(&args);
        if (fmtTool->showHelp()) {
            return 0;
        }
        fmtTool->executeFormatting();
        fmtTool->displayResultTable();
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
