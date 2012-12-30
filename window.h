#ifndef __WINDOW_H
#define __WINDOW_H

struct window;

#include <GL/gl.h>
#include "frame.h"

#define STATE_LOADING   1
#define STATE_VIEW      2

struct window {
    char state; // Window state
    int cur; // Current frame
    int nfr; // Number of frames
    int radius; // Radius of search area
    int w, h; // Window width and height
    int argc;
    char **argv;
    struct frame *frames;
};

struct window *active_window;

int create_window(struct window *window);
void redraw_window();
unsigned char *get_sub_image(struct window *window, int x, int y);

void event_key_special(int key, int x, int y);
void event_mouse(int button, int state, int x, int y);

#endif
