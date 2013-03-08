#include "hough.h"
#include "window.h"

#include <stdlib.h>
#include <GL/glut.h>

#include <stdio.h>

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Usage: %s [path to images] [diameter of ball(meter)]\n", argv[0]);
        return 1;
    }
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
