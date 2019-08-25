#ifndef ZELDA_MESSAGEUI_H
#define ZELDA_MESSAGEUI_H

#include "common.h"
#include "misc/SFMLRoundedRectangle.h"
#include "components.h"

class MessageUI {
public:
    MessageUI(sf::Font &font, sf::Vector2f size, entt::registry &registry);
    void setMessage(const string &message, const TextureFrameSet &f);
    void update(float dt);
    void draw(sf::RenderWindow* win);
    void moveDown();
    inline float getTimeRemaining() const {
        return m_timeRemaining;
    }
private:
    sf::Vector2f m_position;
    sf::Vector2f m_windowSize;
    sf::Text m_text;
    sf::Sprite m_icon;
    sf::RoundedRectangleShape m_background;
    float m_timeRemaining = 0.0f;
    bool m_showIcon;
};

#endif //ZELDA_MESSAGEUI_H
