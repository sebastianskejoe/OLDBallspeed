#ifndef __MATCH_H
#define __MATCH_H

#include <opencv/cv.h>

struct MatchResult {
    float x, y;
    char found;
};

#include "window.h"

#define MARGIN 50

int templateMatch(struct window *window, int frame, int diam, CvMat *templ);

#endif
