
#pragma once

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>

#include <vector>
#include <cstring>
#include <unordered_map>
#include <memory>

#include "chunk_shader.h"
#include "logic_shader.h"
#include "block.h"
#include "tilemap.h"
#include "computer.h"

using namespace std;

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 128

struct World;

struct ChunkCoord {
    int x;
    int y;

    ChunkCoord(int _x, int _y): x(_x), y(_y) {}

    bool operator==(const ChunkCoord &other) const {
        return (x == other.x && y == other.y);
    }

    static ChunkCoord FromWorld(int x, int y) {
        return ChunkCoord(
            x < 0 ? -(((-x - 1) / CHUNK_WIDTH) + 1) : x / CHUNK_WIDTH,
            y < 0 ? -(((-y - 1) / CHUNK_WIDTH) + 1) : y / CHUNK_WIDTH
        );
    }
};

namespace std {
    template <>
    struct hash<ChunkCoord> {
        size_t operator()(ChunkCoord const& coord) const {                                              
            return ((coord.x * 65536) + coord.y);
        }
    };
}

class Chunk {
    World *world;
    BlockState blocks[CHUNK_HEIGHT][CHUNK_WIDTH][CHUNK_WIDTH];
    ChunkCoord coord;
    bool dirty = false;
    bool logicDirty = false;
    bool isTouched = false;
    int currTick = 0;

    // Dynamic state.
    unordered_map<uint32_t, shared_ptr<Computer>> computers;
    bool ds_pulse_next[CHUNK_HEIGHT][CHUNK_WIDTH][CHUNK_WIDTH]; // will be active?
    bool ds_pulse[CHUNK_HEIGHT][CHUNK_WIDTH][CHUNK_WIDTH]; // active?
    bool ds_pulse_prev[CHUNK_HEIGHT][CHUNK_WIDTH][CHUNK_WIDTH]; // was active?

    uint8_t ds_val_next[CHUNK_HEIGHT][CHUNK_WIDTH][CHUNK_WIDTH];
    uint8_t ds_val[CHUNK_HEIGHT][CHUNK_WIDTH][CHUNK_WIDTH]; // pulse value

    vector<GLubyte> vertex_buffer;
    vector<GLfloat> texture_buffer;
    vector<GLfloat> normal_buffer;
    vector<GLushort> index_buffer;

    vector<GLfloat> l_vertex_buffer;
    vector<GLfloat> l_texture_buffer;
    vector<GLfloat> l_normal_buffer;
    vector<GLushort> l_index_buffer;

    void addFace(glm::vec<3, uint8_t> p, BlockState block, BlockFace face, Tilemap *tilemap);
    void addLogicFace(glm::vec<3, float> p, BlockState block, BlockFace face, Tilemap *tilemap, int tick);
    void addDicestoneFloor(glm::vec<3, float> p, DS ds, Tilemap *tilemap, int tick, int status);
    void addDicestoneWall(glm::vec<3, float> p, BlockFace face, Tilemap *tilemap, int tick, int status);
    DS getDicestoneOrientation(int x, int y, int z, bool wallSet[4]);
    void updateMesh(Tilemap *tilemap);
    void updateLogicMesh(Tilemap *tilemap, int tick);

    Block getNeighbor(int x, int y, int z);
    void setNeighborPulse(int x, int y, int z, uint8_t val);

public:
    Chunk(): coord(0,0) {}
    Chunk(ChunkCoord _coord): coord(_coord) {
        memset(blocks, 0, sizeof(blocks));
    }
    Chunk(ChunkCoord _coord, bool _isTouched): coord(_coord), isTouched(_isTouched) {
        memset(blocks, 0, sizeof(blocks));
    }

    void setWorld(World *w) {
        world = w;
    }

    bool canSetBlock(int x, int y, int z, Block block) {
        switch (block) {
            case Block::DICESTONE: return (z > 0 && IsCollider(blocks[z-1][y][x].type));
            default:
                return true;
        }
    }

    uint32_t blockHash(int x, int y, int z) {
        return (
            (((uint32_t)z) << 16)
            | (((uint32_t) y) << 8)
            | (((uint32_t) x) << 0)
        );
    }

    void addComputer(int x, int y, int z, Block block) {
        uint32_t hsh = blockHash(x,y,z);

        switch (block) {
            case Block::SPINNER: computers[hsh] = shared_ptr<Computer>((Computer *)(new SpinnerComputer())); break;
            case Block::TERM: computers[hsh] = shared_ptr<Computer>((Computer *)(new TerminalComputer())); break;
        }
    }

    void removeComputer(int x, int y, int z) {
        uint32_t hsh = blockHash(x,y,z);
        if (computers.count(hsh) > 0) {
            computers.erase(hsh);
        }
    }

    void resetComputers() {
        computers.clear();
        for (int x = 0; x < CHUNK_WIDTH; ++x) {
            for (int y = 0; y < CHUNK_WIDTH; ++y) {
                for (int z = 0; z < CHUNK_HEIGHT; ++z) {
                    addComputer(x,y,z,blocks[z][y][x].type);
                }
            }
        }
    }

    void interact(int x, int y, int z) {
        uint32_t hsh = blockHash(x,y,z);
        if (computers.count(hsh) > 0) {
            computers[hsh]->interact();
        }
    }

    void setBlock(int x, int y, int z, Block block, BlockDir dir, bool user = true) {
        if (canSetBlock(x, y, z, block)) {
            blocks[z][y][x] = { block, dir, 0 };

            if (block == Block::EMPTY) {
                removeComputer(x,y,z);
            } else {
                addComputer(x,y,z,block);
            }

            setDirty();
            setLogicDirty();
            if (user) {
                isTouched = true;
            }
        }
    }

    void setPulse(int x, int y, int z, uint8_t val) {
        if (!ds_pulse_prev[z][y][x]) {
            ds_pulse_next[z][y][x] = true;
            ds_val_next[z][y][x] = val;
        }
    }

    void setDirty() {
        dirty = true;
    }

    void setLogicDirty() {
        logicDirty = true;
    }

    Block getBlock(int x, int y, int z) {
        return blocks[z][y][x].type;
    }

    Block *refBlocks() {
        return (Block *)&blocks;
    }

    bool needsSave() {
        return isTouched;
    }

    void renderStatic(ChunkShader *shader, Tilemap *tilemap);
    void renderLogic(LogicShader *shader, Tilemap *tilemap, int tick);
    void tick(int currTick);
    void postTick();
};
