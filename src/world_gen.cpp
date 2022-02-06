

#include "world.h"
#include "saver.h"


// const char *flag = "dice{m1n3cr4ft_w1th_c0mput3rs_:)}";
// int flag_len = 8;

#define SUB_SIZE 8


void build_sub(World *w, int ox, int oy, int initial_state[8], int flag_state[8], bool solved) {
    
    w->setBlock(ox-1, oy, 12, DICESTONE);
    w->setBlock(ox-1, oy+1, 12, METAL);
    w->setBlock(ox-1, oy+1, 13, DICESTONE);
    w->setBlock(ox-1, oy+2, 13, METAL);
    w->setBlock(ox-1, oy+2, 14, DICESTONE);

    for (int i = 0; i < SUB_SIZE; ++i) {
        int x = ox + (i * 3);
        int y = oy;
        int z = 12;

        w->setBlock(x, y, z, DICESTONE);
        w->setBlock(x+1, y, z, TERM, EAST, solved ? flag_state[i] : 0);
        w->setBlock(x+2, y, z, DICESTONE);

        w->setBlock(x, y+1, z, GLASS);
        w->setBlock(x+1, y+1, z, DICESTONE);
        w->setBlock(x+2, y+1, z, GLASS);
        w->setBlock(x, y+1, z+1, GLASS);
        w->setBlock(x+1, y+1, z+1, GLASS);
        w->setBlock(x+2, y+1, z+1, GLASS);
        w->setBlock(x, y+1, z+2, GLASS);
        w->setBlock(x+1, y+1, z+2, GLASS);
        w->setBlock(x+2, y+1, z+2, GLASS);

        w->setBlock(x+1, y+2, z, DICESTONE);
        w->setBlock(x, y+2, z+1, METAL);
        w->setBlock(x+1, y+2, z+1, METAL);
        w->setBlock(x+2, y+2, z+1, METAL);
        w->setBlock(x, y+2, z+2, TERM, EAST, initial_state[i]);
        w->setBlock(x+1, y+2, z+2, DICESTONE);
        w->setBlock(x+2, y+2, z+2, DICESTONE);

        w->setBlock(x+1, y+3, z, DICESTONE);
        w->setBlock(x, y+3, z+1, METAL);
        w->setBlock(x, y+3, z+2, DICESTONE);

        w->setBlock(x+1, y+4, z, DICESTONE);
        w->setBlock(x, y+4, z, METAL);
        w->setBlock(x, y+4, z+1, DICESTONE);
        w->setBlock(x+1, y+4, z+1, METAL);

        w->setBlock(x, y+5, z, DICESTONE);
        w->setBlock(x+1, y+5, z, MUT, EAST);

        w->setBlock(x+1, y+6, z, DICESTONE);
        w->setBlock(x+1, y+7, z, DICESTONE);

        w->setBlock(x, y+8, z, DICESTONE);
        w->setBlock(x+1, y+8, z, JUNCTION, EAST);
        w->setBlock(x+2, y+8, z, DICESTONE);
    }

    w->setBlock(ox-1, oy+8, 12, DICESTONE);
    w->setBlock(ox-2, oy+8, 12, SHIFTER, EAST);
    w->setBlock(ox-2, oy+9, 12, DICESTONE);
    w->setBlock(ox-2, oy+10, 12, DICESTONE);

    w->setBlock(ox-2, oy+11, 12, JUNCTION, EAST);
    w->setBlock(ox-1, oy+11, 12, DICESTONE);
    w->setBlock(ox, oy+11, 12, DICESTONE);
    w->setBlock(ox+1, oy+11, 12, LOCK, NORTH);
    w->setBlock(ox+1, oy+12, 12, DICESTONE);
    w->setBlock(ox+1, oy+13, 12, DICESTONE);
    w->setBlock(ox+1, oy+14, 12, SIGNAL, WEST);

    for (int i = 0; i < 24; ++i) {
        w->setBlock(ox+2+i, oy+11, 12, DICESTONE);
    }

    for (int i = 0; i < 5; ++i) {
        w->setBlock(ox+24+i, oy, 12, DICESTONE);
    }
}


