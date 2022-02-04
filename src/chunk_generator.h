
#include <iostream>

#include "chunk.h"
#include "perlin.h"


class ChunkGenerator {

public:
    static Chunk Generate(ChunkCoord coord) {
        Chunk ck = Chunk(coord);

        for (int x = 0; x < CHUNK_WIDTH; ++x) {
            for (int y = 0; y < CHUNK_WIDTH; ++y) {
                float fx = x + (coord.x * CHUNK_WIDTH);
                float fy = y + (coord.y * CHUNK_WIDTH);

                float fac = PerlinNoise::perlin(fx / 30.0f, fy / 30.0f);
                
                int height = (fac * 10) + 10;

                for (int z = 0; z < min(height, CHUNK_HEIGHT); ++z) {
                    ck.setBlock(x, y, z, Block::DICE, BlockDir::NORTH, false);
                }
            }
        }

        return ck;
    }

};
