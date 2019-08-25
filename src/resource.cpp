#include "resource.h"
#include <csv/csv.h>
#include "utils.h"
#define BOOST_NO_AUTO_PTR
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <regex>

namespace pt = boost::property_tree;

void dumpSpriteSheets(entt::registry &registry) {
    auto &rs = registry.ctx<ResourceManager>();
    printf("Dump resources:\n");
    for (auto &i : rs.textures) {
        auto sz = i.second.getSize();
        printf("- Texture %s: %d*%d %p\n", i.first.c_str(), sz.x, sz.y, &i.second);
    }

    for (auto &i : rs.frameset) {
        auto frameCount = i.second.frames.size();
        auto rect = i.second.frames[0].rect;
        printf("- Frameset %s: %ld, rect=(%d,%d,%d,%d) texture=%p\n", i.first.c_str(), frameCount, rect.left, rect.top, rect.width, rect.height, i.second.texture);
    }
}

void loadTexture(const string &filename, entt::registry &registry) {
    printf("load texture: \"%s\"\n", filename.c_str());
    auto &res = registry.ctx<ResourceManager>();
    boost::filesystem::path p(filename);
    string key = p.string();
    auto ret = res.textures.emplace(std::piecewise_construct,
                                    std::forward_as_tuple(key),
                                    std::forward_as_tuple());
    sf::Texture &t = ret.first->second;
    if (!t.loadFromFile(filename)) {
        throw runtime_error("Failed to load texture: " + filename);
    }
}

void loadSpriteSheet(const string &filename, entt::registry &registry) {
    printf("load sprite sheet: \"%s\"\n", filename.c_str());
    ifstream spriteSheetFile;
    spriteSheetFile.open(filename);
    if (!spriteSheetFile.is_open()) {
        throw runtime_error("Failed to load sprite sheet: " + filename);
    }

    auto &rs = registry.ctx<ResourceManager>();
    for(CSVIterator irow(spriteSheetFile); irow != CSVIterator(); ++irow) {
        auto row = *irow;
        string name = row[0];
        string textureName = row[1];
        float duration = atof(row[6].c_str());
        auto &frameset = rs.frameset[name];
        sf::IntRect rect(atoi(row[2].c_str()),
                          atoi(row[3].c_str()),
                          atoi(row[4].c_str()),
                          atoi(row[5].c_str()));

        frameset.frames.emplace_back(rect, duration);
        boost::filesystem::path p(filename);
        p.remove_filename();
        p.append(textureName);
        auto t = rs.textures.find(p.string());
        if (t == rs.textures.end()) {
            loadTexture(p.string(), registry);
            t = rs.textures.find(p.string());
            if (t == rs.textures.end()) {
                throw runtime_error("Unknown texture key: " + textureName);
            }
        }

        frameset.texture = &(t->second);
    }
}

void addFrame(const string &key,
              sf::IntRect rect,
              const string& textureName,
              float duration,
              entt::registry &registry) {
    auto &rs = registry.ctx<ResourceManager>();

    auto &frameset = rs.frameset[key];
    frameset.frames.emplace_back(rect, duration);

    auto t = rs.textures.find(textureName);
    if (t == rs.textures.end()) {
        loadTexture(textureName, registry);
        t = rs.textures.find(textureName);
    }

    frameset.texture = &(t->second);
}

const TextureFrameSet& getFrameSet(const string &key,
                                     entt::registry &registry) {
    auto &rs = registry.ctx<ResourceManager>();
    auto f = rs.frameset.find(key);
    if (f == rs.frameset.end()) {
        throw runtime_error("Unknown frame set key: " + key);
    }

    return f->second;
}

sf::Texture& getTexture(const string& filename, entt::registry &registry) {
    auto &rs = registry.ctx<ResourceManager>();
    boost::filesystem::path p(filename);
    const string &key = p.string();
    auto t = rs.textures.find(key);
    if (t == rs.textures.end()) {
        loadTexture(key, registry);
        t = rs.textures.find(key);
        if (t == rs.textures.end()) {
            throw runtime_error("Unknown texture key: " + key);
        }
    }

    return t->second;
}

