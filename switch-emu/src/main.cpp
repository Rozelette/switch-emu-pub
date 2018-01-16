#include <stdio.h>

#include "log.h"
#include "emu.h"

#undef main // For SDL. TODO: how to avoid this?
// TODO fatal errors

int main(int argc, char* argv[]) {
    // TODO actual command line parser
    // TODO arg to load elf

    if (argc != 2) {
        printf("Usage: switch-emu [path]");
        return -1;
    }

    // Disable buffering in stdout so we get full logging to file even when we crash
    // TODO put in some kind of logging init?
    setvbuf(stdout, NULL, _IONBF, 0);

    Emulator emu;

    if (!emu.initSDL()) {
        logger::error("Couldn't init SDL");
        return -1;
    }

    if (!emu.initGraphics()) {
        logger::error("Couldn't init graphics");
        return -1;
    }

    if (!emu.run(argv[1])) {
        logger::error("Couldn't run game");
        return -1;
    }

    return 0;
}