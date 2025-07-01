#include "ctower.h"
#include "cgame.h"
#include <iostream>
#include <cmath>
#include "SoundManager.h"

// =======================================================================
// ==================== HÀM ĐƯỢC SỬA LỖI ===================================
// =======================================================================

ctower::ctower(cgame* game, const std::string& typeId, const TowerLevelData& initialLevelData, const cpoint& position)
    : _gameInstance(game),
    _typeId(typeId),
    _position(position),
    _currentState(State::CONSTRUCTING),
    _level(initialLevelData.level),
    _currentLevelData(initialLevelData),
    _fireCooldown(sf::Time::Zero),
    _animationStartFrame(0),
    _numFramesInLoop(0),
    _currentFrame(0),
    _totalCostInvested(0) // Khởi tạo bằng 0 để an toàn
{
    // ---- THÊM MỚI: Khởi tạo tổng chi phí đầu tư với giá trị ban đầu ----
    _totalCostInvested = initialLevelData.cost;
    // ---- KẾT THÚC ----

    _bulletTexturePath = "assets/bullet.png";
    _sprite.setTexture(_gameInstance->getTexture(_currentLevelData.texturePath));

    _animationStartFrame = _currentLevelData.startFrame;
    _numFramesInLoop = _currentLevelData.numFramesForLevel;

    if (_numFramesInLoop > 0 && _currentLevelData.animationSpeed > 0) {
        _timePerFrame = sf::seconds(_currentLevelData.animationSpeed / _numFramesInLoop);
    }
    else {
        _timePerFrame = sf::Time::Zero;
    }

    _currentFrame = 0;
    _elapsedTime = sf::Time::Zero;

    int initialFrameIndex = _animationStartFrame;
    _sprite.setTextureRect(sf::IntRect(
        initialFrameIndex * _currentLevelData.frameSize.x,
        0,
        _currentLevelData.frameSize.x,
        _currentLevelData.frameSize.y
    ));

    setAnimation(initialLevelData);
    _sprite.setPosition(_position.toVector2f());
}

void ctower::upgrade() {
    const TowerLevelData* nextLevelData = _gameInstance->getTowerNextLevelData(_typeId, _level);
    if (nextLevelData) {

        // ---- THÊM MỚI: Cộng chi phí nâng cấp vào tổng đầu tư ----
        _totalCostInvested += nextLevelData->cost;
        // ---- KẾT THÚC ----

        _level = nextLevelData->level;
        _currentState = State::CONSTRUCTING;
        setAnimation(*nextLevelData);
        std::cout << "Da nang cap thap " << _typeId << " len cap " << _level << ". Tong dau tu: " << _totalCostInvested << std::endl;
    }
}

// =======================================================================
// ==================== CÁC HÀM CÒN LẠI (KHÔNG ĐỔI) =======================
// =======================================================================

void ctower::setAnimation(const TowerLevelData& levelData) {
    _currentLevelData = levelData;

    if (_currentState == State::IDLE && !_currentLevelData.idle_texturePath.empty()) {
        _sprite.setTexture(_gameInstance->getTexture(_currentLevelData.idle_texturePath));
        _animationStartFrame = _currentLevelData.idle_startFrame;
        _numFramesInLoop = _currentLevelData.idle_numFrames;
        if (_numFramesInLoop > 0 && _currentLevelData.idle_animationSpeed > 0) {
            _timePerFrame = sf::seconds(_currentLevelData.idle_animationSpeed / _numFramesInLoop);
        }
        else {
            _timePerFrame = sf::Time::Zero;
        }
    }
    else {
        _sprite.setTexture(_gameInstance->getTexture(_currentLevelData.texturePath));
        _animationStartFrame = _currentLevelData.startFrame;
        _numFramesInLoop = _currentLevelData.numFramesForLevel;
        if (_numFramesInLoop > 0 && _currentLevelData.animationSpeed > 0) {
            _timePerFrame = sf::seconds(_currentLevelData.animationSpeed / _numFramesInLoop);
        }
        else {
            _timePerFrame = sf::Time::Zero;
        }
    }

    _currentFrame = 0;
    _elapsedTime = sf::Time::Zero;

    sf::FloatRect bounds = _sprite.getLocalBounds();
    _sprite.setOrigin(bounds.width / 2.f, bounds.height - _currentLevelData.frameOffsetY);
}


