#include "input.h"
#include "components.h"
#include "utils.h"
#include "scripts.h"
#include "ui/ui.h"
#include <string>

map<string, function<void(vector<string>&, entt::registry&)> > Commands = {
    {"exit", [](vector<string>& args, entt::registry& registry){exit(0);}},
    {"echo", [](vector<string>& args, entt::registry& registry){
        for (const string& arg : args) {
            printf("%s ", arg.c_str());
        }
        printf("\n");
    }},
    {"debug", [](vector<string>& args, entt::registry& registry){
        if (args.size() != 2) return;
        if (args[1] == "off") {
            entt::monostate<entt::hashed_string{"debug"}>{} = false;
        } else if (args[1] == "on") {
            entt::monostate<entt::hashed_string{"debug"}>{} = true;
        }
    }},
    {"shader", [](vector<string>& args, entt::registry& registry){
        auto &rt = registry.ctx<RenderTarget>();
        if (!rt.worldShader.loadFromFile("assets/light.vert", "assets/light.frag")) {
            printf("Failed to load shader\n");
        }
    }}
};

void updateInputState(entt::registry &registry) {
    auto &win = registry.ctx<RenderTarget>().window;
    sf::Event event;
    InputState& input = registry.ctx<InputState>();

    input._upPressed = false;
    input._downPressed = false;
    input._rightPressed = false;
    input._leftPressed = false;
    input._btn1Pressed = false;
    input._btnOKPressed = false;

    while (win->pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            win->close();

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Enter) {
            if (input.textMode) {
                istrstream iss(input.text.c_str());
                vector<string> args((istream_iterator<string>(iss)),
                                                 istream_iterator<string>());
                if (args.empty()) {
                    input.text = "";
                    input.textMode = !input.textMode;
                    return;
                }
                auto icmd = Commands.find(args[0]);
                if (icmd != Commands.end()) {
                    printf("Command: [%s]\n", input.text.c_str());
                    icmd->second(args, registry);
                } else {
                    try {
                        string r = eval<string>(input.text, registry);
                        showMessage(string("~") + r, "", registry);
                    } catch (const runtime_error & e) {
                        showMessage(e.what(), "", registry);
                    }
                }
            }
            input.text = "";
            input.textMode = !input.textMode;
        }

        if (input.textMode) {
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode < 128) {
                    char c = static_cast<char>(event.text.unicode);
                    if (c == '\b') {
                        input.text.pop_back();
                    } else if (c == '\n') {
                        continue;
                    } else {
                        input.text += c;
                    }
                }
            }
        } else {
#define KEY_BIND(keycode, keyname)\
case sf::Keyboard::Key::keycode:\
    input._##keyname##Pressed = (event.type == sf::Event::KeyPressed) ? (!input.keyname) : false;\
    input.keyname = (event.type == sf::Event::KeyPressed);\
    break;

            if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
                switch (event.key.code) {
                    KEY_BIND(W, up)
                    KEY_BIND(S, down)
                    KEY_BIND(A, left)
                    KEY_BIND(D, right)
                    KEY_BIND(J, btn1)
                    KEY_BIND(K, btnOK)
                    KEY_BIND(Q, btnPack)
                    default:
                        break;
                }
            }
        }
    }
}
#undef KEY_BIND
