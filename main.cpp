#include <iostream>
#include <string>
#include "fmt_tool.h"

int main(int argc, char **argv)
{
    std::stringstream args;
    auto fmtTool = std::make_unique<FmtTool>();    
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            args << " " << std::string(argv[i]);
        }
    }

    fmtTool->parseArgs(&args);
    
    return 0;
}
