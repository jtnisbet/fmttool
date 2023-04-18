#include <iostream>
#include <memory>
#include <string>
#include "fmt_tool.h"
#include "fmt_exception.h"

int main(int argc, char **argv)
{
    std::stringstream args;
    auto fmtTool = std::make_unique<FmtTool>();    
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            args << " " << std::string(argv[i]);
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
        fmtTool->showFormatRequests();
        fmtTool->executeFormatting();
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
