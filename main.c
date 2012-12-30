#include "hough.h"
#include "window.h"

#include <stdlib.h>
#include <GL/glut.h>

int main(int argc, char **argv) {
    // Create window
    struct window *window = malloc(sizeof(struct window));
    active_window = window;
    window->w = 400;
    window->h = 400;
    window->argc = argc;
    window->argv = argv;
    window->state = STATE_LOADING;
    window->radius = 50;
    create_window(window);

    glutMainLoop();

    return 0;
}
