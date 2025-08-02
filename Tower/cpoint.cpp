#include "cpoint.h"

cpoint::cpoint() : x(0.f), y(0.f), c(0) {}


cpoint::cpoint(float tx, float ty, int tc) : x(tx), y(ty), c(tc) {}

cpoint::cpoint(const sf::Vector2f& vec, int tc) : x(vec.x), y(vec.y), c(tc) {}

cpoint::cpoint(const cpoint& other) {
    x = other.x;
    y = other.y;
    c = other.c;
}