#include "cbullet.h"
#include "TextureManager.h"
#include <iostream>
#include <cmath>

cbullet::cbullet(const std::string& texturePath, const cpoint& startPosition, const sf::Vector2f& targetDirection, float speed, int damage)
    : _position(startPosition), _speed(speed), _damage(damage), _isActive(true), _gracePeriod(sf::seconds(0.05f)), _textureLoaded(true) {

    _sprite.setTexture(TextureManager::getTexture(texturePath));

    // Thiết lập gốc (origin) cho sprite ở tâm
    sf::FloatRect bounds = _sprite.getLocalBounds();
    _sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    // --- KẾT THÚC THAY ĐỔI ---

    _sprite.setPosition(_position.toVector2f());

    float length = std::sqrt(targetDirection.x * targetDirection.x + targetDirection.y * targetDirection.y);
    if (length != 0) {
        _velocity = (targetDirection / length) * _speed;
    }
    else {
        _velocity = sf::Vector2f(0, -_speed);
    }
}


void cbullet::update(sf::Time deltaTime) {
    if (!_isActive) return;

    if (_gracePeriod > sf::Time::Zero) {
        _gracePeriod -= deltaTime;
    }

    _position.x += _velocity.x * deltaTime.asSeconds();
    _position.y += _velocity.y * deltaTime.asSeconds();
    _sprite.setPosition(_position.toVector2f());
}

void cbullet::render(sf::RenderWindow& window) {
    if (!_isActive) return;

    if (_textureLoaded) {
        window.draw(_sprite);
    }
    else {
        sf::CircleShape placeholder(5.f); // Kích thước đạn
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

sf::FloatRect cbullet::getGlobalBounds() const {
    return _sprite.getGlobalBounds();
}

bool cbullet::canCollide() const {
    return _isActive && _gracePeriod <= sf::Time::Zero;
}