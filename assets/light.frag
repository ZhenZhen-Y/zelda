const vec2 resolution = vec2(1280, 768);
const float lightRange = 600.0;
uniform sampler2D texture;

uniform vec4 envLightColor;
uniform float envLightIntensity;

uniform vec2 lightPositions[5];
uniform vec4 lightColors[5];
uniform float lightIntensities[5];
uniform int lightCount;


vec4 calcLight() {
    vec4 c = vec4(0.0, 0.0, 0.0, 0.0);
    for (int i = 0; i < lightCount; ++i) {
        float d = distance(lightPositions[i], gl_FragCoord.xy);
        float I = lightIntensities[i];
        if (d < lightRange * I) {
            c += lightColors[i] * I * (1.0 - pow(d, 0.7) / pow(lightRange * I, 0.7));
        }
    }
    return c;
}


void main() {
    vec4 texColor = texture2D(texture, gl_TexCoord[0].xy);
    gl_FragColor = texColor * min(envLightColor * envLightIntensity + calcLight(), 1.0);
}