void build_maze(World *w, int x, int y) {

    bool letters[8][80] = {
        0,0,0,0,0,0,0,1,1,1,1,0,0,0,1,1,1,1,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,0,0,0,1,1,0,0,0,1,1,1,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,0,0,0,1,1,1,0,0,0,1,1,1,1,1,1,1,1,0,0,0,1,1,0,0,0,1,1,1,0,0,0,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,0,0,0,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,1,1,0,0,0,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,0,0,0,1,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,0,0,0,0,0,0,0,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,0,0,0,1,1,1,0,0,0,1,1,1,1,1,1,1,1,0,0,0,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,0,0,0,1,1,1,0,0,0,1,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,1,1,1,1,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,1,1,1,1,0,0,0,1,1,0,0,0,1,1,1,0,0,0,1,1,0,0,0,1,1,1,1,0,0,0,0,0,0,1
    };

    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 80; ++j) {
            if (!letters[i][j]) {
                w->setBlock(x + 2 + j, y + 10 - i, 12, DICE);
            }
        }
    }

    int widths[] = {8,3,8,8,8,8,8,8};

    int off = 0;
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 12; ++j) {
            w->setBlock(x + off, y + 12 - j, 12, DICESTONE);
        }

        if (i != 8) {
            for (int k = 0; k < widths[i] + 3; ++k) {
                if (i % 2 == 0) {
                    w->setBlock(x + off + k, y + 12, 12, DICESTONE);
                } else {
                    w->setBlock(x + off + k, y + 1, 12, DICESTONE);
                }
            }
        }

        if (i == 8) {
            w->setBlock(x + off, y + 12, 12, SIGNAL, EAST);
        } else {
            w->setBlock(x + off, y + 12, 12, BUMPER, NORTH);
        }

        if (i != 0) {
            w->setBlock(x + off, y + 1, 12, BUMPER, NORTH);
        }

        off += widths[i] + 3;
    }

}


void build_flag_check(World *w, int x, int y) {
    w->setBlock(x+3, y, 12, FLAG,EAST);

    w->setBlock(x+5, y, 12,BUTTON,EAST);
    for (int i = 0; i < 10; ++i) {
        w->setBlock(x+6 + i, y, 12, DICESTONE);
    }

    w->setBlock(x+5, y+1, 12, DICESTONE);
    w->setBlock(x+5, y+2, 12, DICESTONE);

    w->setBlock(x+3, y+1, 12, DICESTONE);
    w->setBlock(x+3, y+2, 12, DICESTONE);

    int num = 10;

    for (int i = 0; i < num; ++i) {
        w->setBlock(x+5, y+3 + (i * 2), 12, SLOT, EAST);
        w->setBlock(x+6, y+3 + (i * 2), 12, DICESTONE);
        w->setBlock(x+7, y+3 + (i * 2), 12, DICESTONE);
        w->setBlock(x+5, y+4 + (i * 2), 12, DICESTONE);
        w->setBlock(x+7, y+4 + (i * 2), 12, DICESTONE);

        w->setBlock(x+3, y+3 + (i * 2), 12, DICESTONE);
        w->setBlock(x+3, y+4 + (i * 2), 12, DICESTONE);
    }

    w->setBlock(x+7, y+3 + (num * 2), 12, SIGNAL, WEST);
    w->setBlock(x+4, y+2 + (num * 2), 12, DICESTONE);
}


