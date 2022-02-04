
#include "player.h"

#include "world.h"
#include "block.h"

void Player::updatePosition(World *world, float delta) {
    if (!flying) {
        // Apply gravity.
        vel.z += delta * GRAVITY;
        if (vel.z < -MAX_VEL) {
            vel.z = -MAX_VEL;
        }
    }

    pos += vel * delta;
    vel.x *= VEL_DECAY * delta;
    vel.y *= VEL_DECAY * delta;

    int fx = floor(pos.x);
    int fy = floor(pos.y);
    
    // Check for hitting ground.
    if (vel.z < 0 && IsCollider(world->getBlock(fx, fy, floor(pos.z - PLAYER_HEIGHT - COLLISION_PAD)))) {
        onSolidGround = true;
        vel.z = 0;
        pos.z = floor(pos.z - PLAYER_HEIGHT - COLLISION_PAD) + (PLAYER_HEIGHT + 1);
    }

    int fz = floor(pos.z - PLAYER_HEIGHT);

    // Check for hitting wall.
    if (vel.x > 0 && (
            IsCollider(world->getBlock(floor(pos.x + COLLISION_PAD), fy, fz))
            || IsCollider(world->getBlock(floor(pos.x + COLLISION_PAD), fy, fz + 1))
        )) {
        vel.x = 0;
        pos.x = floor(pos.x + COLLISION_PAD) - COLLISION_PAD;
    }
    if (vel.x < 0 && (
            IsCollider(world->getBlock(floor(pos.x - COLLISION_PAD), fy, fz))
            || IsCollider(world->getBlock(floor(pos.x - COLLISION_PAD), fy, fz + 1))
        )) {
        vel.x = 0;
        pos.x = floor(pos.x - COLLISION_PAD) + COLLISION_PAD + 1;
    }

    fx = floor(pos.x);

    if (vel.y > 0 && (
            IsCollider(world->getBlock(fx, floor(pos.y + COLLISION_PAD), fz))
            || IsCollider(world->getBlock(fx, floor(pos.y + COLLISION_PAD), fz + 1))
        )) {
        vel.y = 0;
        pos.y = floor(pos.y + COLLISION_PAD) - COLLISION_PAD;
    }
    if (vel.y < 0 && (
            IsCollider(world->getBlock(fx, floor(pos.y - COLLISION_PAD), fz))
            || IsCollider(world->getBlock(fx, floor(pos.y - COLLISION_PAD), fz + 1))
        )) {
        vel.y = 0;
        pos.y = floor(pos.y - COLLISION_PAD) + COLLISION_PAD + 1;
    }
}

void Player::raycast(World *world, glm::vec<3, int> *sel, glm::vec<3, int> *sel_empty, bool *hit, int maxDepth, float step) {
    
    // Ensure we are currently not occluded.
    int fx = (int)floor(pos.x);
    int fy = (int)floor(pos.y);
    int fz = (int)floor(pos.z);

    if (world->getBlock(fx, fy, fz) != Block::EMPTY) {
        *hit = false;
        return;
    }

    sel_empty->x = fx;
    sel_empty->y = fy;
    sel_empty->z = fz;
    
    for (float offset = 0; offset < maxDepth; offset += step) {
        glm::vec3 curr = pos + (target * offset);

        int fx = (int)floor(curr.x);
        int fy = (int)floor(curr.y);
        int fz = (int)floor(curr.z);

        Block b = world->getBlock(fx, fy, fz);
        if (b != Block::EMPTY) {
            if (curr.z - fz < BlockHeight(b)) {
                *hit = true;
                sel->x = fx;
                sel->y = fy;
                sel->z = fz;
                return;
            }
        } else {
            sel_empty->x = fx;
            sel_empty->y = fy;
            sel_empty->z = fz;
        }
    }

    *hit = false;
}
