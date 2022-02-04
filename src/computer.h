

#include "block.h"

class Computer {

public:
    Computer() {}
    Computer(Computer &&) = default;

    virtual void interact() {}
    virtual void tick(int currTick) {}
    virtual void dataIn(BlockFace face, uint8_t data) {}

    virtual uint8_t getState() {
        return 0;
    }

    virtual void getOutput(bool pulse[4], uint8_t val[4]) {
        pulse[0] = false;
        pulse[1] = false;
        pulse[2] = false;
        pulse[3] = false;
    }
};


class SpinnerComputer : Computer {
    bool enabled = true;
    uint8_t sel = 0;

public:
    SpinnerComputer() {}

    virtual void interact() {
        enabled = !enabled;
    }

    virtual void tick(int currTick) {
        sel = (sel + 1) % 4;
    }

    virtual uint8_t getState() {
        if (enabled) {
            return sel;
        } else {
            return 4;
        }
    }

    virtual void getOutput(bool pulse[4], uint8_t val[4]) {
        if (enabled && (sel == 0)) {
            pulse[0] = true;
            pulse[1] = true;
            pulse[2] = true;
            pulse[3] = true;

            val[0] = 1;
            val[1] = 1;
            val[2] = 1;
            val[3] = 1;
        } else {
            pulse[0] = false;
            pulse[1] = false;
            pulse[2] = false;
            pulse[3] = false;
        }
    }

};


class TerminalComputer : Computer {
    uint8_t sel = 0;

public:
    TerminalComputer() {}

    virtual void interact() {
        sel = (sel + 1) % 16;
    }

    virtual uint8_t getState() {
        return sel;
    }
};
