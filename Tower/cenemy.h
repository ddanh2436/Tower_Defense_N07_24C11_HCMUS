#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <string>
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
    // Split into two passes so the game can draw every enemy body first and
    // every health bar afterwards. Drawing each enemy's bar right after its
    // own sprite meant a bar could be painted over the enemy standing in
    // front of it, and hidden behind the one behind it.
    void render(sf::RenderWindow& window);
    void renderHealthBar(sf::RenderWindow& window);
    float getRenderDepth() const;
    bool isAlive() const;
    // Armour subtracts a flat amount from every hit, so a few heavy shots beat
    // many light ones. ignoreArmour is used by the player ability.
    bool takeDamage(int damage, bool ignoreArmour = false);
    int getArmour() const;
    int getLivesCost() const;
    const std::string& getName() const;
    sf::Vector2f getPosition() const;
    sf::FloatRect getGlobalBounds() const;
    bool isActive() const { return _isActive; }
    void setActive(bool active) { _isActive = active; }
    bool hasReachedEnd() const;
    bool isReadyForRemoval() const;
    int getMoneyValue() const;
    int getId() const;

    // --- CÁC HÀM MỚI ĐỂ HỖ TRỢ LƯU/TẢI GAME ---
    int getTypeIndex() const;
    float getHealth() const;
    size_t getPathIndex() const;
    void setHealth(float newHealth);
    void setPosition(const cpoint& pos);
    void setPathIndex(int newPathIndex);

private:
    void updateMovement(sf::Time deltaTime);
    void setAnimation(EnemyState state, MovementDirection direction);
    void applyDirectionalFlip(const sf::Vector2f& directionVec);

    int _id;
    static int _nextId;

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
    int _armour;
    int _livesCost;   // Lives lost if this enemy reaches the end.
    std::string _name;
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