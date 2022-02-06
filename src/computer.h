

#include "block.h"

class Computer {

public:
    Computer() {}
    Computer(Computer &&) = default;

    virtual void interact() {}
    virtual void tick(int currTick) {}

    virtual void dataIn(BlockSide side, uint32_t data) {}

    virtual uint32_t getState() {
        return 0;
    }

    virtual void setState(uint32_t state) {}

    // 0: front (north)
    // 1: back (south)
    // 2: left (west)
    // 3: right (east)
    virtual void getOutput(bool pulse[4], uint32_t val[4]) {
        pulse[0] = false;
        pulse[1] = false;
        pulse[2] = false;
        pulse[3] = false;
    }
};


class SpinnerComputer : Computer {
    bool enabled = true;
    uint32_t sel = 0;

public:
    SpinnerComputer() {}

    virtual void interact() {
        enabled = !enabled;
    }

    virtual void tick(int currTick) {
        sel = (sel + 1) % 4;
    }

    virtual uint32_t getState() {
        if (enabled) {
            return sel;
        } else {
            return 4;
        }
    }

    virtual void getOutput(bool pulse[4], uint32_t val[4]) {
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
    uint32_t sel = 0;
    bool inputs[4];
    bool triggered = false;

public:
    TerminalComputer() {
        inputs[0] = false;
        inputs[1] = false;
        inputs[2] = false;
        inputs[3] = false;
    }

    virtual void dataIn(BlockSide side, uint32_t data) {
        inputs[side] = true;
    }

    virtual void tick(int currTick) {
        if (inputs[S_LEFT]) {
            triggered = true;
        } else if (triggered) {
            triggered = false;
        }

        inputs[0] = false;
        inputs[1] = false;
        inputs[2] = false;
        inputs[3] = false;
    }

    virtual void getOutput(bool pulse[4], uint32_t val[4]) {
        pulse[0] = false;
        pulse[1] = false;
        pulse[2] = false;
        pulse[3] = false;

        if (triggered) {
            // signal (daisy chain)
            pulse[S_RIGHT] = true;
            val[S_RIGHT] = 1;

            // value
            pulse[S_BACK] = true;
            val[S_BACK] = sel;
        }
    }

    virtual void interact() {
        sel = (sel + 1) % 16;
    }

    virtual uint32_t getState() {
        return sel;
    }

    virtual void setState(uint32_t state) {
        sel = state;
    }
};


class ButtonComputer : Computer {
    bool active = false;
    bool pressed = false;

public:
    ButtonComputer() {}

    virtual uint32_t getState() {
        return (uint32_t)active;
    }

    virtual void interact() {
        pressed = true;
    }

    virtual void tick(int currTick) {
        if (pressed) {
            active = true;
            pressed = false;
        } else if (active) {
            active = false;
        }
    }

