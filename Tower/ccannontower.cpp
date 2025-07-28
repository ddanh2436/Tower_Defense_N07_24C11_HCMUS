#include "ccannontower.h"
#include "SoundManager.h" // Cần để gọi âm thanh riêng

ccannontower::ccannontower(cgame* game, const TowerLevelData& initialLevelData, const cpoint& position, int id)
    : cbasictower(game, "CannonTower", initialLevelData, position, id)
{
    // Lớp con gọi constructor của lớp cha
}

void ccannontower::fireBullet(std::vector<cbullet>& gameBullets, cenemy* target) {
    // Hành vi bắn đặc biệt của Trụ Pháo
    sf::Vector2f targetDirection = target->getPosition() - _position.toVector2f();

    // Sát thương của trụ pháo được tăng thêm 10 (ví dụ)
    int cannonDamage = _currentLevelData.damage + 10;

    gameBullets.emplace_back(
        _currentLevelData.bulletTexturePath,
        _position,
        targetDirection,
        _currentLevelData.bulletSpeed,
        cannonDamage
    );

    // Sử dụng âm thanh bắn khác
    SoundManager::playSoundEffect("assets/tower_shoot.ogg"); // Thay bằng âm thanh của bạn
}