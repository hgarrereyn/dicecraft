
struct World;

class Saver {
public:
    static void load(const char *filename, World *world);
    static void save(const char *filename, World *world);
};

struct SaveInfo {
    unsigned int num_chunks;
    float player_x;
    float player_y;
    float player_z;
    float player_yaw;
    float player_pitch;
};

struct ChunkInfo {
    int x;
    int y;
};
