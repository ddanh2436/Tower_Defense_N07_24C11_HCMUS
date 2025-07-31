#pragma once
#ifndef CBASICTOWER_H
#define CBASICTOWER_H

#include "cpoint.h"
#include "cbullet.h"
#include "cenemy.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class cgame; // Forward declaration

struct TowerLevelData {
    int level = 0;
    int cost = 0;
    sf::Vector2i frameSize = { 0, 0 };
    int frameOffsetY = 0;
    std::string texturePath;
    int startFrame = 0; // Initialize startFrame to 0
    int numFramesForLevel = 0;
    float animationSpeed = 0.0f;
    std::string idle_texturePath;
    int idle_startFrame = 0;
    int idle_numFrames = 0;
    float idle_animationSpeed = 0.0f;
    float range = 0.0f;
    float fireRate = 0.0f;
    int damage = 0;
    float bulletSpeed = 0.0f;
    std::string bulletTexturePath;
};

class cbasictower {
public:
    enum State {
        CONSTRUCTING,
        IDLE,
        ATTACKING,
        SELLING,
        UPGRADING
    };

    virtual ~cbasictower() = default;

    virtual void update(sf::Time deltaTime, std::vector<cenemy>& enemies, std::vector<cbullet>& gameBullets);
    virtual void render(sf::RenderWindow& window);

    // Public getters and setters
    const TowerLevelData& getCurrentLevelData() const { return _currentLevelData; }
    sf::Vector2f getPosition() const;
    sf::FloatRect getGlobalBounds() const;
    int getLevel() const { return _level; }
    std::string getTypeId() const { return _typeId; }
    int getId() const { return _id; }
    bool isPendingRemoval() const;
    bool isBusy() const;
    State getCurrentState() const;
    bool canUpgrade() const;
    int getUpgradeCost() const;
    int getSellValue() const;

    // Public actions
    void upgrade();
    void sell();

protected:
    cbasictower(cgame* game, const std::string& typeId, const TowerLevelData& initialLevelData, const cpoint& position, int id);

    virtual void fireBullet(std::vector<cbullet>& gameBullets, cenemy* target);

    void setAnimation(const TowerLevelData& levelData);
    int findTarget(std::vector<cenemy>& enemies);

    // Member variables
    int _id;
    sf::Sprite _sprite;
    cpoint _position;
    sf::Time _fireCooldown;
    State _currentState;
    int _level;
    std::string _typeId;
    int _targetID;
    TowerLevelData _currentLevelData;
    cgame* _gameInstance;
    int _totalCostInvested;
    sf::Time _effectTimer;
    sf::Time _effectDuration;
    bool _isPendingRemoval;
    const TowerLevelData* _pendingUpgradeData;

private:
    void updateAnimation(sf::Time deltaTime);

    // Animation variables
    int _currentFrame;
    int _animationStartFrame;
    int _numFramesInLoop;
    sf::Time _timePerFrame;
    sf::Time _elapsedTime;
};

#endif // CBASICTOWER_H