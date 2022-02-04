R"(
#version 120

uniform mat4 MVP;
uniform vec3 chunkOffset;

attribute vec3 vertexPos;
attribute vec2 vertexTex;
attribute vec3 vertexNorm;

varying vec3 vertexPosOut;
varying vec2 vertexTexOut;
varying vec3 vertexNormOut;

void main()
{
    gl_Position = MVP * vec4(chunkOffset + vertexPos, 1.0f);
    vertexPosOut = chunkOffset + vertexPos;
    vertexTexOut = vertexTex;
    vertexNormOut = vertexNorm;
}
)"