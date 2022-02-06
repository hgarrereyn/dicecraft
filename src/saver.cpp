

#include "saver.h"

#include "world.h"
#include "block.h"
#include "chunk.h"

void Saver::save(const char *filename, World *world) {
    FILE *f = fopen(filename, "wb");

    uint32_t num = 0;
    auto it = world->chunks.begin();
    while (it != world->chunks.end()) {
        if (it->second.needsSave()) {
            num++;
        }
        it++;
    }

    SaveInfo info = {
        /* num_chunks */ num,
        /* player_x */ world->player.pos.x,
        /* player_y */ world->player.pos.y,
        /* player_z */ world->player.pos.z,
        /* player_yaw */ world->player.dir.x,
        /* player_pitch */ world->player.dir.y,
    };
    fwrite(&info, sizeof(info), 1, f);

    it = world->chunks.begin();
    while (it != world->chunks.end()) {
        if (it->second.needsSave()) {
            ChunkInfo chunk_info = {
                /* x */ it->first.x,
                /* y */ it->first.y
            };
            fwrite(&chunk_info, sizeof(chunk_info), 1, f);
            fwrite(it->second.refBlocks(), sizeof(BlockState), CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT, f);
        }
        it++;
    }

    fclose(f);
}

void Saver::load(const char *filename, World *world) {
    FILE *f = fopen(filename, "rb");

    SaveInfo info;
    fread(&info, sizeof(info), 1, f);

    world->player.pos = glm::vec3(
        info.player_x,
        info.player_y,
        info.player_z
    );

    world->player.dir = glm::vec2(
        info.player_yaw,
        info.player_pitch
    );

    world->player.updateTarget();

    for (int i = 0; i < info.num_chunks; ++i) {
        ChunkInfo chunk_info;
        fread(&chunk_info, sizeof(chunk_info), 1, f);

        ChunkCoord coord(chunk_info.x, chunk_info.y);

        Chunk ck(coord, true);
        ck.setDirty();
        fread(ck.refBlocks(), sizeof(BlockState), CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT, f);
        ck.resetComputers();
        world->setChunk(coord, ck);
    }

    fclose(f);
}
