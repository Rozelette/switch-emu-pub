int main(void) {
    int test = 5;
    int bool1 = 0;
    int bool2 = 0;

    // >
    if (test > 4) {
        // correct
    } else {
        return 1;
    }

    if (test > 6) {
        return 2;
    } else {
        // correct
    }

    // <
    if (test < 6) {
        // correct
    } else {
        return 3;
    }

    if (test < 4) {
        return 4;
    } else {
        // correct
    }

    // >=
    if (test >= 4) {
        // correct
    } else {
        return 5;
    }

    if (test >= 5) {
        // correct
    } else {
        return 6;
    }

    if (test >= 6) {
        return 7;
    } else {
        // correct
    }

    // <=
    if (test <= 6) {
        // correct
    } else {
        return 8;
    }

    if (test <= 5) {
        // correct
    } else {
        return 9;
    }

    if (test <= 4) {
        return 10;
    } else {
        // correct
    }

    // ==
    if (test == 5) {
        // correct
    } else {
        return 11;
    }

    if (test == 6) {
        return 12;
    } else {
        // correct
    }

    // !=
    if (test != 6) {
        // correct
    } else {
        return 13;
    }

    if (test != 5) {
        return 14;
    } else {
        // correct
    }

    // truth tables for &&, ||, !
    bool1 = 0;
    bool2 = 0;
    if (bool1 && bool2) {
        return 15;
    } else {
        // correct
    }
    if (bool1 || bool2) {
        return 16;
    } else {
        // correct
    }

    bool1 = 1;
    bool2 = 0;
    if (bool1 && bool2) {
        return 17;
    } else {
        // correct
    }
    if (bool1 || bool2) {
        // correct
    } else {
        return 18;
    }

    bool1 = 0;
    bool2 = 1;
    if (bool1 && bool2) {
        return 19;
    } else {
        // correct
    }
    if (bool1 || bool2) {
        // correct
    } else {
        return 20;
    }

    bool1 = 1;
    bool2 = 1;
    if (bool1 && bool2) {
        // correct
    } else {
        return 21;
    }
    if (bool1 || bool2) {
        // correct
    } else {
        return 22;
    }

    bool1 = 0;
    bool2 = 1;
    if (!bool1) {
        // correct
    } else {
        return 23;
    }
    if (!bool2) {
        return 24;
    } else {
        // correct
    }

    return 0;
}