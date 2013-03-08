#ifndef __FRAME_H
#define __FRAME_H

#include <GL/gl.h>
#include "window.h"
#include "hough.h"
#include "match.h"
#include <opencv/cv.h>

#define HAS_EDGE    1
#define HAS_HOUGH   2
#define HAS_MATCH   4

struct frame {
    GLuint texture;
    int index;
    char flag;
    CvMat *image;
//    struct EdgeResult edge;
    struct HoughResult hough;
    struct MatchResult match;
    struct frame *prev, *next;

    float speed; // Compared to previous frame
};

int load_frames(struct window *window);
struct frame * get_frame(struct frame *frames, int frame);
void calculate_speed(struct frame *frame);

#endif
