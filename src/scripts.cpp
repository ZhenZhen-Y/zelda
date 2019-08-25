#include "scripts.h"
#include "components.h"
#include "ui/ui.h"
#include "resource.h"
#include "utils.h"
#include "tilemap.h"
#include "graphic.h"
#define BOOST_NO_AUTO_PTR
#include <boost/filesystem.hpp>

void initializeLuaState(entt::registry &registry) {
    sel::State &state = registry.set<sel::State>();
    state["countItem"] = function<int(const string)>([&registry] (const string itemName) {
        int count = 0;
        registry.view<entt::tag<"hero"_hs>, Pack>().each([&registry, &count, &itemName](auto ent, auto _, Pack &pack){
            auto i = pack.items.find(itemName);
            if (i != pack.items.end()) {
                count = i->second;
            }
        });
        return count;
    });
    state["removeItem"] = function<void(const string, unsigned int)>([&registry] (const string itemName, unsigned int count) {
        registry.view<entt::tag<"hero"_hs>, Pack>().each([&registry, &itemName, count](auto ent, auto _, Pack &pack){
            if (pack.removeItem(itemName, count)) {
                auto &rs = registry.ctx<ResourceManager>();
                showMessage(rs.items[itemName].displayName + " -" + to_string(count), itemName, registry);
            }
        });
    });
    state["addItem"] = function<void(const string, const string, unsigned int)>([&registry] (const string entityName, const string itemName, unsigned int count) {
        auto ent = getEntityByName(entityName, registry);
        auto &item = getItemDefine(itemName, registry);
        auto &pack = registry.get<Pack>(ent);
        pack.addItem(itemName, count);
    });
    state["loadSpriteSheet"] = function<void(const string)>([&registry, &state] (const string filename) {
        boost::filesystem::path p(state["filename"]);
        p = p.parent_path() / filename;
        loadSpriteSheet(p.string(), registry);
    });
    state["loadCharacterDefines"] = function<void(const string)>([&registry, &state] (const string filename) {
        boost::filesystem::path p(state["filename"]);
        p = p.parent_path() / filename;
        loadCharacterDefines(p.string(), registry);
    });
    state["loadItemDefines"] = function<void(const string)>([&registry, &state] (const string filename) {
        boost::filesystem::path p(state["filename"]);
        p = p.parent_path() / filename;
        loadItemDefines(p.string(), registry);
    });
    state["loadDialogues"] = function<void(const string)>([&registry, &state] (const string filename) {
        boost::filesystem::path p(state["filename"]);
        p = p.parent_path() / filename;
        loadDialogues(p.string(), registry);
    });
    state["loadMap"] = function<void(const string)>([&registry, &state] (const string filename) {
        boost::filesystem::path p(state["filename"]);
        p = p.parent_path() / filename;
        loadMap(p.string(), registry);
    });
    state["loadEnvironmentLightSheet"] = function<void(const string)>([&registry, &state] (const string filename) {
        boost::filesystem::path p(state["filename"]);
        p = p.parent_path() / filename;
        loadEnvironmentLightSheet(p.string(), registry);
    });

    state["elapse"] = function<void(double)>([&registry](double t){
        auto &clock = registry.ctx<Clock>();
        clock.elapse(t);
    });

    state["createItem"] = function<void(const string, double x, double y)>([&registry](const string name, double x, double y) {
        createItem(name, x, y, registry);
    });
    state["createCharacter"] = function<void(const string, const string, double x, double y, int camp)>([&registry] (
        const string typeName, const string entityName, double x, double y, int camp) {
        createCharacter(typeName, entityName, x, y, (Camp) camp, registry);
    });
}

