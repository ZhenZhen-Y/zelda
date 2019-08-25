#include "common.h"

#include "components.h"
#include "input.h"
#include "utils.h"
#include "sprite.h"
#include "resource.h"
#include "misc/SFMLDebugDraw.h"
#include "VertexArray.h"
#include "tilemap.h"
#include "combat.h"
#include "action.h"
#include "ContactListener.h"
#include "ui/ui.h"
#include "scripts.h"
#include "graphic.h"

void initialize(entt::registry &registry, sf::RenderWindow &win) {
    auto &rt = registry.set<RenderTarget>(&win);
    if (!rt.worldShader.loadFromFile("assets/light.vert", "assets/light.frag")) {
        printf("Failed to load shader\n");
    }

    auto &font = registry.set<sf::Font>();
    if (!font.loadFromFile("assets/font.ttc")) {
        throw runtime_error("Failed to load font");
    }

    auto &b2world = registry.set<b2World>(b2Vec2(0.0f, 0.0f));
    auto* debugDraw = new SFMLDebugDraw(rt.worldRenderTarget);
    debugDraw->SetFlags(b2Draw::e_shapeBit);
    b2world.SetDebugDraw(debugDraw);
    b2world.SetContactListener(new ContactListener(registry));

    registry.set<ResourceManager>();
    registry.set<GameState>();
    registry.set<Clock>();
    registry.set<InputState>();
    registry.set<GarbageCollection>();
    registry.set<CallNextFrameCtx>();
    registry.set<NamedEntities>();
    registry.assign<entt::tag<"interactionDetector"_hs>>(createInteractionDetector(registry));

    initializeUI(registry);
    initializeLuaState(registry);
    initializeGraphic(registry);
}

void initializeScene(entt::registry &registry) {
    auto character = createCharacter("hero", "link", 50, 50, FriendlyCamp, registry);
    registry.assign<entt::tag<"hero"_hs>>(character);
    registry.assign<Light>(character, sf::Color::Yellow, 0.7f);

    //auto fire = createItem(51, 51, "fire", registry);
    //registry.assign<Light>(fire, sf::Color::White, 0.5f);
}

void startGame(const string &filename, entt::registry &registry) {
    auto &l = registry.ctx<sel::State>();
    l["filename"] = filename;
    l.Load(filename);
    l["startGame"]();
}

void afterStartGame(entt::registry &registry) {
    initializeActionSystem(registry);
}

void clearGarbage(entt::registry &registry) {
    auto &gc = registry.ctx<GarbageCollection>();
    b2World &world = registry.ctx<b2World>();
    for (auto ent : gc.entities) {
        b2Body** body = registry.try_get<b2Body*>(ent);
        if (body) {
            world.DestroyBody(*body);
        }
        registry.destroy(ent);
    }
    gc.entities.clear();
}

void callNextFrame(entt::registry &registry) {
    auto &ctx = registry.ctx<CallNextFrameCtx>();
    for (auto &f : ctx.functions) {
        f(registry);
    }
    ctx.functions.clear();
}

void updateTween(float dt, entt::registry &registry) {
    registry.view<Tween>().each([dt, &registry](auto ent, Tween &tween) {
        if (tween.physicsTime) {
            tween.timeElapsed += STEP;
        } else {
            tween.timeElapsed += dt;
        }
        float v;
        if (tween.timeElapsed <= tween.duration) {
            v = (tween.endValue - tween.startValue) * (tween.timeElapsed / tween.duration) + tween.startValue;
        } else {
            v = tween.endValue;
            if (tween.loop) {
                float tmp = tween.endValue;
                tween.endValue = tween.startValue;
                tween.startValue = tmp;
                tween.reverse = !tween.reverse;
                tween.timeElapsed = 0.0f;
            } else {
                //registry.remove<Tween>(ent);
            }
        }
        tween.setter(v);
    });
}

void updateClock(entt::registry &registry) {
    auto &clock = registry.ctx<Clock>();
    clock.elapse(STEP / TIME_SCALE);
}

void updateDuration(entt::registry &registry) {
    registry.view<Duration>().each([&registry](auto ent, Duration &duration) {
        duration.timeElapsed += STEP;
        if (duration.timeElapsed > duration.duration) {
            destroyEntity(ent, registry);
        }
    });
}

int main(int narg, char* argv[])  {
    try {
        if (narg <= 1) {
            printf("Specify lua script!\n");
            return 1;
        }

        sf::ContextSettings settings;
        sf::RenderWindow window(sf::VideoMode(1280, 768), "the Legend of Zelda", sf::Style::Default, settings);
        window.setFramerateLimit(60);
        window.setVerticalSyncEnabled(true);

        entt::registry registry;
        registry.create();  //skip ent 0， Creates a new entity

        initialize(registry, window);
        startGame(argv[1], registry);
        afterStartGame(registry);
        initializeScene(registry);

        sf::Clock tick;

        entt::monostate<entt::hashed_string{"debug"}>{} = false;

        auto &rt = registry.ctx<RenderTarget>();
        sf::Sprite worldRenderSprite;
        worldRenderSprite.setPosition(0, 0);
        auto &b2world = registry.ctx<b2World>();
        float accumulator = 0.0f;
        while (window.isOpen()) {
            window.clear(sf::Color::Black);
            rt.worldRenderTarget.clear(sf::Color::Black);
            float dt = tick.restart().asSeconds();

            updateInputState(registry);
            accumulator += dt;

            // Perform game logic
            while (accumulator >= STEP) {
                b2world.Step(STEP, 8, 3);
                accumulator -= STEP;

                updateClock(registry);
                updateAction(registry);
                updateHero(registry);
                updateDuration(registry);
                updateTween(dt, registry);

                callNextFrame(registry);
                clearGarbage(registry);
            }

            // Update world display
            updateSpritePosition(registry);
            updateSpriteAnimation(dt, registry);

            // Render world
            rt.worldRenderTarget.setView(rt.worldView);
            renderTileMap(registry);
            renderSprites(registry);
            rt.worldRenderTarget.display();

            if (entt::monostate<entt::hashed_string{"debug"}>{}) {
                b2world.DrawDebugData();
            }
            worldRenderSprite.setTexture(rt.worldRenderTarget.getTexture());
            updateGraphic(registry);

            window.draw(worldRenderSprite, &rt.worldShader);

            // Update UI
            updateUI(dt, registry);

            window.display();
        }
    } catch (runtime_error &e) {
        printf("Unhandled exception: %s\n", e.what());
    }

    return 0;
}
