
#include <string>

#include <glm/mat4x4.hpp>

#include "shader.h"

using namespace std;

const string ov_frag =
#include "shaders/ov_frag.glsl"
;

const string ov_vert = 
#include "shaders/ov_vert.glsl"
;

class Overlay {
public:
    int width;
    int height;
    Shader shader;

    Overlay(): shader(ov_vert, ov_frag) {
        
    }

    void setWindowSize(int width, int height) {
        this->width = width;
        this->height = height;
    }

    void render() {
        glLineWidth(3);

        float s = 10;
        float dw = s / (float)width;
        float dh = s / (float)height;

        glBegin(GL_LINES);
        glColor3f(1, 1, 1);
        glVertex2f(0, -dh);
        glVertex2f(0, dh);
        glVertex2f(-dw, 0);
        glVertex2f(dw, 0);
        glEnd();
    }
};
