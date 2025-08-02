#pragma once
#ifndef CBULLET_H
#define CBULLET_H

#include "cpoint.h"
#include <SFML/Graphics.hpp>
#include <string>

class cbullet {
private:
    sf::Sprite _sprite;
    bool _textureLoaded;

    cpoint _position;
    sf::Vector2f _velocity; 
    float _speed;
    int _damage;
    bool _isActive;

    sf::Time _gracePeriod;


public:
    cbullet(const std::string& texturePath, const cpoint& startPosition, const sf::Vector2f& targetDirection, float speed, int damage);

    void update(sf::Time deltaTime);
    void render(sf::RenderWindow& window);

    bool isActive() const;
    void setActive(bool active);
    sf::Vector2f getPosition() const;
    int getDamage() const;
    sf::FloatRect getGlobalBounds() const;
    bool canCollide() const;
};

#endif // CBULLET_H
