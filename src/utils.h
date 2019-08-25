#ifndef ZELDA_UTILS_H
#define ZELDA_UTILS_H

#include "common.h"
#include "components.h"

#include <algorithm>
#include <cctype>
#include <locale>
#include <codecvt>

extern std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wstring_converter;

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

entt::registry::entity_type
createCharacter(const string &typeName, const string &entityName, float x, float y, Camp camp, entt::registry &registry);

entt::registry::entity_type
createItem(const string &name, float x, float y, entt::registry &registry);

entt::registry::entity_type
getEntityByName(const string &name, entt::registry &registry);

inline float meterToPixel(float meter) {
    return 16.0f * meter;
}

inline b2Vec2 meterToPixel(b2Vec2 meter) {
    meter *= 16;
    return meter;
}

inline b2Vec2 directionToVec2(Direction direction) {
    const b2Vec2 vecMap[4] = {{0, 1}, {-1, 0}, {0, -1}, {1, 0}};
    return vecMap[direction];
}

inline void destroyEntity(entt::registry::entity_type ent, entt::registry &registry) {
    auto &gc = registry.ctx<GarbageCollection>();
    gc.entities.push_back(ent);
}

inline entt::registry::entity_type createTweenEntity(float from, float to, float duration, TweenSetter setter, entt::registry &registry) {
    auto ent = registry.create();
    registry.assign<Tween>(ent, from, to, duration, setter);
    return ent;
}

inline void resetTweenEntity(entt::registry::entity_type ent, entt::registry &registry) {
    auto &tween = registry.get<Tween>(ent);
    tween.reset();
}

float getNormalRandom(float a, float b);

#endif //ZELDA_UTILS_H
