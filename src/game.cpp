
#ifndef _WIN32
#include <sys/stat.h>
#endif

#include <iostream>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "world.h"
#include "saver.h"

using namespace std;

GLFWwindow *window;
World *global_world;
const char *saveFile = nullptr;
bool has_focus = true;

void init() {    
    if (!glfwInit()) {
        cerr << "Could not initialize." << endl;
        exit(-1); 
    }

    window = glfwCreateWindow(800, 600, "DiceCraft", NULL, NULL);
    if (!window) {
        glfwTerminate();
        cerr << "Could not create window." << endl;
        exit(-1);
    }

    glfwMakeContextCurrent(window);

    gladLoadGL();

    const char *version = (const char *)glGetString(GL_VERSION);
    if (version != NULL) {
        cout << "OpenGL version: " << version << endl;
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
        has_focus = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    if (key == GLFW_KEY_P && action == GLFW_RELEASE) {
        if (saveFile != nullptr) {
            cout << "[*] Saving...";
            Saver::save(saveFile, global_world);
            cout << "done!" << endl;
        } else {
            cout << "[*] No save file! Run as ./dicecraft <save_file>" << endl;
        }
    }

    global_world->key_input(window, key, scancode, action, mods);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    global_world->mouse_input(window, xpos, ypos);
}

void window_size_callback(GLFWwindow* window, int width, int height) {
    global_world->set_window_size(width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        has_focus = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

int main(int argc, char **argv) {
    init();

    global_world = new World();

    for (int x = -30; x <= 30; ++x) {
        for (int y = -30; y < 30; ++y) {
            for (int z = 0; z < 10; ++z) {
                global_world->setBlock(x,y,z,Block::METAL);
            }
        }
    }

    global_world->player.pos = glm::vec3(0,0,15);
    global_world->player.dir.y = -0.1;
    global_world->player.updateTarget();
    global_world->player.flying = false;

    if (argc == 2) {
        saveFile = argv[1];

#ifndef _WIN32
        struct stat buf;
        if (stat(saveFile, &buf) == 0) {
            cout << "[*] Loading from save file: " << saveFile << endl;
            Saver::load(saveFile, global_world);
        } else {
            cout << "[*] Save file doesn't exist, starting new world..." << endl;
#endif
        }
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    global_world->setup();

    float lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        global_world->render();

        glfwSwapBuffers(window);
        glfwPollEvents();

        double currentTime = glfwGetTime();
        float delta = float(currentTime - lastTime);
        lastTime = currentTime;

        global_world->update(window, delta, has_focus);
    }

    glfwTerminate();
    return 0;
}
