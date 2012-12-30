#ifndef __FRAME_H
#define __FRAME_H

#include <GL/gl.h>
#include "window.h"
#include "edge.h"

#define HAS_EDGE    1
#define HAS_HOUGH   2

struct frame {
    GLuint texture;
    int index;
    char flag;
    unsigned char *data;
    struct EdgeResult edge;
    struct frame *prev, *next;
};

int load_frames(struct window *window);
struct frame * get_frame(struct frame *frames, int frame);

#endif
