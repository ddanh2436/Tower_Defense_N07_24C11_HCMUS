#pragma once
#ifndef CARCHERTOWER_H
#define CARCHERTOWER_H

#include "cbasictower.h"

class carchertower : public cbasictower {
public:
    carchertower(cgame* game, const TowerLevelData& initialLevelData, const cpoint& position, int id);

    // ArcherTower không có hành vi đặc biệt nên không cần override gì thêm
};

#endif // CARCHERTOWER_H