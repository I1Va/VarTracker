#include <iostream>
#include <vector>
#include "tracking.hpp"

// Swap using refs
void tracked_swap(Tracked<int> a, Tracked<int> b) {
    TRACK_VAR(int, temp, a);
    a = b;
    b = temp;
}

// Bubble sort (refs)
void bubble_sort(std::vector<Tracked<int>> arr) {
    for (size_t i = 0; i < arr.size(); ++i) {
        for (size_t j = 0; j + 1 < arr.size(); ++j) {
            if (arr[j] > arr[j + 1]) {
                tracked_swap(arr[j], arr[j + 1]);
            }
        }
    }
}

// Binary search using refs
Tracked<int> binary_search_ref(const std::vector<Tracked<int>> arr,
                               const Tracked<int> key) {
    TRACK_VAR(int, l, 0);
    TRACK_VAR(int, r, (int)arr.size() - 1);

    while (l <= r) {
        TRACK_VAR(int, mid, (l + r) / 2);

        if (arr[mid] == key) {
            return mid;
        } else if (arr[mid] < key) {
            l = mid + 1;
        } else {
            r = mid - 1;
        }
    }

    TRACK_VAR(int, not_found, -1);
    return not_found;
}

// Binary search using values (no refs)
Tracked<int> binary_search_val(std::vector<Tracked<int>> arr,
                               Tracked<int> key) {
    TRACK_VAR(int, l, 0);
    TRACK_VAR(int, r, (int)arr.size() - 1);

    while (l <= r) {
        TRACK_VAR(int, mid, (l + r) / 2);

        if (arr[mid] == key) {
            return mid;
        } else if (arr[mid] < key) {
            l = mid + 1;
        } else {
            r = mid - 1;
        }
    }

    TRACK_VAR(int, not_found, -1);
    return not_found;
}

// Function passing Tracked by value
Tracked<int> add_val(Tracked<int> &a, Tracked<int> &b) {
    TRACK_VAR(int, res, a + b);
    return res;
}

// Function passing Tracked by ref
Tracked<int> add_ref(const Tracked<int> a, const Tracked<int> b) {
    TRACK_VAR(int, res, a + b);
    return res;
}

int main() {
    std::vector<int> data = {5, 2, 8, 3, 1, 3, 7, 132, 21, 54};

    std::vector<Tracked<int>> tdata;
    for (size_t i = 0; i < data.size(); ++i) {
        TRACK_VAR(int, x, data[i]);
        tdata.push_back(x); // copy, not move (unoptimized)
    }

    bubble_sort(tdata);

    TRACK_VAR(int, key, 21);

    Tracked<int> idx1 = binary_search_ref(tdata, key);
    Tracked<int> idx2 = binary_search_val(tdata, key);

    TRACK_VAR(int, a, 3);
    TRACK_VAR(int, b, 4);

    Tracked<int> s1 = add_ref(a, b);
    Tracked<int> s2 = add_val(a, b);

    std::string dot = GraphBuilder::instance().to_dot();
    std::cout << dot << "\n";
}
