#include "graphic.h"
#include "components.h"
#include "utils.h"
#include <cstdlib>
#include <csv/csv.h>

struct EnvironmentLight {
    sf::Glsl::Vec4 color;
    float intensity;
};

struct EnvironmentLightManager {
    vector<EnvironmentLight> lightDefines;
    vector<float> timeSeparate;
};

void initializeGraphic(entt::registry &registry) {
    registry.set<EnvironmentLightManager>();
}

void loadEnvironmentLightSheet(const string &filename, entt::registry &registry) {
    auto &elm = registry.ctx<EnvironmentLightManager>();
    ifstream sheet;
    sheet.open(filename);
    if (!sheet.is_open()) {
        throw runtime_error("Failed to load environment light sheet: " + filename);
    }
    for(CSVIterator irow(sheet); irow != CSVIterator(); ++irow) {
        auto row = *irow;
        float t = atof(row[0].c_str());
        sf::Glsl::Vec4 color(
            atof(row[1].c_str()) / 255.0,
            atof(row[2].c_str()) / 255.0,
            atof(row[3].c_str()) / 255.0,
            1.0);
        float intensity = atof(row[4].c_str());
        elm.lightDefines.push_back(EnvironmentLight{color, intensity});
        elm.timeSeparate.push_back(t);
    }
}

sf::Glsl::Vec4 mix(const sf::Glsl::Vec4 &v1, const sf::Glsl::Vec4 &v2, float ratio) {
    return sf::Glsl::Vec4{
        (float)(v1.x * ratio + v2.x * (1.0 - ratio)),
        (float)(v1.y * ratio + v2.y * (1.0 - ratio)),
        (float)(v1.z * ratio + v2.z * (1.0 - ratio)),
        (float)(v1.w * ratio + v2.w * (1.0 - ratio))
    };
}

float mix(float v1, float v2, float ratio) {
    return v1 * ratio + v2 * (1.0 - ratio);
}

EnvironmentLight getEnvironmentLight(const entt::registry &registry) {
    auto &elm = registry.ctx<EnvironmentLightManager>();
    float hr = registry.ctx<Clock>().time;

    int separateCount = elm.timeSeparate.size();
    int i = 0;
    float ratio = 0.0;
    for (; i < separateCount - 1; ++i) {
        if (hr > elm.timeSeparate[i] && hr <= elm.timeSeparate[i+1]) {
            break;
        }
    }
    float span;
    if (i == separateCount - 1) {
        span = elm.timeSeparate[0] + (24 - elm.timeSeparate[separateCount-1]);
        ratio = (hr > elm.timeSeparate[separateCount-1]) ? (hr - elm.timeSeparate[separateCount-1]) / span : (24 - elm.timeSeparate[separateCount-1] + hr) / span;
    } else {
        span = elm.timeSeparate[i + 1] - elm.timeSeparate[i];
        ratio = (hr - elm.timeSeparate[i]) / span;
    }

    return EnvironmentLight{
        mix(elm.lightDefines[i].color, elm.lightDefines[(i+1) % separateCount].color, 1.0 - ratio),
        mix(elm.lightDefines[i].intensity, elm.lightDefines[(i+1) % separateCount].intensity, 1.0 - ratio)
    };
}

sf::Vector2f shaderPosition(const b2Vec2 &p, const sf::Vector2f vp, const sf::Vector2f vs) {
    sf::Vector2f rp;
    rp.x = (meterToPixel(p.x) - vp.x + vs.x / 2) * WORLD_VIEW_SCALE;
    rp.y = (vs.y - (meterToPixel(p.y) - vp.y + vs.y / 2)) * WORLD_VIEW_SCALE;
    return rp;
}

sf::Glsl::Vec4 shaderColor(sf::Color &color) {
    sf::Glsl::Vec4 c;
    c.x = (float)color.r / 255.0;
    c.y = (float)color.g / 255.0;
    c.z = (float)color.b / 255.0;
    c.w = (float)color.a / 255.0;
    return c;
}

void updateGraphic(entt::registry &registry) {
    auto &rt = registry.ctx<RenderTarget>();
    auto envLight = getEnvironmentLight(registry);
    rt.worldShader.setUniform("envLightIntensity", envLight.intensity);
    rt.worldShader.setUniform("envLightColor", envLight.color);

    sf::Vector2f worldViewPosition = rt.worldView.getCenter();
    sf::Vector2f worldViewSize = rt.worldView.getSize();

    const int MaxLightCount = 5;
    sf::Vector2f lightPositions[MaxLightCount];
    sf::Glsl::Vec4 lightColors[MaxLightCount];
    float lightIntensities[MaxLightCount];

    int lightIndex = 0;
    registry.view<Light, b2Body*>().each([&registry, &worldViewPosition, &worldViewSize,
                                 &lightPositions, &lightColors, &lightIntensities, &lightIndex](auto ent, Light &light, b2Body *body) {
        if (lightIndex >= MaxLightCount) {
            return;
        }
        lightPositions[lightIndex] = shaderPosition(body->GetPosition(), worldViewPosition, worldViewSize);
        lightColors[lightIndex] = shaderColor(light.color);
        lightIntensities[lightIndex] = light.intensity;
        ++lightIndex;
    });

    rt.worldShader.setUniformArray("lightPositions", lightPositions, lightIndex);
    rt.worldShader.setUniformArray("lightColors", lightColors, lightIndex);
    rt.worldShader.setUniformArray("lightIntensities", lightIntensities, lightIndex);
    rt.worldShader.setUniform("lightCount", lightIndex);
}
