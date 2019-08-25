#ifndef ZELDA_PACKUI_H
#define ZELDA_PACKUI_H

#include "UIState.h"
#include "components.h"

class PackItemUI {
public:
    PackItemUI(sf::Font &font, float x, float y);
    void setItem(const string& name, int count, entt::registry& registry);
    void clear();
    void draw(sf::RenderWindow* win);
private:
    bool m_active = false;
    sf::RectangleShape m_background;
    sf::Sprite m_icon;
    sf::Text m_number;
};

class PackUI: public UIState {
public:
    PackUI(sf::Font &font, const sf::Vector2f &size, entt::registry& registry);
    void activate(entt::registry &registry) override;
    void deactivate(entt::registry &registry) override;
    void update(float dt, entt::registry &registry) override;
    void draw(sf::RenderWindow* win, entt::registry &registry) override;
private:
    vector<PackItemUI> m_items;
    sf::RectangleShape m_background;
    sf::Text m_title;
};


#endif //ZELDA_PACKUI_H
