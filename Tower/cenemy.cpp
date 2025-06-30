#include "cenemy.h"
#include "cgame.h"
#include "TextureManager.h"
#include <iostream>
#include <cmath>

cenemy::cenemy(cgame* gameInstance, float speed, int initialHealth, const std::vector<cpoint>& path, float scale, int moneyValue, const sf::Vector2i& frameSize, const std::map<EnemyState, std::map<MovementDirection, std::string>>& texturePaths)
    : _gameInstance(gameInstance),
    _speed(speed),
    _health(initialHealth),
    _maxHealth(initialHealth),
    _isActive(true),
    _path(path),
    _scale(scale),
    _currentFrame(0),
    _elapsedTime(sf::Time::Zero),
    _moneyValue(moneyValue)
{
    _healthBarBackground.setSize({ 40.f, 6.f });
    _healthBarBackground.setFillColor(sf::Color(100, 0, 0, 200));
    _healthBarBackground.setOrigin(20.f, 3.f);
    _healthBarFill.setSize({ 40.f, 6.f });
    _healthBarFill.setFillColor(sf::Color(0, 200, 0, 220));
    _healthBarFill.setOrigin(20.f, 3.f);

    std::vector<float> walkBobEffect = { 0.f, -5.f, -8.f, -5.f, 0.f, 2.f };
    std::vector<float> noBob = {};
    _animations[EnemyState::WALKING][MovementDirection::UP] = { "", {36, 48}, 6, 48, 1.f, walkBobEffect };
    _animations[EnemyState::WALKING][MovementDirection::DOWN] = { "", {36, 48}, 6, 48, 1.f, walkBobEffect };
    _animations[EnemyState::WALKING][MovementDirection::SIDE] = { "", {36, 48}, 6, 48, 1.f, walkBobEffect };
    _animations[EnemyState::DYING][MovementDirection::UP] = { "", {36, 48}, 6, 48, 1.f, noBob };
    _animations[EnemyState::DYING][MovementDirection::DOWN] = { "", {36, 48}, 6, 48, 1.f, noBob };
    _animations[EnemyState::DYING][MovementDirection::SIDE] = { "", {36, 48}, 6, 48, 1.f, noBob };

    for (auto const& [state, dirMap] : texturePaths) {
        for (auto const& [dir, texPath] : dirMap) {
            if (_animations.count(state) && _animations[state].count(dir)) {
                _animations[state][dir].texturePath = texPath;
            }
        }
    }

    if (!_path.empty()) {
        _currentPosition = _path[0];
        _sprite.setPosition(_currentPosition.toVector2f());
        _currentPathIndex = 1;
        _targetPosition = (_currentPathIndex < _path.size()) ? _path[_currentPathIndex] : _path[0];
    }
    else {
        _isActive = false;
        _currentPathIndex = 0;
    }

    _currentState = EnemyState::WALKING;
    sf::Vector2f directionVec = _targetPosition.toVector2f() - _currentPosition.toVector2f();
    _currentDirection = (std::abs(directionVec.y) > std::abs(directionVec.x)) ? ((directionVec.y < 0) ? MovementDirection::UP : MovementDirection::DOWN) : MovementDirection::SIDE;
    setAnimation(_currentState, _currentDirection);
    applyDirectionalFlip(directionVec);
}

void cenemy::setAnimation(EnemyState state, MovementDirection direction) {
    if (_animations.count(state) && _animations[state].count(direction)) {
        Animation& anim = _animations[state][direction];
        if (!anim.texturePath.empty()) {
            _sprite.setTexture(_gameInstance->getTexture(anim.texturePath), true);
        }
        _currentFrameRect = sf::IntRect(0, 0, anim.frameSize.x, anim.frameSize.y);
        _sprite.setTextureRect(_currentFrameRect);
        if (anim.frameCount > 0 && anim.duration > 0) {
            _timePerFrame = sf::seconds(anim.duration / static_cast<float>(anim.frameCount));
        }
        else {
            _timePerFrame = sf::seconds(1.f);
        }
        _sprite.setOrigin(anim.frameSize.x / 2.f, anim.frameSize.y / 2.f);
        _currentFrame = 0;
        _elapsedTime = sf::Time::Zero;
    }
}

void cenemy::update(sf::Time deltaTime) {
    if (_currentState == EnemyState::DEAD) return;

    if (_timePerFrame > sf::Time::Zero) {
        _elapsedTime += deltaTime;
        if (_elapsedTime >= _timePerFrame) {
            _elapsedTime -= _timePerFrame;
            const Animation& currentAnim = _animations.at(_currentState).at(_currentDirection);
            int frameCount = currentAnim.frameCount;

            if (_currentState == EnemyState::DYING) {
                _currentFrame++;
                if (_currentFrame >= frameCount) {
                    _currentFrame = frameCount - 1;
                    _currentState = EnemyState::DEAD;
                    _isActive = false;
                }
            }
            else {
                _currentFrame = (_currentFrame + 1) % frameCount;
            }
            _currentFrameRect.left = _currentFrame * currentAnim.stride;
            _sprite.setTextureRect(_currentFrameRect);
        }
    }

    if (_currentState == EnemyState::WALKING) {
        updateMovement(deltaTime);
    }
}