void build_house(World *w) {
    for (int x = 0; x < 32; ++x) {
        for (int y = -16; y <= 0; ++y) {
            for (int z = 0; z < 12; ++z) {
                w->setBlock(x,y,z,Block::METAL);
            }
        }
    }

    for (int z = 12; z <= 20; ++z) {
        w->setBlock(2, -2, z, METAL);
        w->setBlock(14, -2, z, METAL);
        w->setBlock(2, -14, z, METAL);
        w->setBlock(14, -14, z, METAL);
    }

    for (int p = 0; p < 11; ++p) {
        w->setBlock(2, -3 - p, 12, METAL);
        w->setBlock(2, -3 - p, 20, METAL);
        w->setBlock(14, -3 - p, 12, METAL);
        w->setBlock(14, -3 - p, 20, METAL);
        w->setBlock(3 + p, -14, 12, METAL);
        w->setBlock(3 + p, -14, 20, METAL);
    }

    for (int a = 0; a < 11; ++a) {
        for (int b = 0; b < 7; ++b) {
            w->setBlock(3 + a, -14, 13 + b, GLASS);
            w->setBlock(3 + a, -2, 13 + b, GLASS);
            w->setBlock(2, -3 - a, 13 + b, GLASS);
            w->setBlock(14, -3 - a, 13 + b, GLASS);
        }
    }

    for (int a = 0; a < 12; ++a) {
        for (int b = 0; b < 12; ++b) {
            w->setBlock(3 + a, -2 - b, 20, METAL);
        }
    }

    // Front doorway
    for (int p = 0; p < 4; ++p) {
        w->setBlock(3 + p, -2, 12, METAL);
        w->setBlock(13 - p, -2, 12, METAL);

        w->setBlock(6, -2, 13 + p, METAL);
        w->setBlock(10, -2, 13 + p, METAL);

        w->setBlock(7, -2, 12 + p, EMPTY);
        w->setBlock(8, -2, 12 + p, EMPTY);
        w->setBlock(9, -2, 12 + p, EMPTY);

        w->setBlock(6+p, -2, 16, METAL);
    }

    w->setBlock(3,-3,12,SPINNER,EAST);
    w->setBlock(4,-3,12,DICESTONE);
    w->setBlock(5,-3,12,DICESTONE);
    w->setBlock(6,-3,12,LETTERS,EAST);

    w->setBlock(13,-3,12,BUTTON,EAST);
    w->setBlock(12,-3,12,DICESTONE);
    w->setBlock(11,-3,12,DICESTONE);
    w->setBlock(10,-3,12,LETTERS,EAST);
}


void build(char *filename, bool solved) {
    World w;
    
    for (int x = 0; x < 300; ++x) {
        for (int y = 0; y < 100; ++y) {
            for (int z = 0; z < 12; ++z) {
                w.setBlock(x,y,z,Block::METAL);
            }
        }
    }

    int mz = 12;

    build_house(&w);

    build_flag_check(&w, 3, 3);

    int state[] = {
        12, 7, 15, 5, 5, 15, 8, 0, 14, 12, 0, 1, 7, 2, 10, 7, 4, 2, 12, 11, 4, 9, 13, 1, 6, 1, 15, 1, 14, 15, 14, 3, 14, 0, 9, 3, 15, 15, 3, 13, 15, 4, 3, 3, 14, 13, 14, 6, 1, 4, 1, 14, 5, 8, 3, 4, 0, 2, 4, 1, 0, 2, 11, 14, 15, 4, 15, 8, 9, 6, 2, 4, 0, 9, 12, 6, 4, 4, 5, 8  
    };

    int flag_state[] = {
        6, 4, 6, 9, 6, 3, 6, 5, 7, 11, 4, 13, 3, 1, 6, 14, 3, 3, 6, 3, 7, 2, 3, 4, 6, 6, 7, 4, 5, 15, 6, 2, 7, 5, 7, 4, 5, 15, 7, 7, 3, 1, 7, 4, 6, 8, 5, 15, 6, 3, 3, 0, 6, 13, 7, 0, 7, 5, 7, 4, 3, 3, 7, 2, 7, 3, 5, 15, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 7, 13
    };

    for (int i = 0; i < 10; ++i) {
        build_sub(&w, 20 + (i * 28), 3, &state[i*8], &flag_state[i*8], solved);
    }

    build_maze(&w, 15, 20);

    w.setBlock(17, 14, 12, DICESTONE);
    w.setBlock(16, 14, 12, DICESTONE);
    w.setBlock(15, 14, 12, DICESTONE);

    w.setBlock(15, 15, 12, DICESTONE);
    w.setBlock(15, 16, 12, DICESTONE);
    w.setBlock(15, 17, 12, DICESTONE);
    w.setBlock(15, 18, 12, DICESTONE);
    w.setBlock(15, 19, 12, DICESTONE);
    w.setBlock(15, 20, 12, DICESTONE);

    w.player.pos = glm::vec3(8,-8,18);

    Saver::save(filename, &w);
}


int main() {
    build("./solved.dat", true);
    build("./chal.dat", false);
}
