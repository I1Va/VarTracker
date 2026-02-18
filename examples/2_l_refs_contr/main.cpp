#include <iostream>
#include "tracking.hpp"

typedef Tracked<int> Int;

Int add(Int &a, Int &b) {
    INIT_FUNC()
    TRACK_VAR(int, add_r, a);
    add_r = add_r + b;

    return add_r;
}

const Int& mul(Int a, Int b) {
    INIT_FUNC()

    TRACK_VAR(int, i, 0);
    TRACK_VAR(int, res, 0);
    while (i < b) {
        res = add(res, a);
        i = i + 1;
    }

    TRACK_VAR(int, ret, res);
    return ret;
}

int main() {
    TRACK_VAR(int, x, 1);
    TRACK_VAR(int, y, 1);
    Int res1 = add(x, y);

    TRACK_VAR(int, z, 3);
    Int res2 = mul(res1, z);

    std::cout << res2 << "\n";

    GraphBuilder::instance().to_image("graph", false);
    return 0;
}
