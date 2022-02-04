
#pragma once

#include "opengl.h"
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>
#include <vector>

using namespace std;


class Shader {
private:
    GLuint loadShader(string data, GLenum type) {
        GLuint shader = glCreateShader(type);
        if (shader == 0) {
            cerr << "Error creating shader" << endl;
            exit(-1);
        }

        const char *source = data.c_str();
        glShaderSource(shader, 1, &source , NULL);
        glCompileShader(shader);

        GLint isCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

        if (isCompiled == GL_FALSE) {
            GLint maxLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

            string errorLog;
            errorLog.resize(maxLength);
            glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

            cerr << "Error compiling shader:" << endl;
            cerr << errorLog << endl;

            glDeleteShader(shader);
            exit(-1);
        }

        return shader;
    }

    void loadProgram(string vertexData, string fragmentData) {
        program = glCreateProgram();
        if (program == 0) {
            cerr << "Error creating program" << endl;
            exit(-1);
        }

        vertex = loadShader(vertexData, GL_VERTEX_SHADER);
        fragment = loadShader(fragmentData, GL_FRAGMENT_SHADER);

        glAttachShader(program, vertex);
        glAttachShader(program, fragment);

        glLinkProgram(program);

        GLint is_linked = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
        if (is_linked == GL_FALSE) {
            GLint maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

            string errorLog;
            errorLog.resize(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, &errorLog[0]);

            cerr << "Could not link program:" << endl;
            cerr << errorLog << endl;

            glDeleteProgram(program);
            exit(-1);
        }
    }

public:
    GLuint program;
    GLuint vertex;
    GLuint fragment;
    
    Shader(string vertexData, string fragmentData) {
        loadProgram(vertexData, fragmentData);
    }

    void use() {
        glUseProgram(program);
    }
};
