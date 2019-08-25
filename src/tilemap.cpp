#include <set>
#include <string>
#include <map>

#define BOOST_NO_AUTO_PTR
#include <boost/filesystem.hpp>

#include "tilemap.h"
#include "components.h"
#include "utils.h"
#include "resource.h"

namespace pt = boost::property_tree;

void loadTileSet(const string &filename, set<int> &animationTiles, map<int, int> &collisionGroups, string &textureName, entt::registry &registry) {
    pt::ptree tsx;
    pt::read_xml(filename, tsx);
    int colls = tsx.get<int>("tileset.<xmlattr>.columns");
    int tileWidth = tsx.get<int>("tileset.<xmlattr>.tilewidth");
    int tileHeight = tsx.get<int>("tileset.<xmlattr>.tileheight");
    textureName = tsx.get<string>("tileset.image.<xmlattr>.source");
    string source;
    for (auto &tile : tsx.get_child("tileset")) {
        if (tile.first == "image") {
            source = tile.second.get<string>("<xmlattr>.source");
        } else if (tile.first == "tile") {
            string tileId = tile.second.get<string>("<xmlattr>.id");
            int tileIndex = atoi(tileId.c_str());
            for (auto &tileValue : tile.second) {
                if (tileValue.first == "animation") {
                    for (auto &frameTag : tileValue.second) {
                        int frameTileId = frameTag.second.get<int>("<xmlattr>.tileid");
                        int durationMs = frameTag.second.get<int>("<xmlattr>.duration");
                        sf::IntRect frame(frameTileId % colls * tileWidth,
                                          frameTileId / colls * tileHeight,
                                          tileWidth,
                                          tileHeight);
                        boost::filesystem::path p(filename);
                        p = p.parent_path() / source;
                        addFrame(string("tile-") + tileId, frame, p.string(), (float)durationMs / 1000.0, registry);
                    }
                    animationTiles.insert(tileIndex);
                } else if (tileValue.first == "properties") {
                    for (auto &property : tileValue.second) {
                        if (property.second.get<string>("<xmlattr>.name") == "collisionGroup") {
                            collisionGroups[tileIndex] = property.second.get<int>("<xmlattr>.value");
                        }
                    }
                }
            }
        }
    }
    tsx.clear();
}

void loadMap(const std::string& filename, entt::registry &registry) {
    pt::ptree tmx;

    pt::read_xml(filename, tmx);

    int width = tmx.get<int>("map.<xmlattr>.width");
    int height = tmx.get<int>("map.<xmlattr>.height");
    int tileWidth = tmx.get<int>("map.<xmlattr>.tilewidth");
    int tileHeight = tmx.get<int>("map.<xmlattr>.tileheight");

    auto &map = registry.set<Map>();
    string textureName;

    int iLayer = 0;
    pt::ptree layers = tmx.get_child("map");
    set<int> animationTiles;
    std::map<int, int> collisionGroups;
    boost::filesystem::path fpath(filename);
    for (auto &layer : layers) {
        if (layer.first == "tileset") {
            loadTileSet((fpath.parent_path() / layer.second.get<string>("<xmlattr>.source")).string(),
                animationTiles, collisionGroups, textureName, registry);
        } else if (layer.first == "layer") {
            VertexArray& va = map.vertexArrays.emplace_back();
            va.vertices.setPrimitiveType(sf::Quads);
            va.vertices.resize(width * height * 4);
            va.texture = getTexture((fpath.parent_path() / textureName).string(), registry);
            auto textureSize = va.texture.getSize();
            int textureColl = textureSize.x / tileWidth;

            int layerId = layer.second.get<int>("<xmlattr>.id");
            string data = layer.second.get<string>("data");

            istrstream stream(data.c_str());
            int n;
            char ch;
            for (unsigned int j = 0; j < width; ++j) {
                for (unsigned int i = 0; i < height; ++i) {
                    stream >> n >> ch;
                    n-=1;
                    if (n < 0) {
                        // transparent tile
                        continue;
                    }
                    entt::registry::entity_type ent = entt::null;
                    if (animationTiles.find(n) != animationTiles.end()) {
                        ent = registry.create();
                        auto &sprite = registry.assign<Sprite>(ent);
                        auto &f = getFrameSet("tile-" + to_string(n), registry);
                        sprite.setTexture(*f.texture);
                        sprite.setPosition(i * tileWidth, j * tileHeight);
                        sprite.setOrigin(tileWidth * 0.5f, tileHeight * 0.5f);
                        sprite.setZ(0);
                        auto &as = registry.assign<AnimationState>(ent, textureName);
                        as.frameSet = &f;
                    } else {
                        sf::Vertex *quad = &va.vertices[(i + j * width) * 4];
                        const sf::Vector2f offset(0.5f * tileWidth, 0.5f * tileHeight);

                        quad[0].position = sf::Vector2f(i * tileWidth, j * tileHeight) - offset;
                        quad[1].position = sf::Vector2f((i + 1) * tileWidth, j * tileHeight) - offset;
                        quad[2].position = sf::Vector2f((i + 1) * tileWidth, (j + 1) * tileHeight) - offset;
                        quad[3].position = sf::Vector2f(i * tileWidth, (j + 1) * tileHeight) - offset;

                        int tu = n % textureColl;
                        int tv = n / textureColl;
                        quad[0].texCoords = sf::Vector2f(tu * tileWidth, tv * tileHeight);
                        quad[1].texCoords = sf::Vector2f((tu + 1) * tileWidth, tv * tileHeight);
                        quad[2].texCoords = sf::Vector2f((tu + 1) * tileWidth, (tv + 1) * tileHeight);
                        quad[3].texCoords = sf::Vector2f(tu * tileWidth, (tv + 1) * tileHeight);
                    }
                    auto collisionGroupIter = collisionGroups.find(n);
                    if (collisionGroupIter != collisionGroups.end()) {
                        if (!registry.valid(ent)) {
                            ent = registry.create();
                        }
                        auto &b2world = registry.ctx<b2World>();
                        b2BodyDef bodyDef;
                        bodyDef.type = b2_staticBody;
                        bodyDef.position.x = i;
                        bodyDef.position.y = j;
                        b2Body* body = b2world.CreateBody(&bodyDef);
                        b2PolygonShape shape;
                        shape.SetAsBox(0.5f, 0.5f);

                        b2FixtureDef fixtureDef;
                        fixtureDef.shape = &shape;

                        body->CreateFixture(&fixtureDef);
                        registry.assign<b2Body*>(ent, body);
                    }
                }
            }
            ++iLayer;
        }
    }
    tmx.clear();
}

void renderTileMap(entt::registry &registry) {
    auto &rt = registry.ctx<RenderTarget>();
    auto &r = rt.worldRenderTarget;
    auto &map = registry.ctx<Map>();
    for (auto &va : map.vertexArrays) {
        r.draw(va);
    }
}
