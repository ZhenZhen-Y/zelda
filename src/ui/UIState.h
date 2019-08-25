#ifndef ZELDA_UISTATE_H
#define ZELDA_UISTATE_H

#include "common.h"

class UIState {
public:
    virtual void activate(entt::registry &registry) = 0;
    virtual void deactivate(entt::registry &registry) = 0;
    virtual void update(float dt, entt::registry &registry) = 0;
    virtual void draw(sf::RenderWindow* window, entt::registry &registry) = 0;
};

#endif //ZELDA_UISTATE_H
