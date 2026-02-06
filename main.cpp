#include <iostream>
#include "tracking.hpp"

#include <iostream>
#include <utility>


Tracked<int> make_tracked(int v) {
    TRACK_VAR(int, tmp, v);     
    return tmp;                  
}

void consume(Tracked<int> x) {   
    std::cout << "consume: " << x << "\n";
}

int main() {
    int a = 1;
    int b = 2;

    TRACK_VAR(int, ta, a);
    TRACK_VAR(int, tb, b);


    Tracked<int> m1 = std::move(ta);


    tb = std::move(m1);

  
    Tracked<int> m2 = make_tracked(10);


    consume(std::move(m2));

    std::cout << "tb=" << tb << "\n";
}
