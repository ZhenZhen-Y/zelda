#include "action.h"
#include "resource.h"
#include "utils.h"
#include "combat.h"
#include "input.h"
#include "ui/ui.h"

void changeAction(entt::registry::entity_type entity, const string &action, Direction direction, entt::registry &registry) {
    auto &actionState = registry.get<ActionState>(entity);
    map<string, float> durationMap = {{"idle", .0f}, {"move", .0f}, {"attack", 0.5f}};
    bool updateAnimation = false;
    if (action != actionState.action || direction != actionState.direction) {
        updateAnimation = true;
    }
    actionState.action = action;
    actionState.direction = direction;
    actionState.duration = durationMap[action];
    actionState.timeElapsed = 0.f;

    if (updateAnimation) {
        auto* animationState = registry.try_get<AnimationState>(entity);
        if (animationState) {
            animationState->timeElapsed = 0.0f;
            animationState->loop = (actionState.duration == 0.0f);
            animationState->frameSet = &getAnimationFrameSet(animationState->spriteName, actionState.action, actionState.direction, registry);
            animationState->frame = &animationState->frameSet->frames[0];
            animationState->frameIndex = 0;
        }
    }
}

void updateAction(entt::registry &registry) {
    registry.view<ActionState>().each([&registry](auto ent, ActionState &actionState) {
        if (actionState.duration > 0.f) {
            actionState.timeElapsed += STEP;
            if (actionState.timeElapsed > actionState.duration) {
                changeAction(ent, "idle", actionState.direction, registry);
            }
        }
    });
}

void updateHero(entt::registry &registry)  {
    registry.view<entt::tag<"hero"_hs>, ActionState>().each([&registry](
        auto ent, auto _, auto &actionState) {
        b2Body* body = registry.get<b2Body*>(ent);
        auto &position = body->GetPosition();
        auto &rt = registry.ctx<RenderTarget>();
        rt.worldView.setCenter(meterToPixel(position.x), meterToPixel(position.y));
        GameState& gameState = registry.ctx<GameState>();
        if (gameState != NormalState) {
            return;
        }

        if (actionState.action == "attack") {
            return;
        }

        InputState& input = registry.ctx<InputState>();
        float speed = 5.0f;
        if (input.btn1) {
            changeAction(ent, "attack", actionState.direction, registry);
            b2Vec2 p = body->GetPosition();
            meleeAttack(p.x, p.y, actionState.direction, registry);
        } else if (input.btnOKPressed()) {
            Interaction &interaction = registry.ctx<Interaction>();
            if (interaction.objects.size() > 0) {
                interaction.interact(ent, interaction.objects.front(), registry);
            }
        } else if (input.down) {
            changeAction(ent, "move", DownDirection, registry);
        } else if (input.up) {
            changeAction(ent, "move", UpDirection, registry);
        } else if (input.left) {
            changeAction(ent, "move", LeftDirection, registry);
        } else if (input.right) {
            changeAction(ent, "move", RightDirection, registry);
        } else {
            changeAction(ent, "idle", actionState.direction, registry);
        }

        if (actionState.action == "move") {
            b2Vec2 v = directionToVec2(actionState.direction);
            v *= speed;
            body->SetLinearVelocity(v);
            updateInteractionDetector(body, registry);
        }
    });
}

void setInteractionMark(entt::registry::entity_type interactable, InteractCallback callback, entt::registry &registry) {
    auto &interaction = registry.ctx<Interaction>();
    interaction.objects.push_front(interactable);
    auto &binder = registry.get<Binder>(interaction.mark);
    binder.entity = interactable;
    auto &sprite = registry.get<Sprite>(interaction.mark);
    sprite.setVisible(true);
    interaction.interact = callback;
}

void talk(entt::registry::entity_type, entt::registry::entity_type, entt::registry &registry) {
    auto &gameState = registry.ctx<GameState>();
    gameState = DialogueState;
}

void pickup(entt::registry::entity_type subject, entt::registry::entity_type object, entt::registry &registry) {
    auto &item = registry.get<Item>(object);
    auto &pack = registry.get<Pack>(subject);
    pack.addItem(item.name, 1);
    showMessage(item.displayName + " +1", item.name, registry);
    destroyEntity(object, registry);
}

void enableInteraction(entt::registry::entity_type detector, entt::registry::entity_type interactable, entt::registry &registry) {
    if (registry.try_get<Dialogue>(interactable)) {
        setInteractionMark(interactable, talk, registry);
    } else if (registry.try_get<Item>(interactable)) {
        setInteractionMark(interactable, pickup, registry);
    }
}

void disableInteraction(entt::registry::entity_type detector, entt::registry::entity_type interactable, entt::registry &registry) {
    auto &interaction = registry.ctx<Interaction>();
    for (auto obj : interaction.objects) {
        if (obj == interactable) {
            interaction.objects.remove(obj);
            auto &binder = registry.get<Binder>(interaction.mark);
            auto &sprite = registry.get<Sprite>(interaction.mark);
            if (interaction.objects.size() > 0) {
                binder.entity = interaction.objects.front();
            } else {
                binder.entity = entt::null;
                sprite.setVisible(false);
            }
            break;
        }
    }
}

entt::registry::entity_type
createInteractionDetector(entt::registry &registry) {
    auto ent = registry.create();

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.linearDamping = 10.0f;
    bodyDef.fixedRotation = true;
    auto &b2world = registry.ctx<b2World>();
    b2Body* body = b2world.CreateBody(&bodyDef);
    b2PolygonShape shape;
    shape.SetAsBox(0.6f, 0.6f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.isSensor = true;

    auto fixture = body->CreateFixture(&fixtureDef);
    fixture->SetUserData(reinterpret_cast<void*>(ent));

    registry.assign<b2Body*>(ent, body);
    registry.assign<CollisionCallback>(ent, enableInteraction, disableInteraction);

    return ent;
}

void updateInteractionDetector(b2Body* attachedBody, entt::registry &registry) {
    registry.view<entt::tag<"interactionDetector"_hs>, b2Body*>().each([&registry, attachedBody](auto ent, auto _, auto body){
        b2Vec2 v = attachedBody->GetLinearVelocity();
        body->SetLinearVelocity(v);
        v.Normalize();
        v *= 0.5f;
        body->SetTransform(attachedBody->GetPosition() + v, 0.0f);
    });
}

void initializeActionSystem(entt::registry &registry) {
    auto markEnt = registry.create();
    auto &sprite = registry.assign<Sprite>(markEnt);
    auto &f = getFrameSet("cursor", registry);
    sprite.setTexture(*f.texture);
    sprite.setTextureRect(f.frames[0].rect);
    sprite.setZ(10);
    sprite.setOrigin(8, 8);
    sprite.setScale(0.5, 0.5);
    auto &cursorBinder = registry.assign<Binder>(markEnt, entt::null, b2Vec2{0.0f, -1.0f});
    registry.set<Interaction>(markEnt);

    createTweenEntity(-1.1f, -1.2f, 0.5f, [&cursorBinder](float v){
        cursorBinder.offset.y = v;
    }, registry);
}
