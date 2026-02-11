#include <iostream>
#include <vector>
#include "tracking.hpp"

typedef Tracked<int> Int;

void compAndSwap(std::vector<Int> arr, Int i, Int j, Int direction) {
    if ((direction == 1 && arr[i] > arr[j]) || (direction == 0 && arr[i] < arr[j])) {
        std::swap(arr[i], arr[j]);
    }
}

// Recursively merge a bitonic sequence into sorted order
void bitonicMerge(std::vector<Int> arr, Int low, Int cnt, Int direction) {
    if (cnt > 1) {
        Int k = cnt / Int(2);
        for (Int i = low; i < low + k; i++) {
            compAndSwap(arr, i, i + k, direction);
        }
        bitonicMerge(arr, low, k, direction);
        bitonicMerge(arr, low + k, k, direction);
    }
}

// Recursively build bitonic sequences and sort them
void bitonicSort(std::vector<Int> arr, Int low, Int cnt, Int direction) {
    if (cnt > 1) {
        Int k = cnt / Int(2);

        // Sort first half ascending
        bitonicSort(arr, low, k, 1);

        // Sort second half descending
        bitonicSort(arr, low + k, k, 0);

        // Merge entire sequence in given direction
        bitonicMerge(arr, low, cnt, direction);
    }
}

// function to sort the entire array
void sortArray(std::vector<Int>& arr) {
    
    // up = 1 → ascending, up = 0 → descending
    Int up = 1; 
    bitonicSort(arr, 0, arr.size(), up);
}


int main() {
    std::vector<Int> arr = {7, 3, 4, 8};

    sortArray(arr);
    GraphBuilder::instance().to_image("graph");
    return 0;
}
