#ifndef ZELDA_TILEMAP_H
#define ZELDA_TILEMAP_H

#include "common.h"
#define BOOST_NO_AUTO_PTR
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "VertexArray.h"

struct Map {
    vector<VertexArray> vertexArrays;
};

void loadMap(const std::string& name, entt::registry &registry);
void renderTileMap(entt::registry &registry);

#endif //ZELDA_TILEMAP_H
