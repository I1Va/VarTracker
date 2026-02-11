#include <iostream>
#include <vector>
#include "tracking.hpp"

void swap_val(Tracked<int> a, Tracked<int> b) {
    TRACK_VAR(int, tmp, a);
    a = b;
    b = tmp;
}

std::vector<Tracked<int>> bubble_sort(std::vector<Tracked<int>> arr) {
    for (size_t i = 0; i < arr.size(); ++i) {
        for (size_t j = 0; j + 1 < arr.size(); ++j) {
            if (arr[j] > arr[j + 1]) {
                swap_val(arr[j], arr[j + 1]);
            }
        }
    }
    return arr;
}

Tracked<int> linear_search(std::vector<Tracked<int>> arr, Tracked<int> key) {
    for (size_t i = 0; i < arr.size(); ++i) {
        if (arr[i] == key) {
            TRACK_VAR(int, idx, (int)i);
            return idx;
        }
    }
    TRACK_VAR(int, nf, -1);
    return nf;
}

Tracked<int> sum_all(std::vector<Tracked<int>> arr) {
    TRACK_VAR(int, acc, 0);
    for (size_t i = 0; i < arr.size(); ++i) {
        acc = acc + arr[i];
    }
    return acc;
}

std::vector<Tracked<int>> duplicate(std::vector<Tracked<int>> arr) {
    std::vector<Tracked<int>> out;
    for (size_t i = 0; i < arr.size(); ++i) {
        out.push_back(arr[i]);
    }
    return out;
}

int main() {
    std::vector<int> raw = {7, 2, 9, 1, 5};

    std::vector<Tracked<int>> data;
    for (size_t i = 0; i < raw.size(); ++i) {
        TRACK_VAR(int, x, raw[i]);
        data.push_back(x);
    }

    std::vector<Tracked<int>> copy1 = duplicate(data);
    std::vector<Tracked<int>> sorted = bubble_sort(copy1);

    TRACK_VAR(int, key, 5);
    Tracked<int> pos = linear_search(sorted, key);

    Tracked<int> total = sum_all(sorted);

    GraphBuilder::instance().to_image("graph.png", false);
}
