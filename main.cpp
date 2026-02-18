#include <iostream>
#include <vector>
#include "tracking.hpp"

// Function taking by value (copy)
Tracked<int> copy_func(Tracked<int> x) {
    INIT_FUNC()
    TRACK_VAR(int, local_copy, x);   // copy ctor
    local_copy += 10;                // compound assignment
    return local_copy;               // move ctor when returning
}

// Function taking by rvalue reference (move)
Tracked<int> move_func(Tracked<int>&& x) {
    INIT_FUNC()
    TRACK_VAR(int, local_move, std::move(x)); // move ctor
    local_move *= 2;
    return local_move;                        // move ctor when returning
}

// Function taking by lvalue reference
void ref_func(Tracked<int>& x) {
    INIT_FUNC()
    TRACK_VAR(int, local_ref, x); // copy ctor
    local_ref -= 5;
}

// Function taking by const reference
void const_ref_func(const Tracked<int>& x) {
    INIT_FUNC()
    TRACK_VAR(int, local_const_ref, x); // copy ctor
}

int main() {
    INIT_FUNC()
    int a = 1, b = 2, c = 3;

    TRACK_VAR(int, ta, a);
    TRACK_VAR(int, tb, b);
    TRACK_VAR(int, tc, c);

    // --- Copy and move examples ---
    Tracked<int> t1 = copy_func(ta);        // copy_func copies ta
    Tracked<int> t2 = move_func(std::move(tb)); // move_func moves tb

    // --- Reference functions ---
    ref_func(tc);               // copies tc inside function
    const_ref_func(ta);         // copies ta inside function

    // --- Arithmetic operations ---
    TRACK_VAR(int, td, t1 + t2);  // Tracked + Tracked
    TRACK_VAR(int, te, td + 100); // Tracked + literal
    td += te;                      // compound assignment
    te = td - 50;                  // compound assignment via operator=

    // --- Mix assignments ---
    t1 = t2;     // copy assign
    t2 = std::move(te); // move assign

    // --- Simple loop with tracked variables ---
    TRACK_VAR(int, sum, 0);
    for (int i = 0; i < 10; i++) {
        sum += i;  // compound assignment logged
    }

    // --- More arithmetic chains ---
    TRACK_VAR(int, result, (t1 + t2) * 2 - t1);

    // --- Output graph ---
    GraphBuilder::instance().to_image("graph", false);
}



 