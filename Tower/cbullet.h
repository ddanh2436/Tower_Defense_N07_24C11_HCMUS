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
    sf::Time _timeToLive;   // Bullets used to fly forever once they missed.
    int _targetId;          // Enemy this shot is guided towards, -1 if none.
    float _splashRadius;    // 0 for single target, > 0 for area damage.

public:
    cbullet(const std::string& texturePath, const cpoint& startPosition, const sf::Vector2f& targetDirection,
        float speed, int damage, int targetId = -1, float splashRadius = 0.f);

    void update(sf::Time deltaTime);
    void render(sf::RenderWindow& window);

    // Re-aims the shot at its target's current position. Called by the game
    // each frame while the target is still alive, so shots track a moving
    // enemy instead of sailing past where it used to be.
    void steerTowards(const sf::Vector2f& targetPosition, sf::Time deltaTime);

    bool isActive() const;
    void setActive(bool active);
    sf::Vector2f getPosition() const;
    int getDamage() const;
    int getTargetId() const;
    float getSplashRadius() const;
    sf::FloatRect getGlobalBounds() const;
    bool canCollide() const;
};

#endif // CBULLET_H
