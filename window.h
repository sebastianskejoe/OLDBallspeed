#ifndef __WINDOW_H
#define __WINDOW_H

#include <GL/gl.h>

#define STATE_LOADING   1
#define STATE_VIEW      2

struct frame {
    GLuint texture;
    int index;
    struct frame *prev, *next;
};

struct window {
    char state; // Window state
    int cur; // Current frame
    int nfr; // Number of frames
    int w, h; // Window width and height
    int argc;
    char **argv;
    struct frame *frames;
};

struct window *active_window;

int create_window(struct window *window);
void redraw_window();
int load_frames(struct window *window);
struct frame * get_frame(struct frame *frames, int frame);

void event_key_special(int key, int x, int y);

#endif
