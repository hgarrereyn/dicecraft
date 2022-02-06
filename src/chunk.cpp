
#include <iostream>

#include "opengl.h"
#include <glm/vec3.hpp>

#include "chunk.h"
#include "tilemap.h"
#include "world.h"

using namespace std;


Block Chunk::getNeighbor(int x, int y, int z) {
    if (world == nullptr) {
        return Block::EMPTY;
    }
    if (x >= 0 && x < CHUNK_WIDTH && y >= 0 && y < CHUNK_WIDTH) {
        return blocks[z][y][x].type;
    }
    return world->getBlock(x + (coord.x * CHUNK_WIDTH), y + (coord.y * CHUNK_WIDTH), z);
}

void Chunk::setNeighborPulse(int x, int y, int z, uint32_t val, BlockDir dir) {
    if (x >= 0 && x < CHUNK_WIDTH && y >= 0 && y < CHUNK_WIDTH) {
        setPulse(x, y, z, val, dir);
    }
    if (world == nullptr) return;
    world->setPulse(x + (coord.x * CHUNK_WIDTH), y + (coord.y * CHUNK_WIDTH), z, val, dir);
}

void Chunk::offsetPulse(int x, int y, int z, BlockDir dir, uint32_t val) {
    switch (dir) {
        case NORTH: setNeighborPulse(x+1, y, z, val, dir); break;
        case SOUTH: setNeighborPulse(x-1, y, z, val, dir); break;
        case EAST: setNeighborPulse(x, y-1, z, val, dir); break;
        case WEST: setNeighborPulse(x, y+1, z, val, dir); break;
    }
}

// sides:
// 0: bottom (-z)
// 1: top (+z)
// 2: left (-y)
// 3: right (+y)
// 4: front (-x)
// 5: back (+x)
void Chunk::addFace(glm::vec<3, uint8_t> p, BlockState block, BlockFace face, Tilemap *tilemap) {
    switch (face) {
        case BlockFace::BOTTOM: {
            vertex_buffer.insert(vertex_buffer.end(), {
                p.x, p.y, p.z,
                (uint8_t)(p.x+1), p.y, p.z,
                (uint8_t)(p.x+1), (uint8_t)(p.y+1), p.z,
                p.x, (uint8_t)(p.y+1), p.z
            });
            normal_buffer.insert(normal_buffer.end(), {
                0, 0, 1,
                0, 0, 1,
                0, 0, 1,
                0, 0, 1,
            });
            break;
        }
        case BlockFace::TOP: {
            vertex_buffer.insert(vertex_buffer.end(), {
                p.x, (uint8_t)(p.y+1), (uint8_t)(p.z+1),
                (uint8_t)(p.x+1), (uint8_t)(p.y+1), (uint8_t)(p.z+1),
                (uint8_t)(p.x+1), p.y, (uint8_t)(p.z+1),
                p.x, p.y, (uint8_t)(p.z+1),
            });
            normal_buffer.insert(normal_buffer.end(), {
                0, 0, -1,
                0, 0, -1,
                0, 0, -1,
                0, 0, -1,
            });
            break;
        }
        case BlockFace::LEFT: {
            vertex_buffer.insert(vertex_buffer.end(), {
                (uint8_t)(p.x+1), (uint8_t)(p.y+1), (uint8_t)(p.z+1),
                p.x, (uint8_t)(p.y+1), (uint8_t)(p.z+1),
                p.x, (uint8_t)(p.y+1), p.z,
                (uint8_t)(p.x+1), (uint8_t)(p.y+1), p.z,
            });
            normal_buffer.insert(normal_buffer.end(), {
                0, -1, 0,
                0, -1, 0,
                0, -1, 0,
                0, -1, 0,
            });
            break;
        }
        case BlockFace::RIGHT: {
            vertex_buffer.insert(vertex_buffer.end(), {
                p.x, p.y, (uint8_t)(p.z+1),
                (uint8_t)(p.x+1), p.y, (uint8_t)(p.z+1),
                (uint8_t)(p.x+1), p.y, p.z,
                p.x, p.y, p.z,
            });
            normal_buffer.insert(normal_buffer.end(), {
                0, 1, 0,
                0, 1, 0,
                0, 1, 0,
                0, 1, 0,
            });
            break;
        }
        case BlockFace::FRONT: {
            vertex_buffer.insert(vertex_buffer.end(), {
                p.x, (uint8_t)(p.y+1), (uint8_t)(p.z+1),
                p.x, p.y, (uint8_t)(p.z+1),
                p.x, p.y, p.z,
                p.x, (uint8_t)(p.y+1), p.z,
            });
            normal_buffer.insert(normal_buffer.end(), {
                -1, 0, 0,
                -1, 0, 0,
                -1, 0, 0,
                -1, 0, 0,
            });
            break;
        }
        case BlockFace::BACK: {
            vertex_buffer.insert(vertex_buffer.end(), {
                (uint8_t)(p.x+1), p.y, (uint8_t)(p.z+1),
                (uint8_t)(p.x+1), (uint8_t)(p.y+1), (uint8_t)(p.z+1),
                (uint8_t)(p.x+1), (uint8_t)(p.y+1), p.z,
                (uint8_t)(p.x+1), p.y, p.z,
            });
            normal_buffer.insert(normal_buffer.end(), {
                1, 0, 0,
                1, 0, 0,
                1, 0, 0,
                1, 0, 0,
            });
            break;
        }
    }

    glm::vec2 uv = tilemap->getCoords(block, face);
    texture_buffer.insert(texture_buffer.end(), {
        uv.x, uv.y,
        uv.x + UV_GAP, uv.y,
        uv.x + UV_GAP, uv.y + UV_GAP,
        uv.x, uv.y + UV_GAP
    });

    GLushort off = (GLushort)(vertex_buffer.size() / 3) - 4;
    index_buffer.insert(index_buffer.end(), {
        off, (GLushort)(off+1), (GLushort)(off+2),
        (GLushort)(off+2), (GLushort)(off+3), off,
    });
}

