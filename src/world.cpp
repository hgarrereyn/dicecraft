
#include <iostream>
#include <string>

#include "opengl.h"
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi

#include "world.h"
#include "shader.h"
#include "chunk.h"
#include "chunk_generator.h"
#include "selection.h"
#include "overlay.h"

namespace emb_tilemap {
    #include "emb_tilemap.h"
}

using namespace std;

#define MOUSE_SPEED 0.3f
#define MOVE_SPEED 6.0f

#define CHUNK_VIZ_RADIUS 4

ChunkShader *chunkShader;
LogicShader *logicShader;
Selection *selection;
Overlay *overlay;

Tilemap *tilemap;

glm::vec<3, int> sel;
glm::vec<3, int> sel_empty;
bool has_sel;

bool left_mouse_debounce = true;
bool right_mouse_debounce = true;
Block place_block = Block::EMPTY;
BlockDir place_dir = BlockDir::NORTH;
bool do_interact = false;

void World::setup() {
    chunkShader = new ChunkShader();
    logicShader = new LogicShader();
    selection = new Selection();
    overlay = new Overlay();

    // tilemap = new Tilemap("../res/tilemap.png");
    tilemap = new Tilemap(emb_tilemap::data, 256, 256);
}

void World::render() {
    glClearColor(1.0f, 0.67f, 0.65f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glDisable(GL_CULL_FACE);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_EQUAL, 1.0f);

    glm::mat4 proj = player.getProjectionMatrix(this->aspect_ratio);

    chunkShader->use();
    glUniformMatrix4fv(chunkShader->MVP, 1, GL_FALSE, &proj[0][0]);
    glUniform3f(chunkShader->cameraPos, player.pos.x, player.pos.y, player.pos.z);

    ChunkCoord pcoord = ChunkCoord::FromWorld((int)player.pos.x, (int)player.pos.y);
    for (int x = -CHUNK_VIZ_RADIUS; x <= CHUNK_VIZ_RADIUS; ++x) {
        for (int y = -CHUNK_VIZ_RADIUS; y <= CHUNK_VIZ_RADIUS; ++y) {
            ChunkCoord coord = ChunkCoord(pcoord.x + x, pcoord.y + y);

            if (chunks.count(coord) == 0) {
                setChunk(coord, ChunkGenerator::Generate(coord));
            }

            chunks[coord].renderStatic(chunkShader, tilemap);
        }
    }

    logicShader->use();
    glUniformMatrix4fv(logicShader->MVP, 1, GL_FALSE, &proj[0][0]);
    glUniform3f(logicShader->cameraPos, player.pos.x, player.pos.y, player.pos.z);

    for (int x = -CHUNK_VIZ_RADIUS; x <= CHUNK_VIZ_RADIUS; ++x) {
        for (int y = -CHUNK_VIZ_RADIUS; y <= CHUNK_VIZ_RADIUS; ++y) {
            ChunkCoord coord = ChunkCoord(pcoord.x + x, pcoord.y + y);
            chunks[coord].renderLogic(logicShader, tilemap, currTick);
        }
    }

    if (has_sel) {
        selection->shader.use();

        Block b = getBlock(sel.x, sel.y, sel.z);
        if (b == Block::DICESTONE) {
            selection->setHeight(1 / 8.0f);
        } else {
            selection->setHeight(1);
        }

        selection->setProjection(proj);
        selection->setOffset(sel.x, sel.y, sel.z);
        selection->render();
    }

    overlay->shader.use();
    overlay->setWindowSize(width, height);
    overlay->render();
}

void World::update(GLFWwindow *window, float delta, bool has_focus) {
    if (has_focus) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        player.rotX(-(delta * MOUSE_SPEED * (xpos - ((float)this->width / 2))));
        player.rotY(-(delta * MOUSE_SPEED * (ypos - ((float)this->height / 2))));
        glfwSetCursorPos(window, (double)width / 2, (double)height / 2);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        player.moveY(delta * MOVE_SPEED);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        player.moveY(-(delta * MOVE_SPEED));
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        player.moveX(delta * MOVE_SPEED);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        player.moveX(-(delta * MOVE_SPEED));
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        player.moveZ(delta * MOVE_SPEED);
    }
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        player.moveZ(-(delta * MOVE_SPEED));
    }

    player.updatePosition(this, delta);
    player.raycast(this, &sel, &sel_empty, &has_sel);

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (left_mouse_debounce) {
            left_mouse_debounce = false;

            if (has_focus && has_sel) {
                this->setBlock(sel.x, sel.y, sel.z, Block::EMPTY);
            }
        }
    } else {
        left_mouse_debounce = true;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        if (right_mouse_debounce) {
            right_mouse_debounce = false;
            do_interact = true;
        }
    } else {
        right_mouse_debounce = true;
    }

    if (place_block != Block::EMPTY) {
        if (has_focus && has_sel) {
            float dx = (player.pos.x - ((float)sel_empty.x + 0.5));
            float dy = (player.pos.y - ((float)sel_empty.y + 0.5));

            if (abs(dx) > abs(dy)) {
                if (dx > 0) place_dir = BlockDir::NORTH;
                else place_dir = BlockDir::SOUTH;
            } else {
                if (dy > 0) place_dir = BlockDir::WEST;
                else place_dir = BlockDir::EAST;
            }

            this->setBlock(sel_empty.x, sel_empty.y, sel_empty.z, place_block, place_dir);
        }

        place_block = Block::EMPTY;
    }

    if (do_interact) {
        if (has_focus && has_sel) {
            this->interact(sel.x, sel.y, sel.z);
        }
        do_interact = false;
    }

    elapsed += (int)(delta * 1000);
    if (elapsed > TICK_TIME) {
        elapsed %= TICK_TIME;
        tick();
    }
}

void World::key_input(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_1: place_block = Block::DICE; return;
            case GLFW_KEY_2: place_block = Block::GLASS; return;
            case GLFW_KEY_3: place_block = Block::METAL; return;
            case GLFW_KEY_4: place_block = Block::DICESTONE; return;

            case GLFW_KEY_5: place_block = Block::SPINNER; return;
            case GLFW_KEY_6: place_block = Block::BUMPER; return;
            case GLFW_KEY_7: place_block = Block::TERM; return;
            case GLFW_KEY_8: place_block = Block::BUTTON; return;
            case GLFW_KEY_9: place_block = Block::SIGNAL; return;
            case GLFW_KEY_0: place_block = Block::LOCK; return;

            case GLFW_KEY_T: place_block = Block::JUNCTION; return;
            case GLFW_KEY_Y: place_block = Block::SHIFTER; return;
            case GLFW_KEY_U: place_block = Block::LETTERS; return;
            case GLFW_KEY_I: place_block = Block::MUT; return;
            case GLFW_KEY_O: place_block = Block::FLAG; return;
            case GLFW_KEY_R: place_block = Block::SLOT; return;

            case GLFW_KEY_F: player.toggleFlying(); return;
            
            case GLFW_KEY_LEFT_SHIFT: do_interact = true; return;
        }
    }
}

void World::mouse_input(GLFWwindow* window, double xpos, double ypos) {

}

void World::tick() {
    currTick += 1;

    auto it = chunks.begin();
    while (it != chunks.end()) {
        it->second.tick(currTick);
        it++;
    }

    it = chunks.begin();
    while (it != chunks.end()) {
        it->second.postTick();
        it++;
    }
}