    virtual void getOutput(bool pulse[4], uint32_t val[4]) {
        if (active) {
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


class FlagComputer : Computer {
    bool active = false;

public:
    FlagComputer() {}

    virtual uint32_t getState() {
        return (uint32_t)active;
    }

    virtual void dataIn(BlockSide side, uint32_t data) {
        active = true;
    }
};


class LettersComputer : Computer {
    uint32_t sel;
    bool has_input = false;;

public:
    LettersComputer() {}

    virtual uint32_t getState() {
        return sel;
    }

    virtual void dataIn(BlockSide side, uint32_t data) {
        has_input = true;
    }

    virtual void tick(int currTick) {
        if (has_input) {
            sel = (sel + 1) % 8;
            has_input = false;
        }
    }
};


static bool global_signal = false;
static uint32_t global_signal_val = false;

class SignalComputer : Computer {
    bool has_input = false;
    bool has_output = false;
    uint32_t input_val = 0;
    uint32_t output_val = 0;

    bool broadcasting = false;
    bool listening = false;

public:
    SignalComputer() {}

    virtual void dataIn(BlockSide side, uint32_t data) {
        if (side == S_FRONT) {
            has_input = true;
            input_val = data;
        }
    }

    virtual uint32_t getState() {
        return broadcasting | listening;
    }

    virtual void getOutput(bool pulse[4], uint32_t val[4]) {
        pulse[0] = false;
        pulse[1] = false;
        pulse[2] = false;
        pulse[3] = false;

        if (listening) {
            pulse[S_BACK] = true;
            val[S_BACK] = output_val;
        }
    }

    virtual void tick(int currTick) {
        if (has_input) {
            has_input = false;
            broadcasting = true;
            global_signal = true;
            global_signal_val = false;
        } else if (broadcasting) {
            broadcasting = false;
            global_signal = false;
        } else if (global_signal) {
            listening = true;
            has_output = true;
            output_val = global_signal_val;
        } else if (listening) {
            has_output = false;
            listening = false;
        }
    }
};


class LockComputer : Computer {
    bool has_val = false;
    uint32_t stored_val = 0;

    bool has_trigger = false;
    int trigger_count = 0;

    bool writing = false;

public:
    LockComputer() {}

    virtual void dataIn(BlockSide side, uint32_t data) {
        if (side == S_LEFT || side == S_RIGHT) {
            has_trigger = true;
        } else if (side == S_FRONT) {
            has_val = true;
            stored_val = data;
        }
    }

    virtual uint32_t getState() {
        return has_val | writing;
    }

    virtual void getOutput(bool pulse[4], uint32_t val[4]) {
        pulse[0] = false;
        pulse[1] = false;
        pulse[2] = false;
        pulse[3] = false;

        if (writing) {
            pulse[S_BACK] = true;
            val[S_BACK] = stored_val;
        }
    }

    virtual void tick(int currTick) {
        if (has_trigger) {
            trigger_count += 1;
            has_trigger = false;
        }

        if (trigger_count > 0 && has_val) {
            has_val = false;
            writing = true;
            trigger_count -= 1;
        } else if (writing) {
            writing = false;
        }
    }
};


class SlotComputer : Computer {
    bool has_val = false;
    uint32_t stored_val = 0;

    bool has_trigger = false;

    bool writing = false;

public:
    SlotComputer() {}

    virtual void dataIn(BlockSide side, uint32_t data) {
        if (side == S_LEFT || side == S_RIGHT) {
            has_trigger = true;
        } else if (side == S_FRONT) {
            has_val = true;
            stored_val = data;
        }
    }

    virtual uint32_t getState() {
        return has_val | writing;
    }

    virtual void getOutput(bool pulse[4], uint32_t val[4]) {
        pulse[0] = false;
        pulse[1] = false;
        pulse[2] = false;
        pulse[3] = false;

        if (writing) {
            pulse[S_BACK] = true;
            val[S_BACK] = stored_val;
        }
    }

    virtual void tick(int currTick) {
        if (has_trigger && has_val) {
            has_val = false;
            writing = true;
        } else if (writing) {
            writing = false;
        }
        has_trigger = false;
    }
};


class BumperComputer : Computer {
    uint32_t state = 0xd1c40677;

    BlockSide out_dir;

    bool has_input = 0;
    uint32_t input_val = 0;
    
    bool writing;
    bool cooldown;

public:
    BumperComputer() {}

    virtual void dataIn(BlockSide side, uint32_t data) {
        if (!writing && !has_input && !cooldown) {
            has_input = true;
            input_val = data;
        }
    }

    virtual void tick(int currTick) {
        if (has_input) {
            has_input = false;
            
            if (input_val != 0) {

                uint8_t off_dir = input_val & 3;
                input_val >>= 2;

                // Compute output dir.
                state ^= state << 13;
                state ^= state >> 17;
                state ^= state << 5;
                
                if (input_val > 0) {
                    state = (state * input_val) ^ input_val;
                }

                state += off_dir;

                switch (state % 4) {
                    case 0: out_dir = S_FRONT; break;
                    case 1: out_dir = S_BACK; break;
                    case 2: out_dir = S_LEFT; break;
                    case 3: out_dir = S_RIGHT; break;
                }

                writing = true;
            }
        } else if (writing) {
            writing = false;
            cooldown = true;
        } else if (cooldown) {
            cooldown = false;
        }
    }

    virtual void getOutput(bool pulse[4], uint32_t val[4]) {
        pulse[0] = false;
        pulse[1] = false;
        pulse[2] = false;
        pulse[3] = false;

        if (writing) {
            pulse[out_dir] = true;
            val[out_dir] = input_val;
        }
    }
};


class MutComputer : Computer {
    uint32_t left = 0;
    uint32_t right = 0;
    bool has_left = false;
    bool has_right = false;
    bool triggered = false;
    uint32_t out_val = 0;

public:
    MutComputer() {}

    virtual uint32_t getState() {
        return (uint32_t)triggered;
    }

    virtual void dataIn(BlockSide side, uint32_t data) {
        if (side == S_LEFT) {
            has_left = true;
            left = data;
        } else if (side == S_FRONT) {
            has_right = true;
            right = data;
        }
    }

    uint32_t doMut(uint32_t a, uint32_t b) {
        uint8_t mat[] = {7, 9, 5, 6, 14, 10, 12, 8, 1, 2, 13, 15, 4, 11, 0, 3};
        return mat[((a & 0xf) + (b & 0xf)) % 16];
    }

    virtual void tick(int currTick) {
        if (has_left && has_right) {
            triggered = true;

            out_val = doMut(left, right);

            has_left = false;
            has_right = false;
        } else if (triggered) {
            triggered = false;
        }
    }

    virtual void getOutput(bool pulse[4], uint32_t val[4]) {
        pulse[0] = false;
        pulse[1] = false;
        pulse[2] = false;
        pulse[3] = false;

        if (triggered) {
            pulse[S_BACK] = true;
            val[S_BACK] = out_val;
        }
    }
};


class JunctionComputer : Computer {

    bool has_input = false;
    uint32_t input_val = 0;

    bool writing = false;

public:
    JunctionComputer() {}

    virtual uint32_t getState() {
        return (uint32_t)writing;
    }

    virtual void dataIn(BlockSide side, uint32_t data) {
        if (side == S_RIGHT || side == S_FRONT) {
            has_input = true;
            input_val = data;
        }
    }

    virtual void tick(int currTick) {
        if (has_input) {
            has_input = false;
            writing = true;
        } else if (writing) {
            writing = false;
        }
    }

    virtual void getOutput(bool pulse[4], uint32_t val[4]) {
        pulse[0] = false;
        pulse[1] = false;
        pulse[2] = false;
        pulse[3] = false;

        if (writing) {
            pulse[S_LEFT] = true;
            val[S_LEFT] = input_val;
        }
    }
};


class ShifterComputer : Computer {

    bool has_input = false;
    uint32_t input_val = 0;

    uint32_t aggregate = 0;
    uint8_t count = 0;

    bool writing = false;

public:
    ShifterComputer() {}

    virtual uint32_t getState() {
        return (uint32_t)writing;
    }

    virtual void dataIn(BlockSide side, uint32_t data) {
        if (side == S_RIGHT) {
            has_input = true;
            input_val = data;
        }
    }

    virtual void tick(int currTick) {
        if (has_input) {
            has_input = false;
            aggregate = (aggregate << 4) + input_val;
            count += 1;

            if (count == 8) {
                count = 0;
                writing = true;
            }
        } else if (writing) {
            writing = false;
            aggregate = 0;
        }
    }

    virtual void getOutput(bool pulse[4], uint32_t val[4]) {
        pulse[0] = false;
        pulse[1] = false;
        pulse[2] = false;
        pulse[3] = false;

        if (writing) {
            pulse[S_BACK] = true;
            val[S_BACK] = aggregate;
        }
    }
};