void Chunk::addLogicFace(glm::vec<3, float> p, BlockState block, BlockFace face, Tilemap *tilemap, int tick) {
    switch (face) {
        case BlockFace::BOTTOM: {
            l_vertex_buffer.insert(l_vertex_buffer.end(), {
                p.x, p.y, p.z,
                (p.x+1), p.y, p.z,
                (p.x+1), (p.y+1), p.z,
                p.x, (p.y+1), p.z
            });
            l_normal_buffer.insert(l_normal_buffer.end(), {
                0, 0, 1,
                0, 0, 1,
                0, 0, 1,
                0, 0, 1,
            });
            break;
        }
        case BlockFace::TOP: {
            l_vertex_buffer.insert(l_vertex_buffer.end(), {
                p.x, (p.y+1), (p.z+1),
                (p.x+1), (p.y+1), (p.z+1),
                (p.x+1), p.y, (p.z+1),
                p.x, p.y, (p.z+1),
            });
            l_normal_buffer.insert(l_normal_buffer.end(), {
                0, 0, -1,
                0, 0, -1,
                0, 0, -1,
                0, 0, -1,
            });
            break;
        }
        case BlockFace::LEFT: {
            l_vertex_buffer.insert(l_vertex_buffer.end(), {
                (p.x+1), (p.y+1), (p.z+1),
                p.x, (p.y+1), (p.z+1),
                p.x, (p.y+1), p.z,
                (p.x+1), (p.y+1), p.z,
            });
            l_normal_buffer.insert(l_normal_buffer.end(), {
                0, -1, 0,
                0, -1, 0,
                0, -1, 0,
                0, -1, 0,
            });
            break;
        }
        case BlockFace::RIGHT: {
            l_vertex_buffer.insert(l_vertex_buffer.end(), {
                p.x, p.y, (p.z+1),
                (p.x+1), p.y, (p.z+1),
                (p.x+1), p.y, p.z,
                p.x, p.y, p.z,
            });
            l_normal_buffer.insert(l_normal_buffer.end(), {
                0, 1, 0,
                0, 1, 0,
                0, 1, 0,
                0, 1, 0,
            });
            break;
        }
        case BlockFace::FRONT: {
            l_vertex_buffer.insert(l_vertex_buffer.end(), {
                p.x, (p.y+1), (p.z+1),
                p.x, p.y, (p.z+1),
                p.x, p.y, p.z,
                p.x, (p.y+1), p.z,
            });
            l_normal_buffer.insert(l_normal_buffer.end(), {
                -1, 0, 0,
                -1, 0, 0,
                -1, 0, 0,
                -1, 0, 0,
            });
            break;
        }
        case BlockFace::BACK: {
            l_vertex_buffer.insert(l_vertex_buffer.end(), {
                (p.x+1), p.y, (p.z+1),
                (p.x+1), (p.y+1), (p.z+1),
                (p.x+1), (p.y+1), p.z,
                (p.x+1), p.y, p.z,
            });
            l_normal_buffer.insert(l_normal_buffer.end(), {
                1, 0, 0,
                1, 0, 0,
                1, 0, 0,
                1, 0, 0,
            });
            break;
        }
    }

    glm::vec2 uv = tilemap->getCoords(block, face, tick);
    l_texture_buffer.insert(l_texture_buffer.end(), {
        uv.x, uv.y,
        uv.x + UV_GAP, uv.y,
        uv.x + UV_GAP, uv.y + UV_GAP,
        uv.x, uv.y + UV_GAP
    });

    GLushort off = (GLushort)(l_vertex_buffer.size() / 3) - 4;
    l_index_buffer.insert(l_index_buffer.end(), {
        off, (GLushort)(off+1), (GLushort)(off+2),
        (GLushort)(off+2), (GLushort)(off+3), off,
    });
}

