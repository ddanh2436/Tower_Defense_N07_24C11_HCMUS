#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include "cpoint.h"

// Forward-declare cgame và EnemyType để tránh include vòng lặp
class cgame;
struct EnemyType;

enum class EnemyState { WALKING, DYING, DEAD };
enum class MovementDirection { UP, DOWN, SIDE };

struct Animation {
    std::string texturePath;
    sf::Vector2i frameSize;
    int frameCount;
    int stride;
    float duration;
    std::vector<float> yOffsets;
};

class cenemy {
public:
    // Hàm khởi tạo mới, chỉ nhận vào EnemyType
    cenemy(cgame* gameInstance, const EnemyType& type, const std::vector<cpoint>& path);

    void update(sf::Time deltaTime);
    void render(sf::RenderWindow& window);
    bool isAlive() const;
    void takeDamage(int damage);
    sf::Vector2f getPosition() const;
    sf::FloatRect getGlobalBounds() const;
    bool isActive() const { return _isActive; }
    void setActive(bool active) { _isActive = active; }
    bool hasReachedEnd() const;
    bool isReadyForRemoval() const;
    int getMoneyValue() const;

private:
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
    int _health;
    int _maxHealth;
    bool _isActive;
    int _moneyValue;
    EnemyState _currentState;
    MovementDirection _currentDirection;
    std::map<EnemyState, std::map<MovementDirection, Animation>> _animations;
    int _currentFrame;
    sf::Time _elapsedTime;
    sf::Time _timePerFrame;
    sf::IntRect _currentFrameRect;
    sf::RectangleShape _healthBarBackground;
    sf::RectangleShape _healthBarFill;
};