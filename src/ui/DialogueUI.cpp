#include "DialogueUI.h"
#include "utils.h"
#include "style.h"
#include "scripts.h"
#include "resource.h"

const float BG_WIDTH = 0.4;
const float BG_TOP = 0.7;
const float BG_HEIGHT = 0.2;
const float CHOICE_WIDTH = 0.25;
const float GAP_BETWEEN_BG_CHOICE = (1.0 - BG_WIDTH - 2.0f * CHOICE_WIDTH) / 4.0f;
const float CHOICE_HEIGHT = 0.06;
const float CHOICE_GAP = (BG_HEIGHT - 3 * CHOICE_HEIGHT) / 2.0f;
const float NAME_TOP_MARGIN = BG_HEIGHT * 0.05;
const float NAME_HEIGHT = BG_HEIGHT * 0.1;
const int TEXT_OFFSET_Y = -5;

DialogueChoiceUI::DialogueChoiceUI(sf::Font &font, sf::Vector2f size, float x, float y, entt::registry& registry) {
    m_x = x;
    m_y = y;
    m_windowSize = size;

    m_background.setCornerPointCount(16);
    m_background.setCornersRadius(m_windowSize.y * CHOICE_HEIGHT / 2.0f);
    m_background.setFillColor(BG_COLOR);
    m_background.setOutlineThickness(4.0f);
    m_background.setOutlineColor(sf::Color::Transparent);
    m_background.setSize(sf::Vector2f(m_windowSize.x * CHOICE_WIDTH, m_windowSize.y * CHOICE_HEIGHT));
    m_background.setPosition(x, y);

    m_text.setFont(font);
    m_text.setCharacterSize(CHOICE_FONT_SIZE);
    m_text.setFillColor(sf::Color::White);
    m_text.setStyle(sf::Text::Italic);

    m_cursor.setFont(font);
    m_cursor.setCharacterSize(CHOICE_FONT_SIZE);
    m_cursor.setFillColor(sf::Color::White);
    m_cursor.setString(L"▶");
    auto bounds = m_cursor.getLocalBounds();
    m_cursor.setPosition(x - m_cursor.getCharacterSize(), y + m_background.getSize().y / 2.0f - bounds.height / 2.0f + TEXT_OFFSET_Y);
}
void DialogueChoiceUI::initialize(entt::registry& registry) {
    createTweenEntity(100, 255, 2.0f, [this](float a) {
        if (!m_highlight) return;
        auto color = m_background.getOutlineColor();
        color.a = (sf::Uint8)a;
        m_background.setOutlineColor(color);
    }, registry);
    auto p = m_cursor.getPosition();
    createTweenEntity(p.x - 5, p.x + 5, 0.5f, [this](float v) {
        if (!m_highlight) return;
        auto p = m_cursor.getPosition();
        m_cursor.setPosition(v, p.y);
    }, registry);
}
void DialogueChoiceUI::loadChoice(const Choice & choice) {
    m_text.setString(wstring_converter.from_bytes(choice.text));
    auto bounds = m_text.getLocalBounds();
    m_text.setPosition(m_x + m_background.getCornersRadius(), m_y + m_background.getSize().y / 2.0f - bounds.height / 2.0f + TEXT_OFFSET_Y);
    m_nextDialogueId = choice.nextDialogueId;
    highlight(false);
}
void DialogueChoiceUI::highlight(bool v) {
    m_highlight = v;

    if (v) {
        m_background.setOutlineColor(sf::Color::White);
        m_desc = true;
    } else {
        m_background.setOutlineColor(sf::Color::Transparent);
    }
}
void DialogueChoiceUI::update(float dt) {
}
void DialogueChoiceUI::draw(sf::RenderWindow* win) {
    win->draw(m_background);
    win->draw(m_text);
    if (m_highlight) {
        win->draw(m_cursor);
    }
}

