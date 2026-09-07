#pragma once
#ifndef CCANNONTOWER_H
#define CCANNONTOWER_H

#include "cbasictower.h"

class ccannontower : public cbasictower {
public:
    ccannontower(cgame* game, const TowerLevelData& initialLevelData, const cpoint& position, int id);
    // The cannon no longer needs its own fireBullet: what makes it a cannon is
    // the splash radius in its level data, which the base class already applies.
};

#endif // CCANNONTOWER_H