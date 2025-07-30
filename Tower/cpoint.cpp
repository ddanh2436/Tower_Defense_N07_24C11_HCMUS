#include "cpoint.h"

// Hàm tạo mặc định
cpoint::cpoint() : x(0.f), y(0.f), c(0) {}

// Hàm tạo với tham số
cpoint::cpoint(float tx, float ty, int tc) : x(tx), y(ty), c(tc) {}

// Hàm tạo từ sf::Vector2f
cpoint::cpoint(const sf::Vector2f& vec, int tc) : x(vec.x), y(vec.y), c(tc) {}

// *** QUAN TRỌNG: Định nghĩa hàm tạo sao chép ***
// Hàm này đảm bảo rằng khi một cpoint được sao chép,
// các giá trị x, y, và c của nó được gán một cách chính xác.
cpoint::cpoint(const cpoint& other) {
    x = other.x;
    y = other.y;
    c = other.c;
}