void Chunk::addDicestoneFloor(glm::vec<3, float> p, DS ds, Tilemap *tilemap, int tick, int status) {
    int rot = 0;
    glm::vec2 uv = tilemap->getDicestoneCoords(ds, &rot, status);

    switch (rot) {
        case 0: {
            l_vertex_buffer.insert(l_vertex_buffer.end(), {
                p.x, (p.y+1), (p.z+1),
                (p.x+1), (p.y+1), (p.z+1),
                (p.x+1), p.y, (p.z+1),
                p.x, p.y, (p.z+1),
            });
            break;
        }
        case 1: {
            l_vertex_buffer.insert(l_vertex_buffer.end(), {
                p.x, p.y, (p.z+1),
                p.x, (p.y+1), (p.z+1),
                (p.x+1), (p.y+1), (p.z+1),
                (p.x+1), p.y, (p.z+1),
            });
            break;
        }
        case 2: {
            l_vertex_buffer.insert(l_vertex_buffer.end(), {
                (p.x+1), p.y, (p.z+1),
                p.x, p.y, (p.z+1),
                p.x, (p.y+1), (p.z+1),
                (p.x+1), (p.y+1), (p.z+1),
            });
            break;
        }
        case 3: {
            l_vertex_buffer.insert(l_vertex_buffer.end(), {
                (p.x+1), (p.y+1), (p.z+1),
                (p.x+1), p.y, (p.z+1),
                p.x, p.y, (p.z+1),
                p.x, (p.y+1), (p.z+1),
            });
            break;
        }
    }

    l_normal_buffer.insert(l_normal_buffer.end(), {
        0, 0, -1,
        0, 0, -1,
        0, 0, -1,
        0, 0, -1,
    });

    l_texture_buffer.insert(l_texture_buffer.end(), {
        uv.x, uv.y,
        uv.x + UV_GAP, uv.y,
        uv.x + UV_GAP, uv.y + UV_GAP,
        uv.x, uv.y + UV_GAP
    });

    GLushort off = (GLushort)(l_vertex_buffer.size() / 3) - 4;
    l_index_buffer.insert(l_index_buffer.end(), {
        off, (GLushort)(off+1), (GLushort)(off+2),
        (GLushort)(off+2), (GLushort)(off+3), off,
    });
}

