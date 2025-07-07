#ifndef CPOINT_H
#define CPOINT_H

#include <SFML/System/Vector2.hpp>


class cpoint {
public:
    float x, y; // Sử dụng float cho tọa độ pixel SFML
    int c;      // Thuộc tính phụ, có thể dùng cho loại tile, v.v.

public:
    cpoint() : x(0.f), y(0.f), c(0) {}
    cpoint(float tx, float ty, int tc = 0) : x(tx), y(ty), c(tc) {}
    cpoint(const sf::Vector2f& vec, int tc = 0) : x(vec.x), y(vec.y), c(tc) {}


    // Các getter và setter cơ bản
    float getX() const { return x; }
    float getY() const { return y; }
    int getC() const { return c; }

    void setX(float tx) { x = tx; }
    void setY(float ty) { y = ty; }
    void setC(int tc) { c = tc; }

    sf::Vector2f toVector2f() const {
        return sf::Vector2f(x, y);
    }

    // Các hàm chuyển đổi fromXYToRowCol và fromRowColToXY từ phiên bản console
    // sẽ không còn phù hợp ở đây. Logic chuyển đổi grid <-> pixel
    // sẽ phụ thuộc vào kích thước tile và được xử lý trong cmap hoặc cgame.
};

#endif // CPOINT_H
