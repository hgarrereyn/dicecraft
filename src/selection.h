
#include <string>

#include <glm/mat4x4.hpp>

#include "shader.h"

using namespace std;

const string sel_frag =
#include "shaders/sel_frag.glsl"
;

const string sel_vert = 
#include "shaders/sel_vert.glsl"
;

class Selection {
public:
    int x;
    int y;
    int z;
    Shader shader;

    GLint uniform_proj;
    GLint uniform_selOffset;
    GLint attrib_vertexPos;

    GLuint buf_vertexPos;
    GLuint buf_index;

    vector<GLfloat> position;
    vector<GLubyte> index;

    void updateMesh(float h) {
        position.clear();
        index.clear();

        position.insert(position.end(), {
            0, 0, 0,
            0, 0, h,
            0, 1, 0,
            0, 1, h,
            1, 0, 0,
            1, 0, h,
            1, 1, 0,
            1, 1, h,
        });

        index.insert(index.end(), {
            0, 1,
            2, 3,
            4, 5,
            6, 7,

            0, 2,
            1, 3,
            4, 6,
            5, 7,

            0, 4,
            1, 5,
            2, 6,
            3, 7,
        });

        glBindBuffer(GL_ARRAY_BUFFER, buf_vertexPos);
        glBufferData(GL_ARRAY_BUFFER, position.size() * sizeof(GLfloat), position.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf_index);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(GLubyte), index.data(), GL_STATIC_DRAW);
    }

    Selection(): x(0), y(0), z(0), shader(sel_vert, sel_frag) {
        uniform_proj = glGetUniformLocation(shader.program, "proj");
        uniform_selOffset = glGetUniformLocation(shader.program, "selOffset");
        attrib_vertexPos = glGetAttribLocation(shader.program, "vertexPos");
        
        glBindBuffer(GL_ARRAY_BUFFER, buf_vertexPos);
        glVertexAttribPointer(attrib_vertexPos, 3, GL_FLOAT, false, 0, 0);

        glGenBuffers(1, &buf_vertexPos);
        glGenBuffers(1, &buf_index);

        updateMesh(1);
    }

    void setProjection(glm::mat4 proj) {
        glUniformMatrix4fv(uniform_proj, 1, GL_FALSE, &proj[0][0]);
    }

    void setOffset(int x, int y, int z) {
        glUniform3f(uniform_selOffset, x, y, z);
    }

    void setHeight(float h) {
        updateMesh(h);
    }

    void render() {
        glEnableVertexAttribArray(0);

        glLineWidth(8);

        glBindBuffer(GL_ARRAY_BUFFER, buf_vertexPos);
        glVertexAttribPointer(attrib_vertexPos, 3,  GL_FLOAT, false, 0, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf_index);
        glDrawElements(GL_LINES, index.size(), GL_UNSIGNED_BYTE, (void *)0);

        glDisableVertexAttribArray(0);
    }
};
