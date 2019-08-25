#include "PackUI.h"
#include "components.h"
#include "style.h"
#include "resource.h"

const int PACK_COLL_COUNT = 8;
const int PACK_ROW_COUNT = 5;
const int PACK_TOP = 200;
const int ITEM_GAP = 8;
const int ITEM_ICON_SCALE = 5;
const int ITEM_WIDTH = ITEM_ICON_SCALE * 16;
const int BG_WIDTH = ITEM_WIDTH + 8;
const int ITEM_COUNT_TEXT_SIZE = 24;


PackItemUI::PackItemUI(sf::Font &font, float x, float y) {
    m_background.setPosition(x, y);
    m_background.setSize({BG_WIDTH, BG_WIDTH});
    m_background.setFillColor(BG_COLOR);
    m_icon.setScale(ITEM_ICON_SCALE, ITEM_ICON_SCALE);
    m_icon.setPosition(x + (BG_WIDTH - ITEM_WIDTH) / 2, y + (BG_WIDTH - ITEM_WIDTH) / 2);
    m_number.setFont(font);
    m_number.setCharacterSize(ITEM_COUNT_TEXT_SIZE);
    m_number.setFillColor(sf::Color::White);
    m_number.setStyle(sf::Text::Italic | sf::Text::Bold);
}
void PackItemUI::setItem(const string& name, int count, entt::registry& registry) {
    m_active = true;
    auto &f = getFrameSet(name, registry);
    m_icon.setTexture(*f.texture);
    m_icon.setTextureRect(f.frames[0].rect);
    if (count > 1) {
        m_number.setString(to_string(count));
        auto bounds = m_number.getGlobalBounds();
        auto p = m_background.getPosition() + m_background.getSize();
        m_number.setPosition(p.x - bounds.width - 4, p.y - bounds.height - 8);
    } else {
        m_number.setString("");
    }
}
void PackItemUI::clear(){
    m_active = false;
}
void PackItemUI::draw(sf::RenderWindow* win) {
    win->draw(m_background);
    if (m_active) {
        win->draw(m_icon);
        win->draw(m_number);
    }
}

PackUI::PackUI(sf::Font &font, const sf::Vector2f &size, entt::registry& registry) {
    for (int j = 0; j < PACK_ROW_COUNT; ++j) {
        for (int i = 0; i < PACK_COLL_COUNT; ++i) {
            m_items.emplace_back(font,
                size.x / 2 + (i - PACK_COLL_COUNT / 2) * (ITEM_GAP + BG_WIDTH) ,
                                 PACK_TOP + j * (ITEM_GAP + BG_WIDTH));
        }
    }
    m_background.setPosition(0, 0);
    m_background.setSize(size);
    m_background.setFillColor(sf::Color(0, 0, 0, 80));
    m_title.setString("Pack");
    m_title.setFont(font);
    m_title.setCharacterSize(30);
    m_title.setFillColor(sf::Color::White);
    m_title.setStyle(sf::Text::Italic | sf::Text::Bold);
    m_title.setPosition(size.x / 2 - m_title.getLocalBounds().width / 2, 100);
}
void PackUI::activate(entt::registry &registry) {
    registry.view<entt::tag<"hero"_hs>, Pack>().each([&registry, this](auto ent, auto _, Pack &pack){
        int i = 0;
        for (auto it : pack.items) {
            if (it.second > 0) {
                m_items[i++].setItem(it.first, it.second, registry);
            }
        }
    });
}
void PackUI::deactivate(entt::registry &registry) {
    for (auto &i : m_items) {
        i.clear();
    }
}
void PackUI::update(float dt, entt::registry &registry) {
    InputState& input = registry.ctx<InputState>();
    if (input.btnPackPressed()) {
        auto &gameState = registry.ctx<GameState>();
        gameState = NormalState;
    }
}
void PackUI::draw(sf::RenderWindow* win, entt::registry &registry) {
    win->draw(m_background);
    win->draw(m_title);
    for (auto &i : m_items) {
        i.draw(win);
    }
}