const TextureFrameSet& getAnimationFrameSet(
    const string &spriteName,
    const string &actionName,
    Direction direction,
    entt::registry &registry) {
    const string directionMap[] = {"down", "left", "up", "right"};
    auto frameArrayName = spriteName + "." + actionName + "." + directionMap[direction];
    return getFrameSet(frameArrayName, registry);
}

void loadItemDefines(const string &filename, entt::registry &registry) {
    pt::ptree doc;
    pt::read_xml(filename, doc);
    auto &rs = registry.ctx<ResourceManager>();

    for (auto &item : doc.get_child("items")) {
        if (item.first == "item") {
            string name = item.second.get<string>("<xmlattr>.name");
            string displayName = item.second.get<string>("displayName");
            rs.items[name] = {name, displayName};
        }
    }
}

const ItemDefine &getItemDefine(const string& name, entt::registry &registry) {
    auto &rs = registry.ctx<ResourceManager>();
    auto i = rs.items.find(name);
    if (i != rs.items.end()) {
        return i->second;
    } else {
        throw runtime_error("Item not found");
    }
}

void removeSpaceInText(string &str) {
    trim(str);
    const regex PATTERN1(R"([ \f\r\t\v]+)");
    const regex PATTERN2(R"(\n )");
    str = std::regex_replace(str, PATTERN1, " ");
    str = std::regex_replace(str, PATTERN2, "\n");
}

void loadDialogues(const string &filename, entt::registry &registry) {
    pt::ptree doc;
    pt::read_xml(filename, doc);
    auto &dialogues = registry.ctx<ResourceManager>().dialogues;
    int baseDialogueId = 0;

    for (auto &item : doc.get_child("story")) {
        if (item.first == "dialogue") {
            auto &d = item.second;
            int id = d.get<int>("<xmlattr>.id");
            Dialogue &dialogue = dialogues.emplace(std::piecewise_construct,
                                                      std::forward_as_tuple(id + baseDialogueId),
                                                      std::forward_as_tuple()).first->second;

            for (auto &itemOfDialogue : d.get_child("")) {
                if (itemOfDialogue.first == "text") {
                    dialogue.text = itemOfDialogue.second.get<string>("");
                    removeSpaceInText(dialogue.text);
                } else if (itemOfDialogue.first == "choice") {
                    auto &c = itemOfDialogue.second;
                    Choice& choice = dialogue.choices.emplace_back();
                    choice.text = c.get<string>("");
                    choice.condition = c.get<string>("<xmlattr>.condition", "true");
                    trim(choice.text);
                    choice.nextDialogueId = c.get<int>("<xmlattr>.nextDialogueId") + baseDialogueId;
                } else if (itemOfDialogue.first == "action") {
                    dialogue.action = itemOfDialogue.second.get<string>("");
                    removeSpaceInText(dialogue.action);
                }
            }
        }
    }
}

const Dialogue& getDialogue(int id, entt::registry &registry) {
    auto &dialogues = registry.ctx<ResourceManager>().dialogues;
    auto p = dialogues.find(id);
    if (p == dialogues.end()) {
        throw runtime_error(string("Unknown dialogue: id=") + to_string(id));
    }
    return p->second;
}

void loadCharacterDefines(const string &filename, entt::registry &registry) {
    pt::ptree doc;
    pt::read_xml(filename, doc);

    auto &npcs = registry.ctx<ResourceManager>().npcs;

    for (auto &item : doc.get_child("npcs")) {
        auto &npcData = item.second;
        string typeName = npcData.get<string>("<xmlattr>.typeName", "");
        auto &npc = npcs[typeName];
        npc.typeName = typeName;
        npc.spriteName = npcData.get<string>("<xmlattr>.spriteName", "");
        npc.displayName = npcData.get<string>("<xmlattr>.displayName", "");
        npc.hp = npcData.get<int>("destroyable.<xmlattr>.hp", -1);
        npc.dialogueId = npcData.get<int>("dialogue.<xmlattr>.id", -1);
    }
}

const CharacterDefine &getCharacterDefine(const string &typeName, entt::registry &registry) {
    auto &npcs = registry.ctx<ResourceManager>().npcs;
    auto i = npcs.find(typeName);
    if (i == npcs.end()) {
        throw runtime_error("Unknown npc type '" + typeName + "'");
    }
    return i->second;
}