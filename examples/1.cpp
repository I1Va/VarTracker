#include <iostream>
#include <vector>

std::vector<int> scanVector(int n) {
    std::vector<int> array(n);
    for (int i = 0; i < n; ++i)
        std::cin >> array[i];

    return array;
}

std::vector<int> bubleSort(std::vector<int> array) {
    for (int i = 0; i < array.size() - 1; ++i) {
        for (int j = 0; j < array.size() - 1 - i; ++j) {
            if (array[j] > array[j + 1]) {
                std::swap(array[j], array[j + 1]);
            }
        }
    }
}

int main() {
    int n;
    std::cin >> n;
    std::vector<int> array = scanVector(n);

    for (int i = 0; i < n; ++i)
        std::cout << array[i] << " ";
    std::cout << "\n";

    return 0;
}
