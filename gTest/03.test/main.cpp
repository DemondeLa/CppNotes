#include "add.h"
#include <iostream>

int  main(int argc, char **argv) {
    std::cout << "1 + 2 = " << add(1, 2) << std::endl;
    std::cout << "3 + 4 = " << add(3, 4) << std::endl;
    std::cout << "-1 + 1 = " << add(-1, 1) << std::endl;
    std::cout << "-1 + (-1) = " << add(-1, -1) << std::endl;
    return 0;
}