DialogueStateUI::DialogueStateUI(sf::Font &font, const sf::Vector2f &size, entt::registry& registry) {
    m_windowSize = size;
    m_background.setPosition(size.x / 2.0f, size.y * BG_TOP);
    m_background.setOrigin(size.x * BG_WIDTH / 2.0f, 0);
    m_background.setSize(sf::Vector2f(size.x * BG_WIDTH, size.y * BG_HEIGHT));
    m_background.setCornerPointCount(16);
    m_background.setCornersRadius(size.y * BG_HEIGHT / 2.0f);
    m_background.setFillColor(BG_COLOR);

    auto bgBounds = m_background.getGlobalBounds();

    m_nameText.setFont(font);
    m_nameText.setCharacterSize(NAME_FONT_SIZE);
    m_nameText.setFillColor(sf::Color::White);
    m_nameText.setStyle(sf::Text::Italic | sf::Text::Bold);
    m_nameText.setPosition(bgBounds.left + m_background.getCornersRadius(), size.y * (BG_TOP + NAME_TOP_MARGIN) );

    m_text.setFont(font);
    m_text.setCharacterSize(TEXT_FONT_SIZE);
    m_text.setFillColor(sf::Color::White);
    m_text.setStyle(sf::Text::Italic);

    for (int i = 0; i < 3; ++i) {
        auto &choiceUI = m_choices.emplace_back(font, size,
                                                m_windowSize.x * (0.5 + BG_WIDTH / 2 + GAP_BETWEEN_BG_CHOICE),
                                                size.y * BG_TOP + i * (CHOICE_HEIGHT + CHOICE_GAP) * m_windowSize.y,
                                                registry);
    }
}
void DialogueStateUI::initialize(entt::registry& registry) {
    for (int i = 0; i < 3; ++i) {
        m_choices[i].initialize(registry);
    }
}
void DialogueStateUI::loadDialogue(const Dialogue &dialogue, entt::registry& registry) {
    //m_activeChoiceCount = dialogue.choices.size();
    m_text.setString(wstring_converter.from_bytes(dialogue.text));

    sf::FloatRect bounds = m_text.getGlobalBounds();
    auto bgY = m_background.getPosition().y;
    auto bgSize = m_background.getSize();
    m_text.setPosition(m_windowSize.x/2.0f - bounds.width/2.0f,
                       bgY + (bgSize.y - NAME_HEIGHT * m_windowSize.y) / 2.0f - bounds.height /2.0f + NAME_HEIGHT * m_windowSize.y + TEXT_OFFSET_Y);

    m_activeChoiceCount = 0;
    for (int i = 0; i < dialogue.choices.size(); ++i) {
        if (eval<bool>(dialogue.choices[i].condition, registry)) {
            m_choices[m_activeChoiceCount++].loadChoice(dialogue.choices[i]);
        }
    }
    m_selectedChoice = 0;
    m_choices[m_selectedChoice].highlight(true);
    m_dialogue = &dialogue;
    eval(dialogue.action, registry);
}
void DialogueStateUI::activate(entt::registry &registry) {
    auto &interaction = registry.ctx<Interaction>();
    const Dialogue &dialogue = registry.get<Dialogue>(interaction.objects.front());
    DisplayName* nameComponent = registry.try_get<DisplayName>(interaction.objects.front());
    if (nameComponent) {
        m_nameText.setString(wstring_converter.from_bytes(nameComponent->value)); ;
    }
    loadDialogue(dialogue, registry);
}
void DialogueStateUI::update(float dt, entt::registry &registry) {
    InputState& input = registry.ctx<InputState>();
    GameState& gameState = registry.ctx<GameState>();
    if (input.btnOKPressed() && gameState == DialogueState) {
        if (m_dialogue->choices.empty()) {
            gameState = NormalState;
            return;
        }
        int nextDialogueId = m_choices[m_selectedChoice].getNextDialogueId();
        if (nextDialogueId < 0) {
            gameState = NormalState;
            return;
        } else {
            loadDialogue(getDialogue(nextDialogueId, registry), registry);
        }
    }

    if (m_activeChoiceCount > 0) {
        int oldSelectedChoice = m_selectedChoice;
        if (input.rightPressed() || input.downPressed()) {
            m_selectedChoice = (m_selectedChoice + 1) % m_activeChoiceCount;
        } else if (input.leftPressed() || input.upPressed()) {
            m_selectedChoice = (m_selectedChoice - 1 + m_activeChoiceCount) % m_activeChoiceCount;
        }

        if (oldSelectedChoice != m_selectedChoice) {
            m_choices[oldSelectedChoice].highlight(false);
            m_choices[m_selectedChoice].highlight(true);
        }

        for (int i = 0; i < m_activeChoiceCount; ++i) {
            m_choices[i].update(dt);
        }
    }
}
void DialogueStateUI::deactivate(entt::registry &registry) {

}
void DialogueStateUI::draw(sf::RenderWindow* win, entt::registry &registry) {
    win->draw(m_background);
    win->draw(m_nameText);
    win->draw(m_text);
    for (int i = 0; i < m_activeChoiceCount; ++i) {
        m_choices[i].draw(win);
    }
}

