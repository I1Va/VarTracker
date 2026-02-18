#include <iostream>
#include <string>
#include <cstring>

struct String {
    char* data_;
    size_t length_;

    ~String() { if (data_) delete[] data_; }
    
    String(char *data) {
        data_ = data;
        length_ = std::strlen(data);
    }

    String(String&& other) noexcept
        : data_(other.data_), length_(other.length_) {}

    char& operator[](size_t i) {
        return data_[i];          
    }
};

int main() {
    String a("12e12");
    String b = std::move(a);   
}
