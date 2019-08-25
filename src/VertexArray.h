#ifndef ZELDA_VERTEXARRAY_H
#define ZELDA_VERTEXARRAY_H

#include "common.h"

class VertexArray: public sf::Drawable, public sf::Transformable {
public:
    inline void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        states.texture = &texture;
        target.draw(vertices, states);
    }
    sf::VertexArray vertices;
    sf::Texture texture;
};


#endif //ZELDA_VERTEXARRAY_H
