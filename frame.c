#include "window.h"
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <stdio.h>

#include <math.h>

int load_frames(struct window *window) {
    if (window == 0) {
        return 0;
    }
    // Go through args to find a suitable path format
    int i, frame;
    char *path;
/*    for (i = 0; i < window->argc; i++) {
        if (strstr(window->argv[i], "%d") != 0) {
            path = window->argv[i];
            break;
        }
    }*/
    path = window->argv[1];

    // Load all frames
    char *fr_name = malloc(sizeof(path)+10);
    struct frame *ofr = 0; // old frame
    for (frame = 1; ; frame++) {
        // Check if file exists
        sprintf(fr_name, path, frame);
        if (access(fr_name, R_OK) == -1) {
            break;
        }

        // Make frame
        struct frame *nfr = malloc(sizeof(struct frame));
        memset(nfr, 0, sizeof(struct frame));
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
        nfr->image = cvLoadImageM(fr_name, CV_LOAD_IMAGE_COLOR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nfr->image->cols, nfr->image->rows, 0, GL_BGR, GL_UNSIGNED_BYTE, nfr->image->data.ptr);

        // Extra stuff for first frame
        if (frame == 1) {
            window->frames = nfr;
            window->w = nfr->image->cols;
            window->h = nfr->image->rows;
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
        if (fr && fr->index == frame) {
            return fr;
        }
    }
    return 0;
}

void calculate_speed(struct frame *frame) {
    if (frame->prev && frame->prev->flag & HAS_MATCH && frame->flag & HAS_MATCH) {
        double x = frame->match.x-frame->prev->match.x;
        double y = frame->match.y-frame->prev->match.y;
        frame->speed = sqrt(pow(x, 2.0)+pow(y, 2.0));
    }
}
