#ifndef ZELDA_CONTACTLISTENER_H
#define ZELDA_CONTACTLISTENER_H

#include "common.h"
#include "components.h"

class ContactListener: public b2ContactListener {
public:
    explicit ContactListener(entt::registry &registry): m_registry(registry) {}
    inline void BeginContact(b2Contact* contact) override {
        void* ud1 = contact->GetFixtureA()->GetUserData();
        void* ud2 = contact->GetFixtureB()->GetUserData();
        if (!ud1 || !ud2) {
            return;
        }
        entt::registry::entity_type ent1 = reinterpret_cast<size_t>(ud1);
        entt::registry::entity_type ent2 = reinterpret_cast<size_t>(ud2);

        auto callback1 = m_registry.try_get<CollisionCallback>(ent1);
        if (callback1 && callback1->beginContact) {
            callback1->beginContact(ent1, ent2, m_registry);
        }

        auto callback2 = m_registry.try_get<CollisionCallback>(ent2);
        if (callback2 && callback2->beginContact) {
            callback2->beginContact(ent2, ent1, m_registry);
        }
    }
    inline void EndContact(b2Contact* contact) override {
        void* ud1 = contact->GetFixtureA()->GetUserData();
        void* ud2 = contact->GetFixtureB()->GetUserData();
        if (!ud1 || !ud2) {
            return;
        }
        entt::registry::entity_type ent1 = reinterpret_cast<size_t>(ud1);
        entt::registry::entity_type ent2 = reinterpret_cast<size_t>(ud2);

        auto callback1 = m_registry.try_get<CollisionCallback>(ent1);
        if (callback1 && callback1->endContact) {
            callback1->endContact(ent1, ent2, m_registry);
        }

        auto callback2 = m_registry.try_get<CollisionCallback>(ent2);
        if (callback2 && callback2->endContact) {
            callback2->endContact(ent2, ent1, m_registry);
        }
    }
private:
    entt::registry &m_registry;
};

#endif //ZELDA_CONTACTLISTENER_H
