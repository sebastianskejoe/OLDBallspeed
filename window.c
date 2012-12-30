#include "window.h"
#include "stb_image.h"
#include <GL/glut.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "edge.h"

void initGLUT();

int create_window(struct window *window) {
    if (window == 0) {
        return 1;
    }
    initGLUT(window);
    return 0;
}

void initGLUT(struct window *window) {
    glutInit(&window->argc,window->argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_SINGLE | GLUT_RGBA);
    glutInitWindowPosition(0,0);
    glutInitWindowSize(window->w, window->h);
    glutCreateWindow("Ballspeed");

    glutDisplayFunc(redraw_window);
    glutSpecialFunc(event_key_special);
    glutMouseFunc(event_mouse);
    return;
}

void redraw_window() {
    struct frame *frame;
    glLoadIdentity();
    glViewport(0,0, active_window->w, active_window->h);
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
        char *fr_str = malloc(255);
        char *rad_str = malloc(255);
        sprintf(fr_str, "Frame %d of %d", active_window->cur, active_window->nfr);
        sprintf(rad_str,"Radius: %d", active_window->radius);

        glRasterPos2f(0.0f, 0.0f);
        glutBitmapString(GLUT_BITMAP_HELVETICA_12, fr_str);
        glutBitmapString(GLUT_BITMAP_HELVETICA_12, rad_str);

        // If we have an edge image, draw that
        if (frame->flag & HAS_EDGE) {
            // Create texture
            GLuint tex;
            glGenTextures(1, &tex);
            glBindTexture(GL_TEXTURE_2D, tex);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Load image data
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, frame->edge.radius*2, frame->edge.radius*2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, frame->edge.data);
            glEnable(GL_TEXTURE_2D);
            glViewport(frame->edge.x-frame->edge.radius, active_window->h-frame->edge.y-frame->edge.radius, frame->edge.radius*2, frame->edge.radius*2);
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
        }

        free(fr_str);
        free(rad_str);
        break;
    }

	glFlush();
}

unsigned char *get_sub_image(struct window *window, int x, int y) {
    // Allocate sub image
    unsigned char *data = malloc(window->radius*window->radius*12);
    memset(data, 0, window->radius*window->radius*12);

    struct frame *fr = get_frame(window->frames, window->cur);

    // Copy each row
    int i, stride = window->w*3;
    for (i = 0; i < window->radius*2; i++) {
        memcpy(data+i*window->radius*6, fr->data+(y-window->radius+i)*stride+(x-window->radius)*3, window->radius*6);
    }

    return data;
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

void event_mouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        struct frame *fr = get_frame(active_window->frames, active_window->cur);
        struct EdgeResult er = edgeDetect(active_window, x, y);
        fr->edge = er;
        fr->flag |= HAS_EDGE;
        glutPostRedisplay();
    }
}
