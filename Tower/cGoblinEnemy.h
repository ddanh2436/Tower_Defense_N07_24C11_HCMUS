#pragma once
#ifndef CGOBLINENEMY_H
#define CGOBLINENEMY_H

#include "cbasicenemy.h"

class cGoblinEnemy : public cbasicenemy {
public:
    cGoblinEnemy(cgame* gameInstance, const EnemyType& type, int typeIndex, const std::vector<cpoint>& path);
};

#endif // CGOBLINENEMY_H