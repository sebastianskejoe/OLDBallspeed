#include "window.h"
#include <GL/glut.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "output.h"

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
    glutKeyboardFunc(event_key);
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

        // Draw text
        glColor3f(1.0f, 1.0f, 1.0f);
        char *fr_str = malloc(255);
        char *rad_str = malloc(255);
        sprintf(fr_str, "Frame %d of %d", active_window->cur, active_window->nfr);
        sprintf(rad_str,"Radius: %d", active_window->radius);

        glRasterPos2f(0.0f, 0.0f);
        glutBitmapString(GLUT_BITMAP_HELVETICA_12, fr_str);
        glutBitmapString(GLUT_BITMAP_HELVETICA_12, rad_str);

        // If we have hough transformed it, draw a circle
        if (frame->flag & HAS_HOUGH && frame->hough.found) {
            glLoadIdentity();
            glOrtho(0, active_window->w, active_window->h, 0, 0, 1);
	        float theta;
            float x0, y0, r;
            x0 = frame->hough.x;
            y0 = frame->hough.y;
            r = frame->hough.radius;


            glBegin(GL_LINE_LOOP);
                // Draw circle
                glColor3f(1.0f, 0.0f, 0.0f);
                for (theta = 0.0f; theta < 2*M_PI; theta += 0.01f) {
                    glVertex2f(x0+r*cos(theta), y0+r*sin(theta));
                }

                glColor3f(1.0f, 1.0f, 1.0f);
            glEnd();
        }

        // Draw square around match
        if (frame->flag & HAS_MATCH) {
            glLoadIdentity();
            glOrtho(0, active_window->w, active_window->h, 0, 0, 1);
            float w = (float)active_window->tmpl->cols, h = (float)active_window->tmpl->rows;

            glBegin(GL_LINE_LOOP);
                glColor3f(1.0f, 0.0f, 0.0f);
                glVertex2f(frame->match.x+w, frame->match.y+h);
                glVertex2f(frame->match.x+w, frame->match.y);
                glVertex2f(frame->match.x, frame->match.y);
                glVertex2f(frame->match.x, frame->match.y+h);
                glColor3f(1.0f, 1.0f, 1.0f);
            glEnd();
        }

        free(fr_str);
        free(rad_str);
        break;
    }

	glFlush();
}

/********************** *
 * Event handlers       *
 * *********************/
void event_key_special(int key, int x, int y) {
    if (active_window->state == STATE_LOADING) {
        return;
    }

    int step = 0, i;
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
    case GLUT_KEY_HOME: // Perform match on all frames
        for (i = 1; i <= active_window->nfr; i++) {
            struct frame *fr = get_frame(active_window->frames, i);
            if (fr->flag & HAS_MATCH) {
                continue;
            }
            templateMatch(active_window, i, MARGIN, active_window->tmpl);
            fr->flag |= HAS_MATCH;
            calculate_speed(fr);
        }
        printf("Done matching\n");
    }

    // Update view
    if (active_window->cur+step > 0 && active_window->cur+step <= active_window->nfr) {
        active_window->cur += step;
        glutPostRedisplay();
    }
}

void event_key(unsigned char key, int x, int y) {
    switch (key) {
        case 'p':
        case 'P':
            print_pos_speed(active_window);
            break;
    }
}

void event_mouse(int button, int state, int x, int y) {
    int err;
    if (state == GLUT_DOWN) { // Mouse down = find template
        // Do hough transform to find ball center and radius
        struct frame *fr = get_frame(active_window->frames, active_window->cur);
        err = houghTransform(active_window, active_window->cur, x, y);
        if (err) {
            return;
        }
        fr->flag |= HAS_HOUGH;
        active_window->guess.x = fr->hough.x;
        active_window->guess.y = fr->hough.y;

        // Make a subimage containing the template
        CvRect r = cvRect(fr->hough.x-fr->hough.radius, fr->hough.y-fr->hough.radius, fr->hough.radius*2, fr->hough.radius*2);
        CvMat *sub = cvCreateMatHeader(fr->hough.radius*2, fr->hough.radius*2, CV_32FC1);
        cvGetSubRect(fr->image, sub, r);
        active_window->tmpl = sub;

        // Match (could be left out)
        templateMatch(active_window, active_window->cur, MARGIN, sub);
        fr->flag |= HAS_MATCH;
        glutPostRedisplay();

        // Calculate meters per pixel
        active_window->mpp = atof(active_window->argv[2])/(fr->hough.radius*2);
        printf("Getting mpp: %f/%f = %f\n", atof(active_window->argv[2]), fr->hough.radius*2, active_window->mpp);
    }
}
