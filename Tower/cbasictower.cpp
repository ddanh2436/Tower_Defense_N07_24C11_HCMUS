#include "cbasictower.h"
#include "cgame.h"
#include "SoundManager.h"
#include <iostream>
#include <cmath>
#include <algorithm>

cbasictower::cbasictower(cgame* game, const std::string& typeId, const TowerLevelData& initialLevelData, const cpoint& position, int id)
    : _gameInstance(game),
    _typeId(typeId),
    _position(position),
    _id(id),
    _currentState(State::CONSTRUCTING),
    _level(initialLevelData.level),
    _fireCooldown(sf::Time::Zero),
    _totalCostInvested(initialLevelData.cost),
    _isPendingRemoval(false),
    _pendingUpgradeData(nullptr),
    _targetID(-1)
{
    setAnimation(initialLevelData);
    _sprite.setPosition(_position.toVector2f());
}

void cbasictower::setAnimation(const TowerLevelData& levelData) {
    _currentLevelData = levelData;
    if (_currentState == State::IDLE && !_currentLevelData.idle_texturePath.empty()) {
        _sprite.setTexture(_gameInstance->getTexture(_currentLevelData.idle_texturePath));
        _animationStartFrame = _currentLevelData.idle_startFrame;
        _numFramesInLoop = _currentLevelData.idle_numFrames;
        _timePerFrame = (_numFramesInLoop > 0 && _currentLevelData.idle_animationSpeed > 0) ? sf::seconds(_currentLevelData.idle_animationSpeed / _numFramesInLoop) : sf::Time::Zero;
    }
    else {
        _sprite.setTexture(_gameInstance->getTexture(_currentLevelData.texturePath));
        _animationStartFrame = _currentLevelData.startFrame;
        _numFramesInLoop = _currentLevelData.numFramesForLevel;
        _timePerFrame = (_numFramesInLoop > 0 && _currentLevelData.animationSpeed > 0) ? sf::seconds(_currentLevelData.animationSpeed / _numFramesInLoop) : sf::Time::Zero;
    }
    _currentFrame = 0;
    _elapsedTime = sf::Time::Zero;
    sf::IntRect firstFrameRect(_animationStartFrame * _currentLevelData.frameSize.x, 0, _currentLevelData.frameSize.x, _currentLevelData.frameSize.y);
    _sprite.setTextureRect(firstFrameRect);
    _sprite.setOrigin(static_cast<float>(_currentLevelData.frameSize.x) / 2.f, static_cast<float>(_currentLevelData.frameSize.y) - static_cast<float>(_currentLevelData.frameOffsetY));
}

void cbasictower::updateAnimation(sf::Time deltaTime) {
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
    sf::IntRect newRect(totalFrameIndex * _currentLevelData.frameSize.x, 0, _currentLevelData.frameSize.x, _currentLevelData.frameSize.y);
    _sprite.setTextureRect(newRect);
}

void cbasictower::update(sf::Time deltaTime, std::vector<cenemy>& enemies, std::vector<cbullet>& gameBullets) {
    if (_currentState == State::SELLING) {
        _effectTimer += deltaTime;
        if (_effectTimer >= _effectDuration) _isPendingRemoval = true;
        return;
    }
    if (_currentState == State::UPGRADING) {
        _effectTimer += deltaTime;
        if (_effectTimer >= _effectDuration) {
            _level = _pendingUpgradeData->level;
            _currentState = State::CONSTRUCTING;
            setAnimation(*_pendingUpgradeData);
            _pendingUpgradeData = nullptr;
        }
        return;
    }
    updateAnimation(deltaTime);
    if (_currentState == State::CONSTRUCTING) return;
    if (_fireCooldown > sf::Time::Zero) _fireCooldown -= deltaTime;

    _targetID = findTarget(enemies);
    cenemy* currentTarget = nullptr;
    if (_targetID != -1) {
        for (auto& enemy : enemies) {
            if (enemy.getId() == _targetID) {
                currentTarget = &enemy;
                break;
            }
        }
    }

    if (currentTarget) {
        if (_currentState != State::ATTACKING) {
            _currentState = State::ATTACKING;
            setAnimation(_currentLevelData);
        }
    }
    else {
        if (_currentState != State::IDLE) {
            _currentState = State::IDLE;
            setAnimation(_currentLevelData);
        }
    }

    if (_currentState == State::ATTACKING && _fireCooldown <= sf::Time::Zero && currentTarget) {
        fireBullet(gameBullets, currentTarget);
        _fireCooldown = sf::seconds(_currentLevelData.fireRate);
    }
}