void Chunk::addDicestoneWall(glm::vec<3, float> p, BlockFace face, Tilemap *tilemap, int tick, int status) {
    switch (face) {
        case BlockFace::LEFT: {
            l_vertex_buffer.insert(l_vertex_buffer.end(), {
                (p.x+1), (p.y+1), p.z,
                (p.x+1), (p.y+1), (p.z+1),
                p.x, (p.y+1), (p.z+1),
                p.x, (p.y+1), p.z,
            });
            l_normal_buffer.insert(l_normal_buffer.end(), {
                0, -1, 0,
                0, -1, 0,
                0, -1, 0,
                0, -1, 0,
            });
            break;
        }
        case BlockFace::RIGHT: {
            l_vertex_buffer.insert(l_vertex_buffer.end(), {
                p.x, p.y, p.z,
                p.x, p.y, (p.z+1),
                (p.x+1), p.y, (p.z+1),
                (p.x+1), p.y, p.z,
            });
            l_normal_buffer.insert(l_normal_buffer.end(), {
                0, 1, 0,
                0, 1, 0,
                0, 1, 0,
                0, 1, 0,
            });
            break;
        }
        case BlockFace::FRONT: {
            l_vertex_buffer.insert(l_vertex_buffer.end(), {
                p.x, (p.y+1), p.z,
                p.x, (p.y+1), (p.z+1),
                p.x, p.y, (p.z+1),
                p.x, p.y, p.z,
            });
            l_normal_buffer.insert(l_normal_buffer.end(), {
                -1, 0, 0,
                -1, 0, 0,
                -1, 0, 0,
                -1, 0, 0,
            });
            break;
        }
        case BlockFace::BACK: {
            l_vertex_buffer.insert(l_vertex_buffer.end(), {
                (p.x+1), p.y, p.z,
                (p.x+1), p.y, (p.z+1),
                (p.x+1), (p.y+1), (p.z+1),
                (p.x+1), (p.y+1), p.z,
            });
            l_normal_buffer.insert(l_normal_buffer.end(), {
                1, 0, 0,
                1, 0, 0,
                1, 0, 0,
                1, 0, 0,
            });
            break;
        }
    }

    int rot;
    glm::vec2 uv = tilemap->getDicestoneCoords(_DS_NS, &rot, status);

    l_texture_buffer.insert(l_texture_buffer.end(), {
        uv.x, uv.y,
        uv.x + UV_GAP, uv.y,
        uv.x + UV_GAP, uv.y + UV_GAP,
        uv.x, uv.y + UV_GAP
    });

    GLushort off = (GLushort)(l_vertex_buffer.size() / 3) - 4;
    l_index_buffer.insert(l_index_buffer.end(), {
        off, (GLushort)(off+1), (GLushort)(off+2),
        (GLushort)(off+2), (GLushort)(off+3), off,
    });
}

#define BLOCK_COND(curr, target) ((IsTransparent(curr) && (target == Block::EMPTY)) | (!IsTransparent(curr) && IsTransparent(target)))

void Chunk::updateMesh(Tilemap *tilemap) {
    vertex_buffer.clear();
    normal_buffer.clear();
    index_buffer.clear();
    texture_buffer.clear();

    for (int z = 0; z < CHUNK_HEIGHT; ++z) {
        for (int y = 0; y < CHUNK_WIDTH; ++y) {
            for (int x = 0; x < CHUNK_WIDTH; ++x) {

                glm::vec<3, uint8_t> p(
                    (uint8_t)x,
                    (uint8_t)y,
                    (uint8_t)z
                );
                
                BlockState block = blocks[z][y][x];
                if (block.type == Block::EMPTY || IsLogical(block.type)) {
                    continue;
                }

                if (z == 0 || BLOCK_COND(block.type, blocks[z-1][y][x].type)) {
                    addFace(p, block, BlockFace::BOTTOM, tilemap);
                }
                if (z == CHUNK_HEIGHT-1 || BLOCK_COND(block.type, blocks[z+1][y][x].type)) {
                    addFace(p, block, BlockFace::TOP, tilemap);
                }
                if (((y == 0) && BLOCK_COND(block.type, getNeighbor(x, y-1, z))) || ((y != 0) && BLOCK_COND(block.type, blocks[z][y-1][x].type))) {
                    addFace(p, block, BlockFace::RIGHT, tilemap);
                }
                if (((y == CHUNK_WIDTH - 1) && BLOCK_COND(block.type, getNeighbor(x, y+1, z))) || ((y != CHUNK_WIDTH - 1) && BLOCK_COND(block.type, blocks[z][y+1][x].type))) {
                    addFace(p, block, BlockFace::LEFT, tilemap);
                }
                if (((x == 0) && BLOCK_COND(block.type, getNeighbor(x-1, y, z))) || ((x != 0) && BLOCK_COND(block.type, blocks[z][y][x-1].type))) {
                    addFace(p, block, BlockFace::FRONT, tilemap);
                }
                if (((x == CHUNK_WIDTH - 1) && BLOCK_COND(block.type, getNeighbor(x+1, y, z))) || ((x != CHUNK_WIDTH - 1) && BLOCK_COND(block.type, blocks[z][y][x+1].type))) {
                    addFace(p, block, BlockFace::BACK, tilemap);
                }
            }
        }
    }
}

