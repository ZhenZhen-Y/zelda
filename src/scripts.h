#ifndef ZELDA_SCRIPTS_H
#define ZELDA_SCRIPTS_H

#include "common.h"
#include <selene/selene.h>

void initializeLuaState(entt::registry &registry);

template <typename T>
T eval(const string &expr, entt::registry &registry) {
    sel::State &state = registry.ctx<sel::State>();
    state((string("result = ") + expr).c_str());
    T result = state["result"];
    return result;
}

inline void eval(const string &expr, entt::registry &registry) {
    sel::State &state = registry.ctx<sel::State>();
    state(expr.c_str());
}

#endif //ZELDA_SCRIPTS_H
