#include <iostream>
#include <vector>
#include "tracking.hpp"
#include <string>
#include <typeinfo>
#include <cxxabi.h>
#include <memory>


typedef Tracked<int> Int;

Int f(Int a, Int b) {
    // FUNC_INIT();

    Int c = b * 13 + a;
    c += 4;
    return c;
}


int main() {
    TRACK_VAR(int, result, 0);

    result = f(1, 2); // f(PARAM())
    GraphBuilder::instance().to_image("graph", false);
    
    return 0;
}