DS Chunk::getDicestoneOrientation(int x, int y, int z, bool wallSet[4]) {
    uint8_t nb = 0;
    if (IsLogicConnector(getNeighbor(x+1, y, z))) nb |= 0b0001;
    if (IsLogicConnector(getNeighbor(x-1, y, z))) nb |= 0b0010;
    if (IsLogicConnector(getNeighbor(x, y+1, z))) nb |= 0b0100;
    if (IsLogicConnector(getNeighbor(x, y-1, z))) nb |= 0b1000;

    if (IsLogicConnector(getNeighbor(x+1, y, z-1)) && !IsCollider(getNeighbor(x+1, y, z))) { nb |= 0b0001; wallSet[0] = true; }
    if (IsLogicConnector(getNeighbor(x-1, y, z-1)) && !IsCollider(getNeighbor(x-1, y, z))) { nb |= 0b0010; wallSet[1] = true; }
    if (IsLogicConnector(getNeighbor(x, y+1, z-1)) && !IsCollider(getNeighbor(x, y+1, z))) { nb |= 0b0100; wallSet[2] = true; }
    if (IsLogicConnector(getNeighbor(x, y-1, z-1)) && !IsCollider(getNeighbor(x, y-1, z))) { nb |= 0b1000; wallSet[3] = true; }

    if (!IsCollider(getNeighbor(x, y, z+1))) {
        if (IsLogicConnector(getNeighbor(x+1, y, z+1))) nb |= 0b0001;
        if (IsLogicConnector(getNeighbor(x-1, y, z+1))) nb |= 0b0010;
        if (IsLogicConnector(getNeighbor(x, y+1, z+1))) nb |= 0b0100;
        if (IsLogicConnector(getNeighbor(x, y-1, z+1))) nb |= 0b1000;
    }

    switch (nb) {
        case 0b0000: return _DS;
        case 0b0001: return _DS_N;
        case 0b0010: return _DS_S;
        case 0b0011: return _DS_NS;
        case 0b0100: return _DS_E;
        case 0b0101: return _DS_NE;
        case 0b0110: return _DS_SE;
        case 0b0111: return _DS_NSE;
        case 0b1000: return _DS_W;
        case 0b1001: return _DS_NW;
        case 0b1010: return _DS_SW;
        case 0b1011: return _DS_NSW;
        case 0b1100: return _DS_EW;
        case 0b1101: return _DS_NEW;
        case 0b1110: return _DS_SEW;
        case 0b1111: return _DS_NSEW;
    }
}

