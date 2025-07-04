#include "cenemy.h"
#include "cgame.h"
#include <iostream>
#include <cmath>

const float HEALTH_BAR_Y_OFFSET = 4.f;

cenemy::cenemy(cgame* gameInstance, const EnemyType& type, const std::vector<cpoint>& path)
    : _gameInstance(gameInstance),
    _path(path),
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
                    _currentFrame = frameCount - 1; // Giữ ở frame cuối
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

    // --- Cập nhật di chuyển ---
    if (_currentState == EnemyState::WALKING) {
        updateMovement(deltaTime);
    }

    // --- Cập nhật vị trí cuối cùng của sprite để render ---
    sf::Vector2f finalRenderPosition = _currentPosition.toVector2f();
    // Sử dụng lại `currentAnim` đã được lấy một cách an toàn ở trên
    if (!currentAnim.yOffsets.empty() && _currentFrame < currentAnim.yOffsets.size()) {
        finalRenderPosition.y += currentAnim.yOffsets[_currentFrame];
    }
    _sprite.setPosition(finalRenderPosition);
}

void cenemy::render(sf::RenderWindow& window) {
    if (isReadyForRemoval()) return;

    // Vẽ sprite của quái tại vị trí có hiệu ứng nhún nhảy
    window.draw(_sprite);

    // Vẽ thanh máu
    if (isAlive()) {
        // Lấy thông tin animation hiện tại một cách an toàn
        auto it_state = _animations.find(_currentState);
        if (it_state == _animations.end()) return; // Không tìm thấy state, không vẽ
        auto it_dir = it_state->second.find(_currentDirection);
        if (it_dir == it_state->second.end()) return; // Không tìm thấy direction, không vẽ
        const Animation& currentAnim = it_dir->second;

        // Lấy vị trí logic (không có hiệu ứng nhún nhảy) làm gốc
        sf::Vector2f basePosition = _currentPosition.toVector2f();

        // Tính toán đỉnh đầu của quái dựa trên vị trí gốc và chiều cao của frame
        // (Origin của sprite đang được đặt ở giữa)
        float spriteHalfHeight = (currentAnim.frameSize.y * _sprite.getScale().y) / 2.f;

        // Vị trí thanh máu = Vị trí gốc - Nửa chiều cao sprite - Khoảng cách offset
        float healthBarX = basePosition.x;
        float healthBarY = basePosition.y - spriteHalfHeight - HEALTH_BAR_Y_OFFSET;

        // Cập nhật vị trí cho cả hai thanh
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

    // --- THÊM MỚI: Cập nhật kích thước và màu sắc thanh máu ---
    // Code này bị thiếu trong phiên bản của bạn, đây là phần quan trọng nhất!
    float healthPercent = static_cast<float>(_health) / _maxHealth;
    _healthBarFill.setSize({ _healthBarBackground.getSize().x * healthPercent, _healthBarBackground.getSize().y });

    // Đổi màu theo % máu
    if (healthPercent > 0.6f) {
        _healthBarFill.setFillColor(sf::Color::Green);
    }
    else if (healthPercent > 0.3f) {
        _healthBarFill.setFillColor(sf::Color::Yellow);
    }
    else {
        _healthBarFill.setFillColor(sf::Color::Red);
    }
    // --- KẾT THÚC THÊM MỚI ---
}

int cenemy::getMoneyValue() const { return _moneyValue; }

void cenemy::applyDirectionalFlip(const sf::Vector2f& directionVec) {
    // Giữ nguyên scale Y, chỉ thay đổi scale X để lật ảnh
    float scaleY = _sprite.getScale().y;
    float scaleX = std::abs(_sprite.getScale().x); // Lấy scale dương để tính toán

    if (_currentDirection == MovementDirection::SIDE) {
        if (directionVec.x < 0) { // Đi sang trái
            _sprite.setScale(scaleX, scaleY);
        }
        else { // Đi sang phải
            _sprite.setScale(-scaleX, scaleY);
        }
    }
    else {
        // Khi đi lên/xuống, không lật ảnh
        _sprite.setScale(scaleX, scaleY);
    }
}

bool cenemy::isAlive() const { return _health > 0; }
bool cenemy::hasReachedEnd() const { return _currentPathIndex >= _path.size(); }
sf::Vector2f cenemy::getPosition() const { return _currentPosition.toVector2f(); }
sf::FloatRect cenemy::getGlobalBounds() const { return _sprite.getGlobalBounds(); }
bool cenemy::isReadyForRemoval() const { return _currentState == EnemyState::DEAD || hasReachedEnd(); }