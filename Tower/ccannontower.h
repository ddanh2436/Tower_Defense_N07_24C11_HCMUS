﻿#pragma once
#ifndef CCANNONTOWER_H
#define CCANNONTOWER_H

#include "cbasictower.h"

class ccannontower : public cbasictower {
public:
    ccannontower(cgame* game, const TowerLevelData& initialLevelData, const cpoint& position, int id);
    void fireBullet(std::vector<cbullet>& gameBullets, cenemy* target) override;
};

#endif // CCANNONTOWER_H