void Chunk::updateLogicMesh(Tilemap *tilemap, int tick) {
    l_vertex_buffer.clear();
    l_normal_buffer.clear();
    l_index_buffer.clear();
    l_texture_buffer.clear();

    for (int z = 0; z < CHUNK_HEIGHT; ++z) {
        for (int y = 0; y < CHUNK_WIDTH; ++y) {
            for (int x = 0; x < CHUNK_WIDTH; ++x) {

                glm::vec<3, float> p(
                    (float)x,
                    (float)y,
                    (float)z
                );
                
                BlockState block = blocks[z][y][x];
                if (block.type == Block::EMPTY || !IsLogical(block.type)) {
                    continue;
                }

                if (block.type == Block::DICESTONE) {
                    bool is_active = ds_pulse[z][y][x];
                    bool was_active = ds_pulse_prev[z][y][x];

                    int status = (was_active ? 2 : (is_active ? 1 : 0));

                    bool wallSet[4] = {0};
                    DS ds = getDicestoneOrientation(x, y, z, wallSet);
                    addDicestoneFloor(p - glm::vec3(0, 0, 15/16.0f), ds, tilemap, tick, status);

                    if (wallSet[0]) addDicestoneWall(p + glm::vec3(1/16.0f, 0, -1), BlockFace::BACK, tilemap, tick, status);
                    if (wallSet[1]) addDicestoneWall(p + glm::vec3(-1/16.0f, 0, -1), BlockFace::FRONT, tilemap, tick, status);
                    if (wallSet[2]) addDicestoneWall(p + glm::vec3(0, 1/16.0f, -1), BlockFace::LEFT, tilemap, tick, status);
                    if (wallSet[3]) addDicestoneWall(p + glm::vec3(0, -1/16.0f, -1), BlockFace::RIGHT, tilemap, tick, status);
                } else {
                    if (z == 0 || BLOCK_COND(block.type, blocks[z-1][y][x].type)) {
                        addLogicFace(p, block, BlockFace::BOTTOM, tilemap, tick);
                    }
                    if (z == CHUNK_HEIGHT-1 || BLOCK_COND(block.type, blocks[z+1][y][x].type)) {
                        addLogicFace(p, block, BlockFace::TOP, tilemap, tick);
                    }
                    if (((y == 0) && BLOCK_COND(block.type, getNeighbor(x, y-1, z))) || ((y != 0) && BLOCK_COND(block.type, blocks[z][y-1][x].type))) {
                        addLogicFace(p, block, BlockFace::RIGHT, tilemap, tick);
                    }
                    if (((y == CHUNK_WIDTH - 1) && BLOCK_COND(block.type, getNeighbor(x, y+1, z))) || ((y != CHUNK_WIDTH - 1) && BLOCK_COND(block.type, blocks[z][y+1][x].type))) {
                        addLogicFace(p, block, BlockFace::LEFT, tilemap, tick);
                    }
                    if (((x == 0) && BLOCK_COND(block.type, getNeighbor(x-1, y, z))) || ((x != 0) && BLOCK_COND(block.type, blocks[z][y][x-1].type))) {
                        addLogicFace(p, block, BlockFace::FRONT, tilemap, tick);
                    }
                    if (((x == CHUNK_WIDTH - 1) && BLOCK_COND(block.type, getNeighbor(x+1, y, z))) || ((x != CHUNK_WIDTH - 1) && BLOCK_COND(block.type, blocks[z][y][x+1].type))) {
                        addLogicFace(p, block, BlockFace::BACK, tilemap, tick);
                    }
                }
            }
        }
    }
}

