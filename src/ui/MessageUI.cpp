#include "MessageUI.h"
#include "style.h"
#include "utils.h"

const float MSG_HEIGHT = 0.04;
const float MSG_WIDTH = 0.15;

MessageUI::MessageUI(sf::Font &font, sf::Vector2f size, entt::registry &registry) {
    m_windowSize = size;

    m_background.setCornerPointCount(16);
    m_background.setCornersRadius(size.y * MSG_HEIGHT / 2.0f);
    m_background.setFillColor(BG_COLOR);
    m_background.setOutlineThickness(4.0f);
    m_background.setOutlineColor(sf::Color::Transparent);
    m_background.setSize(sf::Vector2f(size.x * MSG_WIDTH, size.y * MSG_HEIGHT));
    m_background.setPosition(size.x * (1.0f - MSG_WIDTH) - 10, 100);

    m_text.setFont(font);
    m_text.setCharacterSize(CHOICE_FONT_SIZE);
    m_text.setFillColor(sf::Color::White);
    m_text.setStyle(sf::Text::Italic);
}
void MessageUI::setMessage(const string &message, const TextureFrameSet &f) {
    m_text.setString(wstring_converter.from_bytes(message));
    const auto &textBounds = m_text.getLocalBounds();
    m_position = {m_windowSize.x * (1.0f - MSG_WIDTH) - 10, 100};
    sf::Vector2f p;
    p.x = m_windowSize.x * MSG_WIDTH - textBounds.width - m_windowSize.y * MSG_HEIGHT / 2.0f;
    p.y = 1;
    m_text.setPosition(m_windowSize.x + p.x, m_position.y + p.y);
    m_timeRemaining = 5.0f;
    m_background.setPosition(m_windowSize.x, m_position.y);

    if (f.texture) {
        m_icon.setTextureRect(f.frames[0].rect);
        m_icon.setTexture(*f.texture);
        m_icon.setPosition(m_windowSize.x, m_position.y - 10);
        m_icon.setScale(2.5f, 2.5f);
        m_icon.setColor(sf::Color::White);
        m_showIcon = true;
    } else {
        m_showIcon = false;
    }

    m_text.setFillColor(sf::Color::White);
    m_background.setFillColor(BG_COLOR);

}
void MessageUI::update(float dt) {
    m_timeRemaining -= dt;
    if (m_timeRemaining <= 0) {
        m_timeRemaining = 0.0f;
    }
    const auto &p = m_background.getPosition();
    sf::Vector2f s;
    if (m_position.x != p.x) {
        float dx = m_position.x - p.x;
        float v = dx > 0 ? dt * 600.0f : dt * -600.0f;
        if (abs(dx) < abs(v)) {
            s.x = dx;
        } else {
            s.x = v;
        }
    }
    if (m_position.y != p.y) {
        float dy = m_position.y - p.y;
        float v = dy > 0 ? dt * 200.0f : dt * -200.0f;
        if (abs(dy) < abs(v)) {
            s.y = dy;
        } else {
            s.y = v;
        }
    }
    m_background.setPosition(p + s);
    m_text.setPosition(m_text.getPosition() + s);
    m_icon.setPosition(m_icon.getPosition() + s);
    auto color = m_background.getFillColor();
    if (m_timeRemaining < 1.0f) {
        color.a = (sf::Uint8)(m_timeRemaining * 180);
        sf::Uint8 c =  (sf::Uint8)(m_timeRemaining * 255);
        m_icon.setColor({255, 255, 255, c});
        auto tcolor = m_text.getFillColor();
        tcolor.a = c;
        m_text.setFillColor(tcolor);
    }
    m_background.setFillColor(color);
}
void MessageUI::draw(sf::RenderWindow* win) {
    win->draw(m_background);
    win->draw(m_text);
    if (m_showIcon) {
        win->draw(m_icon);
    }
}
void MessageUI::moveDown() {
    m_position.y += MSG_HEIGHT * m_windowSize.y + 15;
}
