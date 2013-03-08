#ifndef __HOUGH_H
#define __HOUGH_H

#include <opencv/cv.h>

struct HoughResult {
    float x, y;
    float radius;
    char found;
};

#include "window.h"

int houghTransform(struct window *window, int frame, int x, int y);

#endif
