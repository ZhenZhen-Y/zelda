#include "combat.h"
#include "utils.h"
#include "resource.h"

void resolveDamage(entt::registry::entity_type damageEnt, entt::registry::entity_type destroyableEnt, entt::registry &registry) {
    Camp* camp = registry.try_get<Camp>(destroyableEnt);
    Destroyable* destroyable = registry.try_get<Destroyable>(destroyableEnt);
    Damage* damage = registry.try_get<Damage>(damageEnt);
    if (!camp || !destroyable || !damage || !((unsigned int)*camp & damage->affectedCampMask)) {
        return;
    }

    destroyable->hp -= damage->damage;
    if (destroyable->hp <= 0) {
        destroyable->hp = 0;

        //death animation
        auto ent = registry.create();
        registry.assign<Duration>(ent, 0.5);
        auto *frames = &getFrameSet("death", registry);
        registry.assign<AnimationState>(ent, "death", frames);
        auto &sprite = registry.assign<Sprite>(ent);
        sprite.setTexture(*frames->texture);
        auto &rect = (frames->frames)[0].rect;
        sprite.setOrigin(rect.width / 2.0f, rect.height * 2.0f / 3.0f);
        auto position = registry.get<b2Body*>(destroyableEnt)->GetPosition();
        sprite.setPosition(meterToPixel(position.x), meterToPixel(position.y));
        sprite.setScale(0.5, 0.5);
        sprite.setZ(1);

        //drop items
        Pack* pack = registry.try_get<Pack>(destroyableEnt);
        if (pack) {
            auto items = pack->items;
            auto &callCtx = registry.ctx<CallNextFrameCtx>();
            callCtx.functions.emplace_back([items, position](entt::registry &registry){
                for (auto &item : items) {
                    for (int i = 0; i < item.second; ++i) {
                        float x, y;
                        const float r = 0.5;
                        x = getNormalRandom(position.x, r);
                        y = getNormalRandom(position.y, r);
                        createItem(item.first, x, y, registry);
                    }
                }
            });
        }

        destroyEntity(destroyableEnt, registry);
    }
}

void meleeAttack(float x, float y, Direction direction, entt::registry &registry) {
    auto &b2world = registry.ctx<b2World>();
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.x = x;
    bodyDef.position.y = y;
    bodyDef.linearDamping = 10.0f;
    bodyDef.angle = b2_pi / 2 * direction;
    b2Body* body = b2world.CreateBody(&bodyDef);
    b2PolygonShape shape;
    shape.SetAsBox(0.8f, 0.1f);

    b2FixtureDef fixtureDef;
    fixtureDef.isSensor = true;
    fixtureDef.shape = &shape;

    b2Fixture* fixture = body->CreateFixture(&fixtureDef);
    auto v = directionToVec2(direction);
    v *= 10;
    body->SetLinearVelocity(v);

    auto ent = registry.create();
    fixture->SetUserData(reinterpret_cast<void*>(ent));
    registry.assign<Duration>(ent, 0.5);
    registry.assign<Damage>(ent, EnemyCamp, 50);
    registry.assign<CollisionCallback>(ent, resolveDamage);
    registry.assign<b2Body*>(ent, body);
}