void cenemy::render(sf::RenderWindow& window) {
    if (isReadyForRemoval()) return;

    sf::Vector2f finalRenderPosition = _currentPosition.toVector2f();
    const Animation& anim = _animations.at(_currentState).at(_currentDirection);

    if (!anim.yOffsets.empty() && _currentFrame < anim.yOffsets.size()) {
        finalRenderPosition.y += anim.yOffsets[_currentFrame];
    }

    _sprite.setPosition(finalRenderPosition);
    window.draw(_sprite);

    if (_currentState != EnemyState::DYING && _currentState != EnemyState::DEAD) {
        const float yOffset = _sprite.getGlobalBounds().height * 0.5f;
        _healthBarBackground.setPosition(finalRenderPosition.x, finalRenderPosition.y - yOffset);
        _healthBarFill.setPosition(finalRenderPosition.x, finalRenderPosition.y - yOffset);
        float healthPercent = static_cast<float>(_health) / _maxHealth;
        _healthBarFill.setSize({ _healthBarBackground.getSize().x * healthPercent, _healthBarBackground.getSize().y });
        window.draw(_healthBarBackground);
        window.draw(_healthBarFill);
    }
}

// Trong tệp cenemy.cpp, thay thế hoàn toàn hàm cũ
void cenemy::updateMovement(sf::Time deltaTime) {
    if (hasReachedEnd()) {
        _isActive = false;
        return;
    }

    // Tổng quãng đường có thể di chuyển trong frame này
    float remainingMoveDistance = _speed * deltaTime.asSeconds() * 2.f;

    // Sử dụng vòng lặp để xử lý việc đi qua waypoint và tiếp tục di chuyển mượt mà
    while (remainingMoveDistance > 0 && !hasReachedEnd()) {
        sf::Vector2f vectorToTarget = _targetPosition.toVector2f() - _currentPosition.toVector2f();
        float distanceToTarget = std::sqrt(vectorToTarget.x * vectorToTarget.x + vectorToTarget.y * vectorToTarget.y);

        // Nếu đã ở rất gần waypoint, tránh lỗi chia cho 0 và lấy waypoint tiếp theo
        if (distanceToTarget < 0.001f) {
            _currentPathIndex++;
            if (!hasReachedEnd()) {
                _targetPosition = _path[_currentPathIndex];
            }
            continue; // Bắt đầu lại vòng lặp với mục tiêu mới
        }

        // Nếu quãng đường còn lại đủ để đi qua hoặc đến waypoint hiện tại
        if (remainingMoveDistance >= distanceToTarget) {
            // Di chuyển kẻ địch đến chính xác waypoint
            _currentPosition = _targetPosition;

            // Giảm quãng đường đã đi
            remainingMoveDistance -= distanceToTarget;

            // Chọn waypoint tiếp theo
            _currentPathIndex++;
            if (!hasReachedEnd()) {
                _targetPosition = _path[_currentPathIndex];

                // Cập nhật hướng animation vì đã đi qua một khúc cua
                sf::Vector2f newDirectionVec = _targetPosition.toVector2f() - _currentPosition.toVector2f();
                MovementDirection newDirection = (std::abs(newDirectionVec.y) > std::abs(newDirectionVec.x))
                    ? ((newDirectionVec.y < 0) ? MovementDirection::UP : MovementDirection::DOWN)
                    : MovementDirection::SIDE;

                if (newDirection != _currentDirection) {
                    _currentDirection = newDirection;
                    setAnimation(_currentState, _currentDirection);
                }
                applyDirectionalFlip(newDirectionVec);
            }
        }
        // Nếu không đủ quãng đường để đến waypoint
        else {
            // Di chuyển về phía waypoint một đoạn bằng quãng đường còn lại
            sf::Vector2f normalizedDir = vectorToTarget / distanceToTarget;
            _currentPosition.x += normalizedDir.x * remainingMoveDistance;
            _currentPosition.y += normalizedDir.y * remainingMoveDistance;

            // Đã dùng hết quãng đường di chuyển của frame này
            remainingMoveDistance = 0;
        }
    }
}

void cenemy::takeDamage(int damage) {
    if (!isAlive()) return;
    _health -= damage;
    if (_health <= 0) {
        _health = 0;
        if (_currentState != EnemyState::DYING) {
            _currentState = EnemyState::DYING;
            if (_animations.count(EnemyState::DYING) == 0 ||
                _animations[EnemyState::DYING].count(_currentDirection) == 0) {
                _currentDirection = MovementDirection::SIDE;
            }
            setAnimation(_currentState, _currentDirection);
        }
    }
}

int cenemy::getMoneyValue() const { return _moneyValue; }

void cenemy::applyDirectionalFlip(const sf::Vector2f& directionVec) {
    float flip = 1.f;
    if (_currentDirection == MovementDirection::SIDE) {
        if (directionVec.x > 0.01f) {
            flip = -1.f;
        }
    }
    _sprite.setScale(flip * _scale, _scale);
}

bool cenemy::isAlive() const { return _health > 0; }
bool cenemy::hasReachedEnd() const { return _currentPathIndex >= _path.size(); }
sf::Vector2f cenemy::getPosition() const { return _currentPosition.toVector2f(); }
sf::FloatRect cenemy::getGlobalBounds() const { return _sprite.getGlobalBounds(); }
bool cenemy::isReadyForRemoval() const { return _currentState == EnemyState::DEAD || hasReachedEnd(); }