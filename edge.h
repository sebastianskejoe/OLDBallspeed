#ifndef __EDGE_H
#define __EDGE_H

struct EdgeResult {
    int x, y;
    int radius;
    char *data;
};

#include "window.h"

struct EdgeResult edgeDetect(struct window *window, int x, int y);

#endif
