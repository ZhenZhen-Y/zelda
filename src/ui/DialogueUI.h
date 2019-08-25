#ifndef ZELDA_DIALOGUEUI_H
#define ZELDA_DIALOGUEUI_H

#include "common.h"
#include "components.h"
#include "misc/SFMLRoundedRectangle.h"
#include "UIState.h"

class DialogueChoiceUI {
public:
    DialogueChoiceUI(sf::Font &font, sf::Vector2f size, float x, float y, entt::registry& registry);
    void initialize(entt::registry& registry);
    void loadChoice(const Choice & choice);
    void highlight(bool v);
    void update(float dt);
    void draw(sf::RenderWindow* win);
    int getNextDialogueId() {return m_nextDialogueId;}
private:
    sf::Text m_text;
    sf::Text m_cursor;
    sf::RoundedRectangleShape m_background;
    bool m_desc = true;
    bool m_highlight;
    sf::Vector2f m_windowSize;
    float m_x, m_y;
    int m_nextDialogueId;
};

class DialogueStateUI: public UIState {
public:
    DialogueStateUI(sf::Font &font, const sf::Vector2f &size, entt::registry& registry);
    void initialize(entt::registry& registry);
    void loadDialogue(const Dialogue &dialogue, entt::registry& registry);
    void activate(entt::registry &registry) override;
    void update(float dt, entt::registry &registry) override;
    void deactivate(entt::registry &registry) override;
    void draw(sf::RenderWindow* win, entt::registry &registry) override;
private:
    sf::Text m_text;
    sf::Text m_nameText;
    sf::RoundedRectangleShape m_background;
    vector<DialogueChoiceUI> m_choices;
    int m_activeChoiceCount = 0;
    int m_selectedChoice = 0;
    sf::Vector2f m_windowSize;
    const Dialogue* m_dialogue = nullptr;
};

#endif //ZELDA_DIALOGUEUI_H
