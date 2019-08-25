#ifndef ZELDA_COMPONENTS_H
#define ZELDA_COMPONENTS_H

#include "common.h"
#include "VertexArray.h"

struct Binder {
    entt::registry::entity_type entity;
    b2Vec2 offset;
};

enum Direction {
    DownDirection = 0,
    LeftDirection,
    UpDirection,
    RightDirection,
};

enum Camp {
    FriendlyCamp = 0x01,
    EnemyCamp = 0x02
};

struct Duration {
    Duration(float t = 0.0f): duration(t) {}
    float timeElapsed = 0.f;
    float duration;
};

struct GarbageCollection {
    vector<entt::registry::entity_type> entities;
};

struct CallNextFrameCtx {
    vector<function<void(entt::registry &)>> functions;
};

struct NamedEntities {
    map<string, entt::registry::entity_type> entities;
};

struct DisplayName {
    string value;
};

struct Clock {
    float time = 0.0f;
    double timeFromStart = 0.0f;

    void elapse(float dt) {
        time += dt;
        while (time >= 24.0) {
            time -= 24;
        }
        timeFromStart += dt;
    }
};

struct RenderTarget {
    RenderTarget(sf::RenderWindow* win):
        window(win),
        worldView(sf::FloatRect(0.f, 0.f, WORLD_VIEW_WIDTH, WORLD_VIEW_HEIGHT)) {
        worldRenderTarget.create(WINDOW_WIDTH, WINDOW_HEIGHT);
    }
    sf::RenderWindow* window;
    sf::RenderTexture worldRenderTarget;
    sf::View worldView;
    sf::Shader worldShader;
};

struct TextureFrame {
    TextureFrame(sf::IntRect& rect, float duration):rect(rect), duration(duration) {}
    sf::IntRect rect;
    float duration;
};

struct TextureFrameSet {
    sf::Texture* texture;
    vector<TextureFrame> frames;
};

const TextureFrameSet EmptyTextureFrameSet{nullptr};

struct AnimationState {
    AnimationState(const string &spriteName, const TextureFrameSet* frameSet = nullptr):
        spriteName(spriteName), frameSet(frameSet) {}
    string spriteName;
    float timeElapsed = 0.f;
    int frameIndex = 0;
    bool loop = true;
    const TextureFrameSet* frameSet = nullptr;
    const TextureFrame* frame = nullptr;
};

struct Sprite: public sf::Sprite {
public:
    Sprite(): sf::Sprite() {}
    int getZ() const {
        return m_z;
    }
    void setZ(int z) {
        m_z = z;
    }
    void setVisible(bool visible) {
        m_visible = visible;
    }
    bool visible() {
        return m_visible;
    }
private:
    int m_z = 0;
    bool m_visible = true;
};

struct Text: public sf::Text {
public:
    Text(): sf::Text() {}
    int getZ() const {
        return m_z;
    }
    void setZ(int z) {
        m_z = z;
    }
    void setVisible(bool visible) {
        m_visible = visible;
    }
    bool visible() {
        return m_visible;
    }
private:
    int m_z = 0;
    bool m_visible = true;
};

struct Destroyable {
    Destroyable(int maxHP): hp(maxHP), maxHP(maxHP) {}
    int hp;
    int maxHP;
};

#define KEY(keyname)\
bool keyname = false;\
bool _##keyname##Pressed = false;\
bool keyname##Pressed() {\
    if (_##keyname##Pressed) {\
        _##keyname##Pressed = false;\
        return true;\
    }\
    return false;\
}

struct InputState {
    KEY(up)
    KEY(down)
    KEY(left)
    KEY(right)
    KEY(btn1)
    KEY(btn2)
    KEY(btnOK)
    KEY(btnCancel)
    KEY(btnPack)

    bool textMode = false;
    string text;
};
#undef KEY

enum GameState {
    NormalState = 0,
    DialogueState,
    PackState,
};

struct CollisionCallback {
    void (*beginContact)(entt::registry::entity_type ent1, entt::registry::entity_type ent2, entt::registry &registry) = nullptr;
    void (*endContact)(entt::registry::entity_type ent1, entt::registry::entity_type ent2, entt::registry &registry) = nullptr;
};

typedef function<void(entt::registry::entity_type, entt::registry::entity_type, entt::registry&)> InteractCallback;

struct Interaction {
    entt::registry::entity_type mark = entt::null;
    InteractCallback interact = nullptr;
    list<entt::registry::entity_type> objects;
};

struct Item {
    string name;
    string displayName;
};

struct ItemDefine {
    string name;
    string displayName;
};

struct CharacterDefine {
    string typeName;
    string displayName;
    string spriteName;
    int hp;
    int dialogueId;
};

struct Pack {
    map<string, int> items;

    void addItem(const string &name, unsigned int count) {
        auto i = items.find(name);
        if (i != items.end()) {
            i->second += count;
        } else {
            items[name] = count;
        }
    }

    bool removeItem(const string &name, unsigned int count) {
        auto i = items.find(name);
        if (i != items.end() && i->second >= count) {
            i->second -= count;
            return true;
        } else {
            return false;
        }
    }
};

struct Light {
    sf::Color color;
    float intensity;
};

struct Choice {
    string text;
    string condition;
    int nextDialogueId = -1;
};

struct Dialogue {
    string text;
    string action;
    vector<Choice> choices;
};

typedef function<void(float)> TweenSetter;

struct Tween {
    float startValue;
    float endValue;
    float duration;
    TweenSetter setter = nullptr;

    float timeElapsed = 0.0f;
    bool loop = true;
    bool physicsTime = false;
    bool reverse = false;

    void reset() {
        if (reverse) {
            float tmp = endValue;
            endValue = startValue;
            startValue = tmp;
        }

        reverse = false;
        timeElapsed = 0.0f;
    }
};

#endif //ZELDA_COMPONENTS_H
