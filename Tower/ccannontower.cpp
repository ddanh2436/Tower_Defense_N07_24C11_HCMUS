#include "ccannontower.h"
#include "SoundManager.h" 

ccannontower::ccannontower(cgame* game, const TowerLevelData& initialLevelData, const cpoint& position, int id)
    : cbasictower(game, "CannonTower", initialLevelData, position, id)
{
    // Lớp con gọi constructor của lớp cha
}
