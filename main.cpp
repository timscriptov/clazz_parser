#include <iostream>
#include <string>
#include <filesystem>
#include <sstream>

#include "class_parser.h"

int main(int argc, char *argv[]) {
    const std::string filename = "D:\\AppProjects\\Test\\test\\out\\production\\test\\Main.class";

    if (!std::filesystem::exists(filename)) {
        std::cerr << "Class file not found: " << filename << std::endl;
        return 1;
    }

    try {
        ClassParser parser(filename);
        parser.parse();
        parser.dump();
    } catch (const std::runtime_error &e) {
        std::cerr << "Error execution: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
