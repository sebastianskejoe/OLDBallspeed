#include "window.h"
#include "stb_image.h"
#include <GL/glut.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void initGLUT();

int create_window(struct window *window) {
    if (window == 0) {
        return 1;
    }
    initGLUT(window);
}

void initGLUT(struct window *window) {
    glutInit(&window->argc,window->argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_SINGLE | GLUT_RGBA);
    glutInitWindowPosition(0,0);
    glutInitWindowSize(window->w, window->h);
    glutCreateWindow("Ballspeed");

    glutDisplayFunc(redraw_window);
    glutSpecialFunc(event_key_special);
    return;
}

void redraw_window() {
    struct frame *frame;
    glLoadIdentity();
	glOrtho(0, 1, 0, 1,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    switch (active_window->state) {
    case STATE_LOADING:
        glColor3f(1.0f, 1.0f, 1.0f);
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, "Loading frames ... Please wait.");
        glFlush();
        load_frames(active_window);
        active_window->state = STATE_VIEW;
        break;
    case STATE_VIEW:
        // Get frame
        frame = get_frame(active_window->frames, active_window->cur);
        if (frame == 0) {
            printf("Frame #%d wasn't found\n", active_window->cur);
            return;
        }

        // Draw image
        glBindTexture(GL_TEXTURE_2D, frame->texture);
        glEnable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
            glTexCoord2f(0.0,1.0);
            glVertex2f(0,0);
            glTexCoord2f(1.0,1.0);
            glVertex2i(1.0f, 0.0f);
            glTexCoord2f(1.0f,0.0f);
            glVertex2f(1.0f, 1.0f);
            glTexCoord2f(0.0f, 0.0f);
            glVertex2f(0,1.0f);
        glEnd();
        glDisable(GL_TEXTURE_2D);

        glColor3f(1.0f, 1.0f, 1.0f);
        char *str = malloc(255);
        sprintf(str, "Frame %d of %d", active_window->cur, active_window->nfr);
        glRasterPos2f(0.0f, 0.0f);
        glutBitmapString(GLUT_BITMAP_HELVETICA_12, str);
        break;
    }

	glFlush();
}

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
        stbi_image_free(data);

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

/********************** *
 * Event handlers       *
 * *********************/
void event_key_special(int key, int x, int y) {
    if (active_window->state == STATE_LOADING) {
        return;
    }

    int step = 0;
    switch (key) {
    case GLUT_KEY_LEFT:
        step = -1;
        break;
    case GLUT_KEY_RIGHT:
        step = 1;
        break;
    case GLUT_KEY_UP:
        step = 10;
        break;
    case GLUT_KEY_DOWN:
        step = -10;
        break;
    }

    if (active_window->cur+step > 0 && active_window->cur+step <= active_window->nfr) {
        active_window->cur += step;
        glutPostRedisplay();
    }
}
