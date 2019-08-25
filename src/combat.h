#ifndef ZELDA_COMBAT_H
#define ZELDA_COMBAT_H

#include "common.h"
#include "components.h"

struct Damage {
    unsigned int affectedCampMask = 0x0;
    int damage;
};

void meleeAttack(float x, float y, Direction direction, entt::registry &registry);

#endif //ZELDA_COMBAT_H
