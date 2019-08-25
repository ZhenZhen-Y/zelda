#ifndef ZELDA_RESOURCE_H
#define ZELDA_RESOURCE_H

#include "common.h"
#include "components.h"

struct ResourceManager {
    map<string, sf::Texture> textures;
    map<string, TextureFrameSet> frameset;
    map<string, ItemDefine> items;
    map<string, CharacterDefine> npcs;
    map<int, Dialogue> dialogues;
};

void loadSpriteSheet(const string& filename, entt::registry &registry);

const TextureFrameSet& getFrameSet(const string &key,
   entt::registry &registry);

const TextureFrameSet& getAnimationFrameSet(
    const string &spriteName,
    const string &actionName,
    Direction direction,
    entt::registry &registry);

sf::Texture& getTexture(const string& key, entt::registry &registry);

void addFrame(const string &key,
    sf::IntRect rect,
    const string& textureName,
    float duration,
    entt::registry &registry);

void dumpSpriteSheets(entt::registry &registry);

void loadDialogues(const string &filename, entt::registry &registry);
const Dialogue& getDialogue(int id, entt::registry &registry);

void loadItemDefines(const string &filename, entt::registry &registry);
const ItemDefine &getItemDefine(const string& name, entt::registry &registry);

void loadCharacterDefines(const string &filename, entt::registry &registry);
const CharacterDefine &getCharacterDefine(const string &typeName, entt::registry &registry);
#endif //ZELDA_RESOURCE_H
