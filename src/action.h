#ifndef ZELDA_ACTION_H
#define ZELDA_ACTION_H

#include "common.h"
#include "components.h"

struct ActionState {
    string action = "idle";
    Direction direction = DownDirection;
    float timeElapsed = 0.f;
    float duration = 0.f;
};

void changeAction(entt::registry::entity_type entity, const string &action, Direction direction, entt::registry &registry);
void updateAction(entt::registry &registry);
void updateHero(entt::registry &registry);

entt::registry::entity_type createInteractionDetector(entt::registry &registry);
void updateInteractionDetector(b2Body* attachedBody, entt::registry &registry);
void initializeActionSystem(entt::registry &registry);

#endif //ZELDA_ACTION_H
