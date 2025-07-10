#include "cenemy.h"
#include "cgame.h"
#include <iostream>
#include <cmath>

const float HEALTH_BAR_Y_OFFSET = 4.f;

// SỬA ĐỔI: Thêm typeIndex vào hàm khởi tạo
cenemy::cenemy(cgame* gameInstance, const EnemyType& type, int typeIndex, const std::vector<cpoint>& path)
    : _gameInstance(gameInstance),
    _path(path),
    _typeIndex(typeIndex), // THÊM MỚI: Khởi tạo biến typeIndex
    _speed(type.speed),
    _health(type.health),
    _maxHealth(type.health),
    _moneyValue(type.moneyValue),
    _isActive(true),
    _currentFrame(0),
    _elapsedTime(sf::Time::Zero),
    _currentPathIndex(0)
{

    _healthBarBackground.setSize({ 32.f, 5.f });
    _healthBarBackground.setFillColor(sf::Color(50, 50, 50, 210));
    _healthBarBackground.setOrigin(_healthBarBackground.getSize().x / 2.f, _healthBarBackground.getSize().y / 2.f);
    _healthBarFill.setSize({ 32.f, 5.f });
    _healthBarFill.setFillColor(sf::Color::Green);
    _healthBarFill.setOrigin(0, _healthBarFill.getSize().y / 2.f);

    _sprite.setScale(type.scale, type.scale);

    std::vector<float> walkBobEffect = { 0.f, -5.f, -8.f, -5.f, 0.f, 2.f };
    std::vector<float> noBob = {};

    _animations[EnemyState::WALKING][MovementDirection::UP] = { "", type.frameSize, type.frameCount, type.stride, 1.f, walkBobEffect };
    _animations[EnemyState::WALKING][MovementDirection::DOWN] = { "", type.frameSize, type.frameCount, type.stride, 1.f, walkBobEffect };
    _animations[EnemyState::WALKING][MovementDirection::SIDE] = { "", type.frameSize, type.frameCount, type.stride, 1.f, walkBobEffect };
    _animations[EnemyState::DYING][MovementDirection::UP] = { "", type.frameSize, type.frameCount, type.stride, 1.f, noBob };
    _animations[EnemyState::DYING][MovementDirection::DOWN] = { "", type.frameSize, type.frameCount, type.stride, 1.f, noBob };
    _animations[EnemyState::DYING][MovementDirection::SIDE] = { "", type.frameSize, type.frameCount, type.stride, 1.f, noBob };

    for (auto const& [state, dirMap] : type.texturePaths) {
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
    }

    _currentState = EnemyState::WALKING;
    sf::Vector2f directionVec = _targetPosition.toVector2f() - _currentPosition.toVector2f();
    _currentDirection = (std::abs(directionVec.y) > std::abs(directionVec.x)) ? ((directionVec.y < 0) ? MovementDirection::UP : MovementDirection::DOWN) : MovementDirection::SIDE;
    setAnimation(_currentState, _currentDirection);
    applyDirectionalFlip(directionVec);
}

