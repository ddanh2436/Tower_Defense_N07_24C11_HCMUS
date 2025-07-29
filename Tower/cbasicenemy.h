#ifndef CBASICENEMY_H
#define CBASICENEMY_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <string>
#include "cpoint.h"

class cgame;

// ĐỊNH NGHĨA CÁC ENUM VÀ STRUCT Ở ĐÂY
enum EnemyState { WALKING, DYING, DEAD };
enum MovementDirection { UP, DOWN, SIDE };

struct EnemyType {
    float speed;
    int health;
    float scale;
    int moneyValue;
    std::map<EnemyState, std::map<MovementDirection, std::string>> texturePaths;
    sf::Vector2i frameSize;
    int frameCount;
    int stride;
    EnemyType() : speed(0.0f), health(0), scale(1.0f), moneyValue(0), frameCount(0), stride(0) {}
};

struct Animation {
    std::string texturePath;
    sf::Vector2i frameSize;
    int frameCount = 0;
    int stride = 0;
    float duration = 0.0f;
    std::vector<float> yOffsets;
};

class cbasicenemy {
public:
    virtual ~cbasicenemy() = default;

    virtual void update(sf::Time deltaTime);
    virtual void render(sf::RenderWindow& window);
    virtual bool takeDamage(int damage);

    bool isAlive() const;
    sf::Vector2f getPosition() const;
    sf::FloatRect getGlobalBounds() const;
    bool isActive() const;
    void setActive(bool active);
    bool hasReachedEnd() const;
    bool isReadyForRemoval() const;
    int getMoneyValue() const;
    int getId() const;
    int getTypeIndex() const;
    float getHealth() const;
    size_t getPathIndex() const;
    void setHealth(float newHealth);
    void setPosition(const cpoint& pos);
    void setPathIndex(size_t newPathIndex);

protected:
    cbasicenemy(cgame* gameInstance, const EnemyType& type, int typeIndex, const std::vector<cpoint>& path);

    void updateMovement(sf::Time deltaTime);
    void setAnimation(EnemyState state, MovementDirection direction);
    void applyDirectionalFlip(const sf::Vector2f& directionVec);

    cgame* _gameInstance;
    sf::Sprite _sprite;
    cpoint _currentPosition;
    cpoint _targetPosition;
    std::vector<cpoint> _path;
    size_t _currentPathIndex;
    float _speed;
    float _health;
    int _maxHealth;
    bool _isActive;
    int _moneyValue;
    int _typeIndex;
    EnemyState _currentState;
    MovementDirection _currentDirection;
    std::map<EnemyState, std::map<MovementDirection, Animation>> _animations;
    int _currentFrame;
    sf::Time _elapsedTime;
    sf::Time _timePerFrame;
    sf::IntRect _currentFrameRect;
    sf::RectangleShape _healthBarBackground;
    sf::RectangleShape _healthBarFill;

private:
    int _id;
    static int _nextId;
};

#endif // CBASICENEMY_H