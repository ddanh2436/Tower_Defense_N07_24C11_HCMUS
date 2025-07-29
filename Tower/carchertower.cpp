#include "carchertower.h"

carchertower::carchertower(cgame* game, const TowerLevelData& initialLevelData, const cpoint& position, int id)
    : cbasictower(game, "ArcherTower", initialLevelData, position, id)
{
    // Lớp con gọi constructor của lớp cha
}