// ... (Các hàm update, render, updateMovement, v.v. của bạn giữ nguyên) ...
// ... (Tôi sẽ dán lại đầy đủ để bạn không bị sót) ...

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
    auto it_state = _animations.find(_currentState);
    if (it_state == _animations.end()) {
        std::cerr << "Error: Animation state " << static_cast<int>(_currentState) << " not found!" << std::endl;
        return;
    }
    auto it_dir = it_state->second.find(_currentDirection);
    if (it_dir == it_state->second.end()) {
        std::cerr << "Error: Animation direction for state " << static_cast<int>(_currentState) << " not found!" << std::endl;
        return;
    }
    const Animation& currentAnim = it_dir->second;
    if (_timePerFrame > sf::Time::Zero) {
        _elapsedTime += deltaTime;
        if (_elapsedTime >= _timePerFrame) {
            _elapsedTime -= _timePerFrame;
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
    sf::Vector2f finalRenderPosition = _currentPosition.toVector2f();
    if (!currentAnim.yOffsets.empty() && _currentFrame < currentAnim.yOffsets.size()) {
        finalRenderPosition.y += currentAnim.yOffsets[_currentFrame];
    }
    _sprite.setPosition(finalRenderPosition);
}

void cenemy::render(sf::RenderWindow& window) {
    if (isReadyForRemoval()) return;
    window.draw(_sprite);
    if (isAlive()) {
        auto it_state = _animations.find(_currentState);
        if (it_state == _animations.end()) return;
        auto it_dir = it_state->second.find(_currentDirection);
        if (it_dir == it_state->second.end()) return;
        const Animation& currentAnim = it_dir->second;
        sf::Vector2f basePosition = _currentPosition.toVector2f();
        float spriteHalfHeight = (currentAnim.frameSize.y * _sprite.getScale().y) / 2.f;
        float healthBarX = basePosition.x;
        float healthBarY = basePosition.y - spriteHalfHeight - HEALTH_BAR_Y_OFFSET;
        _healthBarBackground.setPosition(healthBarX, healthBarY);
        _healthBarFill.setPosition(healthBarX - _healthBarBackground.getSize().x / 2.f, healthBarY);
        window.draw(_healthBarBackground);
        window.draw(_healthBarFill);
    }
}

void cenemy::updateMovement(sf::Time deltaTime) {
    if (hasReachedEnd()) {
        _isActive = false;
        return;
    }
    float remainingMoveDistance = _speed * deltaTime.asSeconds() * 2.f;
    while (remainingMoveDistance > 0 && !hasReachedEnd()) {
        sf::Vector2f vectorToTarget = _targetPosition.toVector2f() - _currentPosition.toVector2f();
        float distanceToTarget = std::sqrt(vectorToTarget.x * vectorToTarget.x + vectorToTarget.y * vectorToTarget.y);
        if (distanceToTarget < 0.001f) {
            _currentPathIndex++;
            if (!hasReachedEnd()) {
                _targetPosition = _path[_currentPathIndex];
            }
            continue;
        }
        if (remainingMoveDistance >= distanceToTarget) {
            _currentPosition = _targetPosition;
            remainingMoveDistance -= distanceToTarget;
            _currentPathIndex++;
            if (!hasReachedEnd()) {
                _targetPosition = _path[_currentPathIndex];
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
        else {
            sf::Vector2f normalizedDir = vectorToTarget / distanceToTarget;
            _currentPosition.x += normalizedDir.x * remainingMoveDistance;
            _currentPosition.y += normalizedDir.y * remainingMoveDistance;
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
            if (_animations.count(EnemyState::DYING) == 0 || _animations[EnemyState::DYING].count(_currentDirection) == 0) {
                _currentDirection = MovementDirection::SIDE;
            }
            setAnimation(_currentState, _currentDirection);
        }
    }
    float healthPercent = static_cast<float>(_health) / _maxHealth;
    _healthBarFill.setSize({ _healthBarBackground.getSize().x * healthPercent, _healthBarBackground.getSize().y });
    if (healthPercent > 0.6f) {
        _healthBarFill.setFillColor(sf::Color::Green);
    }
    else if (healthPercent > 0.3f) {
        _healthBarFill.setFillColor(sf::Color::Yellow);
    }
    else {
        _healthBarFill.setFillColor(sf::Color::Red);
    }
}

int cenemy::getMoneyValue() const { return _moneyValue; }

void cenemy::applyDirectionalFlip(const sf::Vector2f& directionVec) {
    float scaleY = _sprite.getScale().y;
    float scaleX = std::abs(_sprite.getScale().x);
    if (_currentDirection == MovementDirection::SIDE) {
        if (directionVec.x < 0) {
            _sprite.setScale(scaleX, scaleY);
        }
        else {
            _sprite.setScale(-scaleX, scaleY);
        }
    }
    else {
        _sprite.setScale(scaleX, scaleY);
    }
}

bool cenemy::isAlive() const { return _health > 0; }
bool cenemy::hasReachedEnd() const { return _currentPathIndex >= _path.size(); }
sf::Vector2f cenemy::getPosition() const { return _currentPosition.toVector2f(); }
sf::FloatRect cenemy::getGlobalBounds() const { return _sprite.getGlobalBounds(); }
bool cenemy::isReadyForRemoval() const { return _currentState == EnemyState::DEAD || hasReachedEnd(); }

// --- TRIỂN KHAI CÁC HÀM MỚI ĐỂ HỖ TRỢ LƯU/TẢI GAME ---

int cenemy::getTypeIndex() const {
    return _typeIndex;
}

float cenemy::getHealth() const {
    return _health;
}

int cenemy::getPathIndex() const {
    return _currentPathIndex;
}

void cenemy::setHealth(float newHealth) {
    _health = newHealth;
    // Cập nhật lại thanh máu một cách trực quan
    if (_maxHealth > 0) {
        float healthPercent = (_health < 0) ? 0 : (_health / _maxHealth);
        _healthBarFill.setSize({ _healthBarBackground.getSize().x * healthPercent, _healthBarBackground.getSize().y });

        if (healthPercent > 0.6f) _healthBarFill.setFillColor(sf::Color::Green);
        else if (healthPercent > 0.3f) _healthBarFill.setFillColor(sf::Color::Yellow);
        else _healthBarFill.setFillColor(sf::Color::Red);
    }
}

void cenemy::setPosition(const cpoint& pos) {
    _currentPosition = pos;
    // Vị trí thực của sprite sẽ được cập nhật trong hàm update() chính,
    // nên chúng ta chỉ cần thiết lập lại vị trí logic ở đây.
}

void cenemy::setPathIndex(int newPathIndex) {
    if (newPathIndex >= 0 && static_cast<size_t>(newPathIndex) <= _path.size()) {
        _currentPathIndex = newPathIndex;
        if (!hasReachedEnd()) {
            _targetPosition = _path[_currentPathIndex];
        }
    }
}