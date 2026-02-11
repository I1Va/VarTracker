#include <iostream>
#include "tracking.hpp"

Tracked<int> add(Tracked<int> a, Tracked<int> b) {
    TRACK_VAR(int, add_r, a + b);
    return add_r;
}

Tracked<int> mul(Tracked<int> a, Tracked<int> b) {
    TRACK_VAR(int, i, 0);
    TRACK_VAR(int, res, 0);
    while (i < b) {
        res = add(res, a);
        i = i + 1;
    }
    return res;
}

Tracked<int> expr(Tracked<int> a, Tracked<int> b, Tracked<int> c) {
    TRACK_VAR(int, s, add(a, b));
    TRACK_VAR(int, p, mul(s, c));
    TRACK_VAR(int, r, add(p, a));
    return r;
}

Tracked<int> pipeline(Tracked<int> x) {
    TRACK_VAR(int, a, expr(x, 2, 3));
    TRACK_VAR(int, b, expr(a, x, 2));
    TRACK_VAR(int, c, add(a, b));
    return c;
}

int main() {
    TRACK_VAR(int, x, 3);
    TRACK_VAR(int, y, 4);
    //TRACK_VAR(int, z, 2);

 
    TRACK_VAR(int, r1, mul(x, y));

    // TRACK_VAR(int, r1, expr(x, y, z));


    // TRACK_VAR(int, r2, pipeline(r1));
    // TRACK_VAR(int, r3, add(r1, r2));

    GraphBuilder::instance().to_image("graph.png", false);
    return 0;
}
