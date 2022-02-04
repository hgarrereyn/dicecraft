
#pragma once

#include <cstdlib>

enum Block : uint8_t {
    EMPTY=0,
    DICE=1,
    METAL=2,
    GLASS=3,
    
    SPINNER=10,
    DIODE=11,
    TERM=12,

    DICESTONE=40,
};

enum DS : uint8_t {
    // NSEW
    _DS=100,
    _DS_N=101,
    _DS_S=102,
    _DS_E=103,
    _DS_W=104,
    _DS_NE=105,
    _DS_NW=106,
    _DS_SE=107,
    _DS_SW=108,
    _DS_NS=109,
    _DS_EW=110,
    _DS_NSE=111,
    _DS_NSW=112,
    _DS_NEW=113,
    _DS_SEW=114,
    _DS_NSEW=115,
};

enum BlockFace {
    TOP=0,
    BOTTOM=1,
    LEFT=2,
    RIGHT=3,
    FRONT=4,
    BACK=5
};

enum BlockDir {
    NORTH=0,
    SOUTH=1,
    EAST=2,
    WEST=3
};

struct BlockState {
    Block type;
    BlockDir dir;
    uint8_t state;
};

static float BlockHeight(Block b) {
    switch (b) {
        case DICESTONE:
            return 1/8.0f;
        default:
            return 1;
    }
}

static bool IsTransparent(Block b) {
    switch (b) {
        case EMPTY:
        case GLASS:
        case DICESTONE:
            return true;
        default:
            return false;
    }
}

static bool IsLogical(Block b) {
    switch (b) {
        case SPINNER:
        case DIODE:
        case TERM:
        case DICESTONE:
            return true;
        default:
            return false;
    }
}

static bool IsLogicConnector(Block b) {
    switch (b) {
        case SPINNER:
        case DIODE:
        case TERM:
        case DICESTONE:
            return true;
        default:
            return false;
    }
}

static bool IsCollider(Block b) {
    switch (b) {
        case EMPTY:
        case DICESTONE:
            return false;
        default:
            return true;
    }
}
