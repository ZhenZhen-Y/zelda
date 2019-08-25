#ifndef ZELDA_SPRITE_H
#define ZELDA_SPRITE_H

#include "common.h"

void updateSpritePosition(entt::registry &registry);
void updateSpriteAnimation(float dt, entt::registry &registry);
void renderSprites(entt::registry &registry);

#endif //ZELDA_SPRITE_H
