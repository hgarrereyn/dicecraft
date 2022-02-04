
#pragma once

#include <iostream>

#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "block.h"

#define PLAYER_HEIGHT 1.8f
#define GRAVITY -24
#define MAX_VEL 50
#define VEL_DECAY 0.8
#define COLLISION_PAD 0.1f

using namespace std;

struct World;

class Player {

public:
    // Player faces (1,0,0) with yaw=0, pitch=0
    glm::vec3 pos; // (x, y, z)
    glm::vec2 dir; // (yaw, pitch)
    glm::vec3 target;
    bool flying = true;
    bool onSolidGround = true;

    glm::vec3 vel;

    Player(): pos(0,0,0), dir(0,0), vel(0,0,0) {
        updateTarget();
    }

    glm::mat4 getProjectionMatrix(float aspect_ratio) {
        glm::mat4 view = glm::lookAt(
            pos,
            pos + target,
            glm::vec3(0, 0, 1)
        );

        glm::mat4 proj = glm::perspective(glm::pi<float>() * 0.45f, aspect_ratio, 0.1f, 100.f);
        glm::mat4 model = glm::mat4(1.0f);

        return proj * view * model;
    }

    void updateTarget() {
        target.x = glm::cos(dir.x) * glm::cos(dir.y);
        target.y = glm::sin(dir.x) * glm::cos(dir.y);
        target.z = glm::sin(dir.y);
    }

    void rotX(float angle) {
        dir.x += angle;
        dir.x = remainderf(dir.x, 2 * glm::pi<float>());
        updateTarget();
    }

    void rotY(float angle) {
        dir.y += angle;
        float eps = 1e-6;
        dir.y = fmin(fmax(dir.y, -(glm::pi<float>() / 2) + eps), (glm::pi<float>() / 2) - eps);
        updateTarget();
    }

    void moveX(float units) {
        if (flying) {
            pos.x += glm::cos(dir.x) * units;
            pos.y += glm::sin(dir.x) * units;
        } else {
            units *= 30;
            vel.x += glm::cos(dir.x) * units;
            vel.y += glm::sin(dir.x) * units;
        }
    }

    void moveY(float units) {
        if (flying) {
            pos.x += glm::cos(dir.x + (glm::pi<float>() / 2)) * units;
            pos.y += glm::sin(dir.x + (glm::pi<float>() / 2)) * units;
        } else {
            units *= 30;
            vel.x += glm::cos(dir.x + (glm::pi<float>() / 2)) * units;
            vel.y += glm::sin(dir.x + (glm::pi<float>() / 2)) * units;
        }
    }

    void moveZ(float units) {
        if (flying) {
            pos.z += units;
        } else {
            if (units > 0 && onSolidGround) {
                // jump
                vel.z = 8;
                onSolidGround = false;
            }
        }
    }

    void toggleFlying() {
        flying = !flying;

        if (flying) {
            vel = glm::vec3(0);
        }
    }

    void updatePosition(World *world, float delta);

    void raycast(World *world, glm::vec<3, int> *sel, glm::vec<3, int> *sel_empty, bool *hit, int maxDepth = 6, float step = 0.05);
};
