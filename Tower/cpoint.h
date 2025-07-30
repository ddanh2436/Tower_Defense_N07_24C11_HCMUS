#ifndef CPOINT_H
#define CPOINT_H

#include <SFML/System/Vector2.hpp>

class cpoint {
public:
    float x, y;
    int c;

public:
    cpoint();
    cpoint(float tx, float ty, int tc = 0);
    cpoint(const sf::Vector2f& vec, int tc = 0);
    cpoint(const cpoint& other); // <-- HÀM TẠO SAO CHÉP

    float getX() const { return x; }
    float getY() const { return y; }
    int getC() const { return c; }

    void setX(float tx) { x = tx; }
    void setY(float ty) { y = ty; }
    void setC(int tc) { c = tc; }

    sf::Vector2f toVector2f() const {
        return sf::Vector2f(x, y);
    }
};

#endif // CPOINT_H