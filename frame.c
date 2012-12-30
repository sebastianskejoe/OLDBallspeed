#include "window.h"
#include "stb_image.h"
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int load_frames(struct window *window) {
    if (window == 0) {
        return 0;
    }
    // Go through args to find a suitable path format
    int i, frame, x, y, n;
    char *path;
    for (i = 0; i < window->argc; i++) {
        if (strstr(window->argv[i], "%d") != 0) {
            path = window->argv[i];
        }
    }

    // Load all frames
    char *fr_name = malloc(sizeof(path)+10);
    struct frame *ofr = 0; // old frame
    unsigned char *data;
    for (frame = 1; ; frame++) {
        // Check if file exists
        sprintf(fr_name, path, frame);
        if (access(fr_name, R_OK) == -1) {
            break;
        }

        // Make frame
        struct frame *nfr = malloc(sizeof(struct frame));
        nfr->index = frame;
        nfr->prev = ofr;

        // Create and bind texture
        glGenTextures(1, &nfr->texture);
        glBindTexture(GL_TEXTURE_2D, nfr->texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Load image data
        data = stbi_load(fr_name, &x, &y, &n, 3);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x,y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        nfr->data = data;

        // Extra stuff for first frame
        if (frame == 1) {
            window->frames = nfr;
            window->w = x;
            window->h = y;
            glutReshapeWindow(window->w, window->h);
            glutPostRedisplay();
        }

        // Prepare for next frame
        if (ofr) {
            ofr->next = nfr;
        }
        ofr = nfr;
    }

    frame--;
    window->nfr = frame;
    window->cur = 1;
    printf("Done loading %d frames\n", window->nfr);
    return frame;
}

struct frame * get_frame(struct frame *frames, int frame) {
    struct frame *fr;
    for (fr = frames; fr; fr = fr->next) {
        if (fr->index == frame) {
            return fr;
        }
    }
    return 0;
}
