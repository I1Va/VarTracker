#include <iostream>
#include "tracking.hpp"

typedef Tracked<int> Int;

Int no_rvo(Int flag) {
    INIT_FUNC()

    Int a = 1;
    Int b = 2;
    if (flag) {
        return a;
    } else {
        return b;
    }
}

Int yes_rvo(Int flag) {
    INIT_FUNC()

    Int a = 1;
    Int b = 2;

    return a;

    // if (flag) {
    //     return std::move(a);
    // } else {
    //     return std::move(b);
    // }
}

int main() {
    INIT_FUNC()
    TRACK_VAR(int, flag1, 1);
    std::cin >> flag1;
    Int res1 = no_rvo(flag1);
    std::cout << res1;

    TRACK_VAR(int, flag2, 1);
    std::cin >> flag2;
    Int res2 = yes_rvo(flag2);
    std::cout << res2;


    GraphBuilder::instance().to_image("graph", false);
    return 0;
}
