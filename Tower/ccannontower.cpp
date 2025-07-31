#include "ccannontower.h"
#include "SoundManager.h" 

ccannontower::ccannontower(cgame* game, const TowerLevelData& initialLevelData, const cpoint& position, int id)
    : cbasictower(game, "CannonTower", initialLevelData, position, id)
{
    // Lớp con gọi constructor của lớp cha
}

void ccannontower::fireBullet(std::vector<cbullet>& gameBullets, cenemy* target) {
    sf::Vector2f targetDirection = target->getPosition() - _position.toVector2f();
    int cannonDamage = _currentLevelData.damage + 10;

    gameBullets.emplace_back(
        _currentLevelData.bulletTexturePath,
        _position,
        targetDirection,
        _currentLevelData.bulletSpeed,
        cannonDamage
    );

    SoundManager::playSoundEffect("assets/tower_shoot.ogg"); 
}