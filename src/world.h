
#pragma once

#include <tuple>
#include <vector>
#include <unordered_map>

#include "chunk.h"
#include "player.h"

// (ms)
#define TICK_TIME 200

using namespace std;

class Save;

class World {
public:
    unordered_map<ChunkCoord, Chunk> chunks;
    Player player;
    int width = 800;
    int height = 600;
    float aspect_ratio = (float)800/(float)600;
    int elapsed = 0;
    int currTick = 0;

    World() {}

    void setBlock(int x, int y, int z, Block block, BlockDir dir = BlockDir::NORTH) {
        ChunkCoord coord = ChunkCoord::FromWorld(x, y);

        if (chunks.count(coord) == 0) {
            setChunk(coord, Chunk(coord));
        }

        int cx_off = x - (coord.x * CHUNK_WIDTH);
        int cy_off = y - (coord.y * CHUNK_WIDTH);
        chunks[coord].setBlock(cx_off, cy_off, z, block, dir);

        if (cx_off == 0) {
            ChunkCoord back(coord.x+1, coord.y);
            if (chunks.count(back) != 0) chunks[back].setDirty();
        }
        if (cx_off == CHUNK_WIDTH - 1) {
            ChunkCoord front(coord.x+1, coord.y);
            if (chunks.count(front) != 0) chunks[front].setDirty();
        }
        if (cy_off == 0) {
            ChunkCoord right(coord.x, coord.y-1);
            if (chunks.count(right) != 0) chunks[right].setDirty();
        }
        if (cy_off == CHUNK_WIDTH - 1) {
            ChunkCoord left(coord.x, coord.y+1);
            if (chunks.count(left) != 0) chunks[left].setDirty();
        }
    }

    void setPulse(int x, int y, int z, uint8_t val) {
        ChunkCoord coord = ChunkCoord::FromWorld(x, y);
        if (chunks.count(coord) == 0) return;
        
        int cx_off = x - (coord.x * CHUNK_WIDTH);
        int cy_off = y - (coord.y * CHUNK_WIDTH);
        chunks[coord].setPulse(cx_off, cy_off, z, val);
    }

    void interact(int x, int y, int z) {
        ChunkCoord coord = ChunkCoord::FromWorld(x, y);
        if (chunks.count(coord) > 0) {
            int cx_off = x - (coord.x * CHUNK_WIDTH);
            int cy_off = y - (coord.y * CHUNK_WIDTH);

            chunks[coord].interact(cx_off, cy_off, z);
        }
    }

    Block getBlock(int x, int y, int z) {
        if (z < 0 || z > CHUNK_HEIGHT) {
            return Block::EMPTY;
        }

        ChunkCoord coord = ChunkCoord::FromWorld(x, y);
        if (chunks.count(coord) == 0) {
            return Block::EMPTY;
        } else {
            return chunks[coord].getBlock(x - (coord.x * CHUNK_WIDTH), y - (coord.y * CHUNK_WIDTH), z);
        }
    }

    void setChunk(ChunkCoord coord, Chunk ck) {
        ck.setWorld(this);
        ck.setDirty();
        chunks[coord] = ck;

        ChunkCoord left(coord.x, coord.y+1);
        ChunkCoord right(coord.x, coord.y-1);
        ChunkCoord front(coord.x-1, coord.y);
        ChunkCoord back(coord.x+1, coord.y);

        if (chunks.count(left) != 0) chunks[left].setDirty();
        if (chunks.count(right) != 0) chunks[right].setDirty();
        if (chunks.count(front) != 0) chunks[front].setDirty();
        if (chunks.count(back) != 0) chunks[back].setDirty();
    }

    void set_window_size(int width, int height) {
        this->width = width;
        this->height = height;
        this->aspect_ratio = (float)width / (float)height;
    }

    void load(Save *save);
    void save(Save *save);

    void setup();
    void render();
    void update(GLFWwindow *window, float delta, bool has_focus);
    void key_input(GLFWwindow* window, int key, int scancode, int action, int mods);
    void mouse_input(GLFWwindow* window, double xpos, double ypos);
    void tick();
};