void Chunk::renderStatic(ChunkShader *shader, Tilemap *tilemap) {
    if (dirty) {
        updateMesh(tilemap);
        dirty = false;
    }

    shader->writeVertexData(vertex_buffer);
    shader->writeTextureData(texture_buffer);
    shader->writeNormalData(normal_buffer);
    shader->writeIndexData(index_buffer);

    glUniform3f(shader->chunkOffset, coord.x * CHUNK_WIDTH, coord.y * CHUNK_WIDTH, 0);

    shader->use();

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, shader->vertex_buffer);
    glVertexAttribPointer(shader->vertexPos, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(GLubyte) * 3, (void *)0);

    glBindBuffer(GL_ARRAY_BUFFER, shader->texture_buffer);
    glVertexAttribPointer(shader->vertexTex, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, (void *)0);
    
    glBindBuffer(GL_ARRAY_BUFFER, shader->normal_buffer);
    glVertexAttribPointer(shader->vertexNorm, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void *)0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tilemap->texture);
    glUniform1i(shader->tex, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shader->index_buffer);
    glDrawElements(GL_TRIANGLES, index_buffer.size(), GL_UNSIGNED_SHORT, (void *)0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void Chunk::renderLogic(LogicShader *shader, Tilemap *tilemap, int tick) {
    if (logicDirty) {
        updateLogicMesh(tilemap, tick);
        logicDirty = false;
    }

    shader->writeVertexData(l_vertex_buffer);
    shader->writeTextureData(l_texture_buffer);
    shader->writeNormalData(l_normal_buffer);
    shader->writeIndexData(l_index_buffer);

    glUniform3f(shader->chunkOffset, coord.x * CHUNK_WIDTH, coord.y * CHUNK_WIDTH, 0);

    shader->use();

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, shader->vertex_buffer);
    glVertexAttribPointer(shader->vertexPos, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void *)0);

    glBindBuffer(GL_ARRAY_BUFFER, shader->texture_buffer);
    glVertexAttribPointer(shader->vertexTex, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, (void *)0);
    
    glBindBuffer(GL_ARRAY_BUFFER, shader->normal_buffer);
    glVertexAttribPointer(shader->vertexNorm, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void *)0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tilemap->texture);
    glUniform1i(shader->tex, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shader->index_buffer);
    glDrawElements(GL_TRIANGLES, l_index_buffer.size(), GL_UNSIGNED_SHORT, (void *)0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void Chunk::tick(int currTick) {
    setLogicDirty();

    auto it = computers.begin();
    while (it != computers.end()) {
        int x = (it->first) & 0xff;
        int y = ((it->first) >> 8) & 0xff;
        int z = ((it->first) >> 16) & 0xff;

        it->second->tick(currTick);
        blocks[z][y][x].state = it->second->getState();

        bool pulse[4];
        uint32_t val[4];
        it->second->getOutput(pulse, val);

        if (pulse[S_FRONT]) offsetPulse(x, y, z, RelativeToGlobal(blocks[z][y][x].dir, S_FRONT), val[S_FRONT]);
        if (pulse[S_BACK]) offsetPulse(x, y, z, RelativeToGlobal(blocks[z][y][x].dir, S_BACK), val[S_BACK]);
        if (pulse[S_LEFT]) offsetPulse(x, y, z, RelativeToGlobal(blocks[z][y][x].dir, S_LEFT), val[S_LEFT]);
        if (pulse[S_RIGHT]) offsetPulse(x, y, z, RelativeToGlobal(blocks[z][y][x].dir, S_RIGHT), val[S_RIGHT]);

        it++;
    }

    // Dicestone updates.
    for (int x = 0; x < CHUNK_WIDTH; ++x) {
        for (int y = 0; y < CHUNK_WIDTH; ++y) {
            for (int z = 0; z < CHUNK_HEIGHT; ++z) {

                BlockState block = blocks[z][y][x];
                if (block.type != DICESTONE) continue;

                uint32_t val = ds_val[z][y][x];
                if (ds_pulse[z][y][x]) {
                    setNeighborPulse(x+1, y, z, val, BlockDir::NORTH);
                    setNeighborPulse(x-1, y, z, val, BlockDir::SOUTH);
                    setNeighborPulse(x, y+1, z, val, BlockDir::WEST);
                    setNeighborPulse(x, y-1, z, val, BlockDir::EAST);

                    if (blocks[z+1][y][x].type == Block::EMPTY) {
                        setNeighborPulse(x+1, y, z+1, val, BlockDir::NORTH);
                        setNeighborPulse(x-1, y, z+1, val, BlockDir::SOUTH);
                        setNeighborPulse(x, y+1, z+1, val, BlockDir::WEST);
                        setNeighborPulse(x, y-1, z+1, val, BlockDir::EAST);
                    }

                    if (z > 0) {
                        if (getNeighbor(x+1,y,z) == Block::EMPTY) setNeighborPulse(x+1, y, z-1, val, BlockDir::NORTH);
                        if (getNeighbor(x-1,y,z) == Block::EMPTY) setNeighborPulse(x-1, y, z-1, val, BlockDir::SOUTH);
                        if (getNeighbor(x,y+1,z) == Block::EMPTY) setNeighborPulse(x, y+1, z-1, val, BlockDir::WEST);
                        if (getNeighbor(x,y-1,z) == Block::EMPTY) setNeighborPulse(x, y-1, z-1, val, BlockDir::EAST);
                    }
                }

            }
        }
    }
}

void Chunk::postTick() {
    // Cooldown propogation.
    memcpy(ds_pulse_prev, ds_pulse, sizeof(ds_pulse));
    memcpy(ds_pulse, ds_pulse_next, sizeof(ds_pulse));
    memset(ds_pulse_next, 0, sizeof(ds_pulse_next));

    memcpy(ds_val, ds_val_next, sizeof(ds_val));
    memset(ds_val_next, 0, sizeof(ds_val_next));
}
