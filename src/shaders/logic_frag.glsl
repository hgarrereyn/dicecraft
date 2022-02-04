R"(
#version 120

uniform sampler2D tex;
uniform vec3 cameraPos;

varying vec3 vertexPosOut;
varying vec2 vertexTexOut;
varying vec3 vertexNormOut;

vec3 light = normalize(vec3(3,2,-1));
float amb = 0.3f;

float getFogFactor(float d) {
    const float FogMax = 80.0;
    const float FogMin = 20.0;

    if (d >= FogMax) return 1.0f;
    if (d <= FogMin) return 0.0f;

    return 1 - (FogMax - d) / (FogMax - FogMin);
}

void main() {
    float lightIntensity = clamp(dot(light, vertexNormOut), 0, 1);
    
    float fac = clamp(amb + lightIntensity, 0, 1);

    vec4 color = texture2D(tex, vertexTexOut);
    vec4 bcolor = vec4(color.xyz * fac, color.w);

    float d = distance(cameraPos, vertexPosOut);
    float a = getFogFactor(d);

    gl_FragColor = (vec4(1.0f, 0.67f, 0.65f, 1) * a) + (bcolor * (1 - a));
}
)"
