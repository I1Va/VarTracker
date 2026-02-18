#include <iostream>
#include "tracking.hpp"

typedef Tracked<int> Int;

Int add(Int &a, Int &b) {
    INIT_FUNC()
    TRACK_VAR(int, add_r, a);
    add_r = add_r + b;

    return add_r;
}

Int mul(Int a, Int b) {
    INIT_FUNC()

    TRACK_VAR(int, i, 0);
    TRACK_VAR(int, res, 0);
    while (i < b) {
        res = add(res, a);
        i = i + 1;
    }
    return res;
}

Int RVO(Int a) {
    TRACK_VAR(int, ret, 1 + a);
    return ret;
}

int main() {
    TRACK_VAR(int, x, 1);

    std::cout << RVO(x);
    // TRACK_VAR(int, y, 1);
    // Int res1 = add(x, y);
    // std::cout << res1 << "\n";

    GraphBuilder::instance().to_image("2", false);
    return 0;
}
