R"(
#version 120

uniform mat4 proj;
uniform vec3 selOffset;

attribute vec3 vertexPos;

float fac = 1.05f;

void main()
{
    vec3 vp = ((vertexPos - vec3(0.5, 0.5, 0.5)) * fac) + vec3(0.5, 0.5, 0.5);
    gl_Position = proj * vec4(selOffset + vp, 1.0f);
}
)"