void ctower::updateAnimation(sf::Time deltaTime) {
    if (_numFramesInLoop <= 1 || _timePerFrame == sf::Time::Zero) return;

    _elapsedTime += deltaTime;
    if (_elapsedTime < _timePerFrame) return;

    _elapsedTime -= _timePerFrame;
    _currentFrame = (_currentFrame + 1) % _numFramesInLoop;

    if (_currentState == State::CONSTRUCTING && _currentFrame == 0) {
        _currentState = State::IDLE;
        setAnimation(_currentLevelData);
    }

    int totalFrameIndex = _animationStartFrame + _currentFrame;
    sf::IntRect newRect(
        totalFrameIndex * _currentLevelData.frameSize.x,
        0,
        _currentLevelData.frameSize.x,
        _currentLevelData.frameSize.y
    );
    _sprite.setTextureRect(newRect);
}

void ctower::update(sf::Time deltaTime, std::vector<cenemy>& enemies, std::vector<cbullet>& gameBullets) {
    updateAnimation(deltaTime);

    if (_currentState == State::CONSTRUCTING) {
        return;
    }

    if (_fireCooldown > sf::Time::Zero) {
        _fireCooldown -= deltaTime;
    }

    _target = findTarget(enemies);

    if (_target && _currentState != State::ATTACKING) {
        _currentState = State::ATTACKING;
        setAnimation(_currentLevelData);
    }
    else if (!_target && _currentState != State::IDLE) {
        _currentState = State::IDLE;
        setAnimation(_currentLevelData);
    }

    if (_currentState == State::ATTACKING && _fireCooldown <= sf::Time::Zero) {
        sf::Vector2f targetDirection = _target->getPosition() - _position.toVector2f();
        gameBullets.emplace_back(_bulletTexturePath, _position, targetDirection, _currentLevelData.bulletSpeed, _currentLevelData.damage);
        _fireCooldown = sf::seconds(_currentLevelData.fireRate);
        SoundManager::playSoundEffect("tower_shoot");
    }
}


cenemy* ctower::findTarget(std::vector<cenemy>& enemies) {
    cenemy* target = nullptr;
    float minDistance = _currentLevelData.range;

    for (auto& enemy : enemies) {
        if (enemy.isActive() && enemy.isAlive()) {
            sf::Vector2f enemyPos = enemy.getPosition();
            sf::Vector2f towerPos = _position.toVector2f();
            float dx = enemyPos.x - towerPos.x;
            float dy = enemyPos.y - towerPos.y;
            float distanceSquared = dx * dx + dy * dy;

            if (distanceSquared <= minDistance * minDistance) {
                target = &enemy;
                return target;
            }
        }
    }
    return target;
}

bool ctower::canUpgrade() const {
    return _gameInstance->getTowerNextLevelData(_typeId, _level) != nullptr;
}

int ctower::getUpgradeCost() const {
    const TowerLevelData* nextLevelData = _gameInstance->getTowerNextLevelData(_typeId, _level);
    if (nextLevelData) {
        return nextLevelData->cost;
    }
    return 0;
}

void ctower::render(sf::RenderWindow& window) {
    window.draw(_sprite);
}

sf::Vector2f ctower::getPosition() const {
    return _sprite.getPosition();
}

sf::FloatRect ctower::getGlobalBounds() const {
    return _sprite.getGlobalBounds();
}

int ctower::getSellValue() const {
    return static_cast<int>(_totalCostInvested * 0.75f);
}