void cbasictower::fireBullet(std::vector<cbullet>& gameBullets, cenemy* target) {
    sf::Vector2f targetDirection = target->getPosition() - _position.toVector2f();
    gameBullets.emplace_back(_currentLevelData.bulletTexturePath, _position, targetDirection, _currentLevelData.bulletSpeed, _currentLevelData.damage);
    SoundManager::playSoundEffect("assets/tower_shoot.ogg");
}

int cbasictower::findTarget(std::vector<cenemy>& enemies) {
    int bestTargetId = -1;
    float minDistanceSq = _currentLevelData.range * _currentLevelData.range;
    for (auto& enemy : enemies) {
        if (enemy.isActive() && enemy.isAlive()) {
            sf::Vector2f enemyPos = enemy.getPosition();
            sf::Vector2f towerPos = _position.toVector2f();
            float dx = enemyPos.x - towerPos.x;
            float dy = enemyPos.y - towerPos.y;
            float distanceSquared = dx * dx + dy * dy;
            if (distanceSquared < minDistanceSq) {
                minDistanceSq = distanceSquared;
                bestTargetId = enemy.getId();
            }
        }
    }
    return bestTargetId;
}

void cbasictower::render(sf::RenderWindow& window) {
    if (_currentState == State::SELLING) {
        float ratio = _effectTimer.asSeconds() / _effectDuration.asSeconds();
        sf::Uint8 alpha = static_cast<sf::Uint8>(255 * (1.f - std::min(1.f, ratio)));
        _sprite.setColor(sf::Color(255, 255, 255, alpha));
    }
    else if (_currentState == State::UPGRADING) {
        int flashInterval_ms = 80;
        bool isVisible = (static_cast<int>(_effectTimer.asMilliseconds()) / flashInterval_ms) % 2 == 0;
        _sprite.setColor(isVisible ? sf::Color(255, 255, 0, 255) : sf::Color::White);
    }
    else {
        _sprite.setColor(sf::Color::White);
    }
    window.draw(_sprite);
}

void cbasictower::upgrade() {
    const TowerLevelData* nextLevelData = _gameInstance->getTowerNextLevelData(_typeId, _level);
    if (nextLevelData) {
        _totalCostInvested += nextLevelData->cost;
        _currentState = State::UPGRADING;
        _effectTimer = sf::Time::Zero;
        _effectDuration = sf::seconds(0.4f);
        _pendingUpgradeData = nextLevelData;
    }
}

void cbasictower::sell() {
    if (_currentState != State::SELLING) {
        _currentState = State::SELLING;
        _effectTimer = sf::Time::Zero;
        _effectDuration = sf::seconds(0.2f);
    }
}

// Implementations for getters
bool cbasictower::canUpgrade() const { return _gameInstance->getTowerNextLevelData(_typeId, _level) != nullptr; }
int cbasictower::getUpgradeCost() const {
    const TowerLevelData* data = _gameInstance->getTowerNextLevelData(_typeId, _level);
    return data ? data->cost : 0;
}
int cbasictower::getSellValue() const { return static_cast<int>(_totalCostInvested * 0.75f); }
sf::Vector2f cbasictower::getPosition() const { return _sprite.getPosition(); }
sf::FloatRect cbasictower::getGlobalBounds() const { return _sprite.getGlobalBounds(); }
bool cbasictower::isPendingRemoval() const { return _isPendingRemoval; }
bool cbasictower::isBusy() const { return _currentState == State::CONSTRUCTING || _currentState == State::UPGRADING || _currentState == State::SELLING; }
cbasictower::State cbasictower::getCurrentState() const { return _currentState; }