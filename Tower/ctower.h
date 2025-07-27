#ifndef CTOWER_H
#define CTOWER_H

#include "cpoint.h"
#include "cbullet.h"
#include "cenemy.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <map>

class cenemy;
class cgame;
class cbullet;

struct TowerLevelData {
    int level;
    int cost;
    sf::Vector2i frameSize;
    int frameOffsetY;

    std::string texturePath;
    int startFrame;
    int numFramesForLevel;
    float animationSpeed;

    std::string idle_texturePath;
    int idle_startFrame;
    int idle_numFrames;
    float idle_animationSpeed;

    float range;
    float fireRate;
    int damage;
    float bulletSpeed;
    std::string bulletTexturePath;

};

class ctower {
public:
    enum State {
        CONSTRUCTING,
        IDLE,
        ATTACKING,
        SELLING,
        UPGRADING
    };
    const TowerLevelData& getCurrentLevelData() const { return _currentLevelData; }

private:
    int _id; // THÊM: ID duy nhất cho mỗi trụ
    sf::Sprite _sprite;
    cpoint _position;
    sf::Time _fireCooldown;

    State _currentState;
    int _level;
    std::string _typeId;

    sf::IntRect _currentFrameRect;
    int _currentFrame;
    int _animationStartFrame;
    int _numFramesInLoop;
    sf::Time _timePerFrame;
    sf::Time _elapsedTime;

    TowerLevelData _currentLevelData;
    cgame* _gameInstance;
    cenemy* _target;
    int _totalCostInvested;

    sf::Time _effectTimer;
    sf::Time _effectDuration;
    bool _isPendingRemoval;

    const TowerLevelData* _pendingUpgradeData;

    void setAnimation(const TowerLevelData& levelData);
    void updateAnimation(sf::Time deltaTime);

public:
    // SỬA ĐỔI: Thêm tham số 'id' vào constructor
    ctower(cgame* game, const std::string& typeId, const TowerLevelData& initialLevelData, const cpoint& position, int id);

    cenemy* findTarget(std::vector<cenemy>& enemies);
    void update(sf::Time deltaTime, std::vector<cenemy>& enemies, std::vector<cbullet>& gameBullets);
    void render(sf::RenderWindow& window);
    sf::Vector2f getPosition() const;

    void upgrade();
    bool canUpgrade() const;
    int getUpgradeCost() const;
    int getSellValue() const;

    sf::FloatRect getGlobalBounds() const;
    int getLevel() const { return _level; }
    std::string getTypeId() const { return _typeId; }
    int getId() const { return _id; } // THÊM: Hàm lấy ID của trụ

    void sell();
    bool isPendingRemoval() const;
    bool isBusy() const;
    State getCurrentState() const;
};

#endif // CTOWER_H