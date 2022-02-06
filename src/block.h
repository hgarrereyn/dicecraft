
#pragma once

#include <cstdlib>

enum Block : uint8_t {
    EMPTY=0,
    DICE=1,
    METAL=2,
    GLASS=3,
    
    SPINNER=10,
    BUMPER=11,
    TERM=12,
    BUTTON=13,
    FLAG=14,
    LETTERS=16,
    SIGNAL=17,
    LOCK=18,
    SLOT=19,
    MUT=20,
    JUNCTION=21,
    SHIFTER=22,

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

enum BlockSide {
    S_FRONT=0,
    S_BACK=1,
    S_LEFT=2,
    S_RIGHT=3,
};

struct BlockState {
    Block type;
    BlockDir dir;
    uint32_t state;
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
        case BUMPER:
        case TERM:
        case BUTTON:
        case FLAG:
        case DICESTONE:
        case LETTERS:
        case SIGNAL:
        case LOCK:
        case MUT:
        case JUNCTION:
        case SHIFTER:
        case SLOT:
            return true;
        default:
            return false;
    }
}

static bool IsLogicConnector(Block b) {
    switch (b) {
        case SPINNER:
        case BUMPER:
        case TERM:
        case BUTTON:
        case FLAG:
        case DICESTONE:
        case LETTERS:
        case SIGNAL:
        case LOCK:
        case MUT:
        case JUNCTION:
        case SHIFTER:
        case SLOT:
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

static BlockSide RelativeSide(BlockDir blockDir, BlockDir interactDir) {
    switch (blockDir) {
        case NORTH: switch (interactDir) {
            case NORTH: return S_BACK;
            case SOUTH: return S_FRONT;
            case EAST: return S_RIGHT;
            case WEST: return S_LEFT;
        }
        case SOUTH: switch (interactDir) {
            case NORTH: return S_FRONT;
            case SOUTH: return S_BACK;
            case EAST: return S_LEFT;
            case WEST: return S_RIGHT;
        }
        case EAST: switch (interactDir) {
            case NORTH: return S_LEFT;
            case SOUTH: return S_RIGHT;
            case EAST: return S_BACK;
            case WEST: return S_FRONT;
        }
        case WEST: switch (interactDir) {
            case NORTH: return S_RIGHT;
            case SOUTH: return S_LEFT;
            case EAST: return S_FRONT;
            case WEST: return S_BACK;
        }
    }
}

static BlockDir RelativeToGlobal(BlockDir blockDir, BlockSide side) {
    switch (blockDir) {
        case NORTH: switch (side) {
            case S_FRONT: return NORTH;
            case S_BACK: return SOUTH;
            case S_LEFT: return EAST;
            case S_RIGHT: return WEST;
        }
        case SOUTH: switch (side) {
            case S_FRONT: return SOUTH;
            case S_BACK: return NORTH;
            case S_LEFT: return WEST;
            case S_RIGHT: return EAST;
        }
        case EAST: switch (side) {
            case S_FRONT: return EAST;
            case S_BACK: return WEST;
            case S_LEFT: return SOUTH;
            case S_RIGHT: return NORTH;
        }
        case WEST: switch (side) {
            case S_FRONT: return WEST;
            case S_BACK: return EAST;
            case S_LEFT: return NORTH;
            case S_RIGHT: return SOUTH;
        }
    }
}
