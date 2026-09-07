#include "cbullet.h"
#include "TextureManager.h"
#include <iostream>
#include <cmath>

namespace {
    // A shot that has not connected within this long has clearly missed.
    const sf::Time BULLET_LIFETIME = sf::seconds(3.f);
    // How fast a guided shot may swing towards its target, in degrees/second.
    const float BULLET_TURN_RATE = 540.f;

    float lengthOf(const sf::Vector2f& v) {
        return std::sqrt(v.x * v.x + v.y * v.y);
    }
}

cbullet::cbullet(const std::string& texturePath, const cpoint& startPosition, const sf::Vector2f& targetDirection,
    float speed, int damage, int targetId, float splashRadius)
    : _position(startPosition), _speed(speed), _damage(damage), _isActive(true),
    _gracePeriod(sf::seconds(0.05f)), _timeToLive(BULLET_LIFETIME),
    _targetId(targetId), _splashRadius(splashRadius), _textureLoaded(true) {

    _sprite.setTexture(TextureManager::getTexture(texturePath));

    sf::FloatRect bounds = _sprite.getLocalBounds();
    _sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    _sprite.setPosition(_position.toVector2f());

    float length = lengthOf(targetDirection);
    if (length != 0.f) {
        _velocity = (targetDirection / length) * _speed;
    }
    else {
        _velocity = sf::Vector2f(0.f, -_speed);
    }
}

void cbullet::steerTowards(const sf::Vector2f& targetPosition, sf::Time deltaTime) {
    if (!_isActive) return;

    sf::Vector2f desired = targetPosition - _position.toVector2f();
    float desiredLength = lengthOf(desired);
    if (desiredLength < 0.001f) return;

    const float currentAngle = std::atan2(_velocity.y, _velocity.x);
    const float desiredAngle = std::atan2(desired.y, desired.x);

    // Shortest signed turn, wrapped into [-pi, pi].
    const float pi = 3.14159265358979323846f;
    float delta = desiredAngle - currentAngle;
    while (delta > pi) delta -= 2.f * pi;
    while (delta < -pi) delta += 2.f * pi;

    const float maxTurn = (BULLET_TURN_RATE * pi / 180.f) * deltaTime.asSeconds();
    if (delta > maxTurn) delta = maxTurn;
    else if (delta < -maxTurn) delta = -maxTurn;

    const float newAngle = currentAngle + delta;
    _velocity = sf::Vector2f(std::cos(newAngle) * _speed, std::sin(newAngle) * _speed);
}

void cbullet::update(sf::Time deltaTime) {
    if (!_isActive) return;

    if (_gracePeriod > sf::Time::Zero) {
        _gracePeriod -= deltaTime;
    }

    _timeToLive -= deltaTime;
    if (_timeToLive <= sf::Time::Zero) {
        _isActive = false;
        return;
    }

    _position.x += _velocity.x * deltaTime.asSeconds();
    _position.y += _velocity.y * deltaTime.asSeconds();
    _sprite.setPosition(_position.toVector2f());

    // Point the sprite along its flight path so arrows and cannonballs read
    // correctly instead of always facing the same way.
    const float pi = 3.14159265358979323846f;
    _sprite.setRotation(std::atan2(_velocity.y, _velocity.x) * 180.f / pi);
}

void cbullet::render(sf::RenderWindow& window) {
    if (!_isActive) return;

    if (_textureLoaded) {
        window.draw(_sprite);
    }
    else {
        sf::CircleShape placeholder(5.f);
        placeholder.setFillColor(sf::Color::Yellow);
        placeholder.setOrigin(5.f, 5.f);
        placeholder.setPosition(_position.toVector2f());
        window.draw(placeholder);
    }
}

bool cbullet::isActive() const {
    return _isActive;
}

void cbullet::setActive(bool active) {
    _isActive = active;
}

sf::Vector2f cbullet::getPosition() const {
    return _position.toVector2f();
}

int cbullet::getDamage() const {
    return _damage;
}

int cbullet::getTargetId() const {
    return _targetId;
}

float cbullet::getSplashRadius() const {
    return _splashRadius;
}

sf::FloatRect cbullet::getGlobalBounds() const {
    return _sprite.getGlobalBounds();
}

bool cbullet::canCollide() const {
    return _isActive && _gracePeriod <= sf::Time::Zero;
}
