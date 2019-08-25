#include "gtest/gtest.h"
#include "resource.cpp"


TEST(test_loadSpriteSheet, test_loadSpriteSheet) {
    entt::registry registry;
    registry.set<ResourceManager>();
    loadSpriteSheet("assets/hero.csv", registry);

    auto &fs = getFrameSet("hero.move.down", registry);
    for (auto &f : fs.frames) {
        printf("%f\n", f.duration);
    }
}

