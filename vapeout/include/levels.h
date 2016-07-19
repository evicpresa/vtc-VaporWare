#ifndef VAPEOUT_LEVELS_H
#define VAPEOUT_LEVEL_H

#include <stdlib.h>

struct levelDesc {
    uint8_t width;
    uint8_t height;
    uint8_t layout[];
};

struct levelDesc level1Desc = {
    .width = 10,
    .height = 2,
    .layout = {1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1}
};
#endif // VAPEOUT_LEVELS_H
