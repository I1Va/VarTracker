#include <iostream>
#include <vector>

#include "operators.hpp"

TrackedInt binary_search(const std::vector<TrackedInt>& a, TrackedInt varX) {
    TrackedInt l = 0, r = (TrackedInt)a.size() - 1;
    while (l <= r) {
        TrackedInt m = l + (r - l) / 2;
        if (a[m] == varX) return m;
        if (a[m] < varX) l = m + 1;
        else r = m - 1;
    }
    return -1; // not found
}

int main() {
    TrackedInt varN;
    std::cin >> varN;

    std::vector<TrackedInt> a(varN);
    for (TrackedInt i = 0; i < varN; ++i)
        std::cin >> a[i];

    TrackedInt varX;
    std::cin >> varX;

    std::cout << binary_search(a, varX) << "\varN";
    return 0;
}
