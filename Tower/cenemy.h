#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include "cpoint.h"

// Forward-declare cgame và EnemyType để tránh include vòng lặp
class cgame;
struct EnemyType;

enum EnemyState { WALKING, DYING, DEAD };
enum MovementDirection { UP, DOWN, SIDE };

struct Animation {
    std::string texturePath;
    sf::Vector2i frameSize;
    int frameCount = 0;
    int stride = 0;
    float duration = 0.0f;
    std::vector<float> yOffsets;
};


class cenemy {
public:
    // SỬA ĐỔI: Thêm typeIndex vào hàm khởi tạo
    cenemy(cgame* gameInstance, const EnemyType& type, int typeIndex, const std::vector<cpoint>& path);

    void update(sf::Time deltaTime);
    void render(sf::RenderWindow& window);
    bool isAlive() const;
    bool takeDamage(int damage);
    sf::Vector2f getPosition() const;
    sf::FloatRect getGlobalBounds() const;
    bool isActive() const { return _isActive; }
    void setActive(bool active) { _isActive = active; }
    bool hasReachedEnd() const;
    bool isReadyForRemoval() const;
    int getMoneyValue() const;

    // --- CÁC HÀM MỚI ĐỂ HỖ TRỢ LƯU/TẢI GAME ---
    int getTypeIndex() const;
    float getHealth() const;
    int getPathIndex() const;
    void setHealth(float newHealth);
    void setPosition(const cpoint& pos);
    void setPathIndex(int newPathIndex);

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
    float _health; // SỬA ĐỔI: Chuyển sang float để lưu máu chính xác hơn
    int _maxHealth;
    bool _isActive;
    int _moneyValue;
    int _typeIndex; // THÊM MỚI: Biến để lưu chỉ số của loại quái vật
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