
#pragma once

#include <vector>
#include <iostream>

#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/common.hpp>

#include "lodepng.h"
#include "block.h"

using namespace std;

#define TILE_WIDTH (float)16

#define UV_GAP 1/TILE_WIDTH
#define TILE(x) glm::vec2(remainderf((x), TILE_WIDTH) / TILE_WIDTH, (int)((x) / TILE_WIDTH) / TILE_WIDTH)

#define DSTONE(x,y) *rot = y; return TILE(x + (status == 0 ? 0 : (status == 1 ? 16 : 32)))

class Tilemap {

    void loadTex(unsigned char *image, unsigned int width, unsigned int height) {
        glGenTextures(1, &texture);

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
public:
    GLuint texture;

    Tilemap(const char *filename) {
        vector<unsigned char> image;
        unsigned int width, height;
        unsigned int err = lodepng::decode(image, width, height, filename, LodePNGColorType::LCT_RGBA);
        if (err != 0) {
            cerr << "Error loading tilemap: " << lodepng_error_text(err) << endl;
            exit(-1);
        }

        cout << "Got tilemap: " << width << " x " << height << endl;
        loadTex(image.data(), width, height);
    }

    Tilemap(unsigned char *image, unsigned int width, unsigned int height) {
        loadTex(image, width, height);
    }

    glm::vec2 getCoords(BlockState block, BlockFace worldFace, int tick = 0) {
        BlockFace face;
        switch (worldFace) {
            case TOP: face = TOP; break;
            case BOTTOM: face = BOTTOM; break;
            case BlockFace::FRONT: switch (block.dir) {
                case NORTH: face = BlockFace::BACK; break;
                case SOUTH: face = BlockFace::FRONT; break;
                case EAST: face = BlockFace::LEFT; break;
                case WEST: face = BlockFace::RIGHT; break;
            }
            break;
            case BlockFace::BACK: switch (block.dir) {
                case NORTH: face = BlockFace::FRONT; break;
                case SOUTH: face = BlockFace::BACK; break;
                case EAST: face = BlockFace::RIGHT; break;
                case WEST: face = BlockFace::LEFT; break;
            }
            break;
            case BlockFace::LEFT: switch (block.dir) {
                case NORTH: face = BlockFace::LEFT; break;
                case SOUTH: face = BlockFace::RIGHT; break;
                case EAST: face = BlockFace::BACK; break;
                case WEST: face = BlockFace::FRONT; break;
            }
            break;
            case BlockFace::RIGHT: switch (block.dir) {
                case NORTH: face = BlockFace::RIGHT; break;
                case SOUTH: face = BlockFace::LEFT; break;
                case EAST: face = BlockFace::FRONT; break;
                case WEST: face = BlockFace::BACK; break;
            }
            break;
        }

        switch (block.type) {
            case Block::DICE: switch (face) {
                case BlockFace::TOP: return TILE(0);
                case BlockFace::FRONT: return TILE(1);
                case BlockFace::RIGHT: return TILE(2);
                case BlockFace::BOTTOM: return TILE(3);
                case BlockFace::BACK: return TILE(4);
                case BlockFace::LEFT: return TILE(5);
            }
            case Block::METAL: return TILE(6);
            case Block::GLASS: return TILE(7);

            case Block::SPINNER: switch (face) {
                case BlockFace::TOP: return TILE(21);
                case BlockFace::FRONT: return TILE(32 + block.state);
                case BlockFace::RIGHT: switch (tick % 2) {
                    case 0: return TILE(17);
                    case 1: return TILE(18);
                }
                case BlockFace::BOTTOM: return TILE(21);
                case BlockFace::BACK: return TILE(20);
                case BlockFace::LEFT: return TILE(19);
            }

            case Block::BUMPER: switch (face) {
                case BlockFace::TOP: return TILE(48 + (tick % 4));
                case BlockFace::FRONT: return TILE(19);
                case BlockFace::RIGHT: switch (tick % 2) {
                    case 0: return TILE(17);
                    case 1: return TILE(18);
                }
                case BlockFace::BOTTOM: return TILE(21);
                case BlockFace::BACK: return TILE(20);
                case BlockFace::LEFT: return TILE(19);
            }

            case Block::TERM: switch (face) {
                case BlockFace::TOP: return TILE(21);
                case BlockFace::FRONT: return TILE((64 + (block.state % 16)));
                case BlockFace::RIGHT: switch (tick % 2) {
                    case 0: return TILE(17);
                    case 1: return TILE(18);
                }
                case BlockFace::BOTTOM: return TILE(21);
                case BlockFace::BACK: return TILE(20);
                case BlockFace::LEFT: return TILE(19);
            }

            case Block::BUTTON: switch (face) {
                case BlockFace::TOP: return TILE(21);
                case BlockFace::FRONT: return TILE((80 + (block.state % 2)));
                case BlockFace::RIGHT: switch (tick % 2) {
                    case 0: return TILE(17);
                    case 1: return TILE(18);
                }
                case BlockFace::BOTTOM: return TILE(21);
                case BlockFace::BACK: return TILE(20);
                case BlockFace::LEFT: return TILE(19);
            }

            case Block::FLAG: switch (face) {
                case BlockFace::TOP: return TILE(21);
                case BlockFace::FRONT: return TILE((37 + (block.state % 2)));
                case BlockFace::RIGHT: switch (tick % 2) {
                    case 0: return TILE(17);
                    case 1: return TILE(18);
                }
                case BlockFace::BOTTOM: return TILE(21);
                case BlockFace::BACK: return TILE(20);
                case BlockFace::LEFT: return TILE(19);
            }

            case Block::LETTERS: switch (face) {
                case BlockFace::TOP: return TILE(21);
                case BlockFace::FRONT: return TILE((112 + (block.state % 8)));
                case BlockFace::RIGHT: switch (tick % 2) {
                    case 0: return TILE(17);
                    case 1: return TILE(18);
                }
                case BlockFace::BOTTOM: return TILE(21);
                case BlockFace::BACK: return TILE(20);
                case BlockFace::LEFT: return TILE(19);
            }

            case Block::SIGNAL: switch (face) {
                case BlockFace::TOP: return TILE(21);
                case BlockFace::FRONT: return TILE((82 + (block.state % 2)));
                case BlockFace::RIGHT: switch (tick % 2) {
                    case 0: return TILE(17);
                    case 1: return TILE(18);
                }
                case BlockFace::BOTTOM: return TILE(21);
                case BlockFace::BACK: return TILE(20);
                case BlockFace::LEFT: return TILE(19);
            }

            case Block::LOCK: switch (face) {
                case BlockFace::TOP: return TILE(21);
                case BlockFace::FRONT: return TILE((84 + (block.state % 2)));
                case BlockFace::RIGHT: switch (tick % 2) {
                    case 0: return TILE(17);
                    case 1: return TILE(18);
                }
                case BlockFace::BOTTOM: return TILE(21);
                case BlockFace::BACK: return TILE(20);
                case BlockFace::LEFT: return TILE(19);
            }

            case Block::SLOT: switch (face) {
                case BlockFace::TOP: return TILE(21);
                case BlockFace::FRONT: return TILE((96 + (block.state % 2)));
                case BlockFace::RIGHT: switch (tick % 2) {
                    case 0: return TILE(17);
                    case 1: return TILE(18);
                }
                case BlockFace::BOTTOM: return TILE(21);
                case BlockFace::BACK: return TILE(20);
                case BlockFace::LEFT: return TILE(19);
            }

            case Block::MUT: switch (face) {
                case BlockFace::TOP: return TILE(21);
                case BlockFace::FRONT: return TILE((52 + (block.state % 2)));
                case BlockFace::RIGHT: switch (tick % 2) {
                    case 0: return TILE(17);
                    case 1: return TILE(18);
                }
                case BlockFace::BOTTOM: return TILE(21);
                case BlockFace::BACK: return TILE(20);
                case BlockFace::LEFT: return TILE(19);
            }

            case Block::JUNCTION: switch (face) {
                case BlockFace::TOP: return TILE(21);
                case BlockFace::FRONT: return TILE((54 + (block.state % 2)));
                case BlockFace::RIGHT: switch (tick % 2) {
                    case 0: return TILE(17);
                    case 1: return TILE(18);
                }
                case BlockFace::BOTTOM: return TILE(21);
                case BlockFace::BACK: return TILE(20);
                case BlockFace::LEFT: return TILE(19);
            }

            case Block::SHIFTER: switch (face) {
                case BlockFace::TOP: return TILE(21);
                case BlockFace::FRONT: return TILE((98 + (block.state % 2)));
                case BlockFace::RIGHT: switch (tick % 2) {
                    case 0: return TILE(17);
                    case 1: return TILE(18);
                }
                case BlockFace::BOTTOM: return TILE(21);
                case BlockFace::BACK: return TILE(20);
                case BlockFace::LEFT: return TILE(19);
            }

            default: return TILE(255);
        }
    }

    glm::vec2 getDicestoneCoords(DS ds, int *rot, int status) {
        switch (ds) {
            case _DS: DSTONE(8, 0);

            case _DS_N: DSTONE(13, 0);
            case _DS_S: DSTONE(13, 2);
            case _DS_E: DSTONE(13, 1);
            case _DS_W: DSTONE(13, 3);

            case _DS_NE: DSTONE(9, 1);
            case _DS_SE: DSTONE(9, 2);
            case _DS_SW: DSTONE(9, 3);
            case _DS_NW: DSTONE(9, 0);

            case _DS_NS: DSTONE(10, 0);
            case _DS_EW: DSTONE(10, 1);

            case _DS_NSE: DSTONE(11, 3);
            case _DS_NSW: DSTONE(11, 1);
            case _DS_NEW: DSTONE(11, 2);
            case _DS_SEW: DSTONE(11, 0);

            case _DS_NSEW: DSTONE(12, 0);

            default: DSTONE(255, 0);
        }
    }
};
