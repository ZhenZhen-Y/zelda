#include "ui.h"
#include "components.h"
#include "misc/SFMLRoundedRectangle.h"
#include "utils.h"
#include "resource.h"
#include "UIState.h"
#include "style.h"
#include "MessageUI.h"
#include "DialogueUI.h"
#include "PackUI.h"

class NormalStateUI: public UIState {
    void activate(entt::registry &registry) override {
    }
    void update(float dt, entt::registry &registry) override {
        InputState& input = registry.ctx<InputState>();
        if (input.btnPackPressed()) {
            auto &gameState = registry.ctx<GameState>();
            gameState = PackState;
        }
    }
    void deactivate(entt::registry &registry) override {
    }
    void draw(sf::RenderWindow* win, entt::registry &registry) override {
    }
};

struct UI {
    sf::Text debugText;
    vector<UIState*> uiStates;
    GameState state;
    list<MessageUI> visibleMessages;
    list<MessageUI> messages;
    sf::Text clock;
};

void initializeUI(entt::registry &registry) {
    auto &ui = registry.set<UI>();
    auto &font = registry.ctx<sf::Font>();
    auto &rt = registry.ctx<RenderTarget>();

    ui.debugText.setFont(font);
    ui.debugText.setCharacterSize(20);
    ui.debugText.setFillColor(sf::Color::White);
    ui.debugText.setStyle(sf::Text::Bold);
    ui.debugText.setPosition(2, WINDOW_HEIGHT - 24);
    ui.debugText.setOutlineThickness(1.0f);
    ui.debugText.setOutlineColor(sf::Color::Black);

    ui.uiStates.push_back(new NormalStateUI());
    auto* s = new DialogueStateUI(font, {WINDOW_WIDTH, WINDOW_HEIGHT}, registry);
    s->initialize(registry);
    ui.uiStates.push_back(s);
    ui.uiStates.push_back(new PackUI(font, {WINDOW_WIDTH, WINDOW_HEIGHT}, registry));

    for (int i = 0; i < 5; ++i) {
        ui.messages.emplace_back(font, sf::Vector2f{WINDOW_WIDTH, WINDOW_HEIGHT}, registry);
    }

    ui.clock.setFont(font);
    ui.clock.setCharacterSize(TEXT_FONT_SIZE);
    ui.clock.setFillColor(sf::Color::White);
    ui.clock.setStyle(sf::Text::Italic);
    ui.clock.setString("00:00");
    ui.clock.setPosition(WINDOW_WIDTH - ui.clock.getLocalBounds().width, 0);
}

void updateDebugInfo(float dt, entt::registry &registry) {
    auto &rt = registry.ctx<RenderTarget>();
    auto &win = rt.window;
    auto winSize= win->getSize();
    auto &ui = registry.ctx<UI>();
    char buffer[128];
    InputState& input = registry.ctx<InputState>();
    sf::String inputs;
    if (input.textMode) {
        inputs = wstring_converter.from_bytes(">" + input.text);
    } else {
        if (input.btn1) inputs += L"①";
        if (input.btnOK) inputs += L"(OK)";
        if (input.down) inputs += L"↓";
        if (input.up) inputs += L"↑";
        if (input.left) inputs += L"←";
        if (input.right) inputs += L"→";
    }
    snprintf(buffer, sizeof(buffer)-1, "fps=%02.1f, window=%dx%d", 1.0/dt, winSize.x, winSize.y);
    ui.debugText.setString(sf::String(buffer) + ", [" + inputs + "]");
    rt.window->draw(ui.debugText);
}

void updateUI(float dt, entt::registry &registry) {
    auto &gameState = registry.ctx<GameState>();
    auto &ui = registry.ctx<UI>();
    if (ui.state != gameState) {
        ui.uiStates[ui.state]->deactivate(registry);
        UIState* newState = ui.uiStates[gameState];
        newState->activate(registry);
        ui.state = gameState;
    }

    ui.uiStates[gameState]->update(dt, registry);

    auto *window = registry.ctx<RenderTarget>().window;
    ui.uiStates[gameState]->draw(window, registry);

    updateDebugInfo(dt, registry);

    auto i = ui.visibleMessages.begin();
    while (i != ui.visibleMessages.end()) {
        i->update(dt);
        i->draw(window);

        if (i->getTimeRemaining() == 0.0f) {
            ui.messages.push_back(*i);
            ui.visibleMessages.erase(i++);
        } else {
            ++i;
        }
    }

    float clockTime = registry.ctx<Clock>().time;
    int hr = (int)floorf(clockTime);
    int minute = (int)(60.0 * (clockTime - hr));
    char clockTimeStr[6];
    sprintf(clockTimeStr, "%02d:%02d", hr, minute);
    ui.clock.setString(clockTimeStr);
    window->draw(ui.clock);
}

void showMessage(const string &message, const string &icon, entt::registry &registry) {
    auto &ui = registry.ctx<UI>();
    if (ui.messages.empty()) {
        auto &font = registry.ctx<sf::Font>();
        auto &rt = registry.ctx<RenderTarget>();
        ui.messages.emplace_back(font, sf::Vector2f{WINDOW_WIDTH, WINDOW_HEIGHT}, registry);
    }

    for (auto &m : ui.visibleMessages) {
        m.moveDown();
    }
    auto &msg = ui.messages.front();
    if (icon.empty()) {
        msg.setMessage(message, EmptyTextureFrameSet);
    } else {
        msg.setMessage(message, getFrameSet(icon, registry));
    }

    ui.visibleMessages.push_front(msg);
    ui.messages.pop_front();
}
