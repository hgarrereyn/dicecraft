
#include <string>

#include "opengl.h"

#include "shader.h"

using namespace std;

const string logic_frag =
#include "shaders/logic_frag.glsl"
;

const string logic_vert = 
#include "shaders/logic_vert.glsl"
;

class LogicShader : public Shader {

public:
    GLint MVP;
    GLint chunkOffset;
    GLint tex;
    GLint vertexPos;
    GLint vertexTex;
    GLint vertexNorm;
    GLint cameraPos;

    GLuint vertex_buffer;
    GLuint texture_buffer;
    GLuint normal_buffer;
    GLuint index_buffer;

    LogicShader() : Shader(logic_vert, logic_frag) {
        MVP = glGetUniformLocation(this->program, "MVP");
        chunkOffset = glGetUniformLocation(this->program, "chunkOffset");
        tex = glGetUniformLocation(this->program, "tex");

        cameraPos = glGetUniformLocation(this->program, "cameraPos");
        vertexPos = glGetAttribLocation(this->program, "vertexPos");
        vertexTex = glGetAttribLocation(this->program, "vertexTex");
        vertexNorm = glGetAttribLocation(this->program, "vertexNorm");

        glGenBuffers(1, &vertex_buffer);
        glGenBuffers(1, &texture_buffer);
        glGenBuffers(1, &normal_buffer);
        glGenBuffers(1, &index_buffer);

        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glVertexAttribPointer(vertexPos, 3,  GL_FLOAT, false, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, texture_buffer);
        glVertexAttribPointer(vertexTex, 2,  GL_FLOAT, false, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
        glVertexAttribPointer(vertexTex, 3,  GL_FLOAT, false, 0, 0);
    }

    void writeVertexData(vector<GLfloat> buffer) {
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(GLfloat), buffer.data(), GL_STATIC_DRAW);
    }

    void writeTextureData(vector<GLfloat> buffer) {
        glBindBuffer(GL_ARRAY_BUFFER, texture_buffer);
        glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(GLfloat), buffer.data(), GL_STATIC_DRAW);
    }

    void writeNormalData(vector<GLfloat> buffer) {
        glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
        glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(GLfloat), buffer.data(), GL_STATIC_DRAW);
    }

    void writeIndexData(vector<GLushort> buffer) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer.size() * sizeof(GLushort), buffer.data(), GL_STATIC_DRAW);
    }
};
