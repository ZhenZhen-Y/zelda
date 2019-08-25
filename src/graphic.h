#ifndef ZELDA_GRAPHIC_H
#define ZELDA_GRAPHIC_H

#include "common.h"

void initializeGraphic(entt::registry &registry);
void loadEnvironmentLightSheet(const string &filename, entt::registry &registry);
void updateGraphic(entt::registry &registry);

#endif //ZELDA_GRAPHIC_H
