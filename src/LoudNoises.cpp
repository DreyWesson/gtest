#include "../inc/LoudNoises.hpp"
#include <iostream>
#include <cctype>

std::string makeLoudNoises(int argc, char **argv) {
    std::string result;

    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            int j = 0;
            while (argv[i][j] != '\0') {
                if (std::islower(argv[i][j])) {
                    result += std::toupper(argv[i][j]);
                } else {
                    result += argv[i][j];
                }
                ++j;
            }
        }
    } else {
        result = "* LOUD AND UNBEARABLE FEEDBACK NOISE *";
    }

    return result;
}
