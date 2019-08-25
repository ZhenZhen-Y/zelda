#include <random>
#include "utils.h"
#include "components.h"
#include "resource.h"
#include "action.h"
#include "sprite.h"

std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wstring_converter;

entt::registry::entity_type
createCharacter(const string &typeName, const string &entityName, float x, float y, Camp camp, entt::registry &registry) {
    auto ent = registry.create();
    const auto &npcDefine = getCharacterDefine(typeName, registry);

    auto &sprite = registry.assign<Sprite>(ent);

    registry.assign<ActionState>(ent);
    auto *frames = &getAnimationFrameSet(npcDefine.spriteName, "idle", DownDirection, registry);
    registry.assign<AnimationState>(ent, npcDefine.spriteName, frames);
    sprite.setTexture(*frames->texture);
    auto &rect = (frames->frames)[0].rect;
    sprite.setOrigin(rect.width / 2.0f, rect.height * 2.0f / 3.0f);
    sprite.setPosition(meterToPixel(x), meterToPixel(y));
    sprite.setZ(1);

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.x = x;
    bodyDef.position.y = y;
    bodyDef.linearDamping = 10.0f;
    bodyDef.fixedRotation = true;
    auto &b2world = registry.ctx<b2World>();
    b2Body* body = b2world.CreateBody(&bodyDef);

    b2CircleShape shape;
    shape.m_radius = 0.8f / 2.0f;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 10.0f;
    fixtureDef.friction = 0.1f;

    b2Fixture* fixture = body->CreateFixture(&fixtureDef);

    fixture->SetUserData(reinterpret_cast<void*>(ent));
    registry.assign<b2Body*>(ent, body);
    registry.assign<Camp>(ent, camp);
    if (npcDefine.hp > 0) {
        registry.assign<Destroyable>(ent, npcDefine.hp);
    }
    registry.assign<DisplayName>(ent, npcDefine.displayName);
    registry.assign<Pack>(ent);
    if (!entityName.empty()) {
        registry.ctx<NamedEntities>().entities[entityName] = ent;
    }
    if (npcDefine.dialogueId > 0) {
        registry.assign<Dialogue>(ent, getDialogue(npcDefine.dialogueId, registry));
    }
    return ent;
}

entt::registry::entity_type
createItem(const string &name, float x, float y, entt::registry &registry) {
    auto ent = registry.create();
    auto &sprite = registry.assign<Sprite>(ent);

    auto &frames = getFrameSet(name, registry);
    sprite.setTexture(*frames.texture);
    auto &rect = (frames.frames)[0].rect;
    if (frames.frames.size() > 1) {
        registry.assign<AnimationState>(ent, name, &frames);
    }

    sprite.setOrigin(rect.width / 2.0f, rect.height * 2.0f / 3.0f);
    sprite.setPosition(meterToPixel(x), meterToPixel(y));
    sprite.setTextureRect(rect);

    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.x = x;
    bodyDef.position.y = y;
    auto &b2world = registry.ctx<b2World>();
    b2Body* body = b2world.CreateBody(&bodyDef);

    b2CircleShape shape;
    shape.m_radius = 0.8f / 2.0f;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.isSensor = true;

    b2Fixture* fixture = body->CreateFixture(&fixtureDef);

    fixture->SetUserData(reinterpret_cast<void*>(ent));
    registry.assign<b2Body*>(ent, body);

    auto &rs = registry.ctx<ResourceManager>();
    registry.assign<Item>(ent, name, rs.items[name].displayName);
    return ent;
}

entt::registry::entity_type
getEntityByName(const string &name, entt::registry &registry) {
    auto &e = registry.ctx<NamedEntities>();
    auto i = e.entities.find(name);
    if (i == e.entities.end()) {
        return entt::null;
    } else {
        return i->second;
    }
}

float getNormalRandom(float c, float e) {
    static std::random_device rd{};
    static std::mt19937 gen{rd()};
    std::normal_distribution<> d{c, e};
    return d(gen);
}
