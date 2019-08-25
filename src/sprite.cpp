#include "sprite.h"
#include "components.h"
#include "utils.h"
#include "resource.h"
#include <Box2D/Box2D.h>


void updateSpritePosition(entt::registry &registry) {
    registry.view<b2Body*, Sprite>().each(
        [&registry](auto ent, const b2Body* body, Sprite &sprite) {
            if (!sprite.visible()) {
                return;
            }
            auto &p = body->GetPosition();
            sprite.setPosition(meterToPixel(p.x), meterToPixel(p.y));
        });

    registry.view<Binder, Sprite>().each(
        [&registry](auto ent, const Binder &binder, Sprite &sprite) {
            if (!sprite.visible() || !registry.valid(binder.entity)) {
                return;
            }
            auto body = registry.try_get<b2Body*>(binder.entity);
            if (!body) {
                return;
            }
            auto p = (*body)->GetPosition() + binder.offset;
            sprite.setPosition(meterToPixel(p.x), meterToPixel(p.y));
        });
}

void updateSpriteAnimation(float dt, entt::registry &registry) {
    registry.view<AnimationState, Sprite>().each(
        [dt, &registry](auto ent, AnimationState &animState, Sprite &sprite) {
            if (!sprite.visible()) {
                return;
            }
            bool changeFrame = false;
            if (!animState.frame || animState.timeElapsed == 0.0f) {
                animState.timeElapsed += dt;
                changeFrame = true;
            } else {
                animState.timeElapsed += dt;
                if (animState.timeElapsed >= animState.frame->duration) {
                    animState.timeElapsed -= animState.frame->duration;
                    animState.frameIndex += 1;
                    if (animState.loop) {
                        animState.frameIndex = animState.frameIndex % animState.frameSet->frames.size();
                    } else if (animState.frameIndex >= animState.frameSet->frames.size()) {
                        animState.frameIndex = animState.frameSet->frames.size() - 1;
                    }
                    changeFrame = true;
                }
            }

            if (changeFrame) {
                animState.frame = &animState.frameSet->frames[animState.frameIndex];
                auto &rect = animState.frame->rect;
                sprite.setTextureRect(rect);
                sprite.setOrigin(rect.width / 2.0f, rect.height * 2.0f / 3.0f);
            }
        });
}

void renderSprites(entt::registry &registry) {
    auto &rt = registry.ctx<RenderTarget>();
    registry.sort<Sprite>([](const Sprite &s1, const Sprite &s2){
        return s1.getZ() == s2.getZ() ? s1.getPosition().y < s2.getPosition().y : s1.getZ() < s2.getZ();
    });
    registry.view<Sprite>().each([&rt](auto ent, Sprite &sprite) {
        if (!sprite.visible()) {
            return;
        }
        rt.worldRenderTarget.draw(sprite);

        if (entt::monostate<entt::hashed_string{"debug"}>{}) {
            sf::CircleShape shape(2.0f);
            auto &p = sprite.getPosition();
            shape.setPosition(p.x, p.y);
            shape.setOutlineColor(sf::Color::Black);
            shape.setFillColor(sf::Color::Red);
            shape.setOrigin(1.0f, 1.0f);
            rt.worldRenderTarget.draw(shape);
        }
    });
}
