#ifndef ZELDA_UI_H
#define ZELDA_UI_H

#include "common.h"

void initializeUI(entt::registry &registry);
void updateUI(float dt, entt::registry &registry);
void showMessage(const string &message, const string &icon, entt::registry &registry);

#endif //ZELDA_UI_H
