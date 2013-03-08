#include "hough.h"
#include "window.h"
#include "frame.h"

#include "stdio.h"

#include <opencv/cv.h>


int templateMatch(struct window *window, int frame, int diam, CvMat *tmpl) {
    // Init
    struct frame *fr = get_frame(window->frames, frame);

//    printf("Guess is (%d, %d), diameter is %d\n", window->guess.x, window->guess.y, diam);
    float init_x = (float)window->guess.x-diam, init_y = (float)window->guess.y-diam;

    // See if we can guess were the ball might be
    CvRect rect = cvRect(init_x, init_y, diam*2, diam*2);
    // Make sure rect is with image
    rect.x = rect.x < 0 ? 0 : rect.x;
    rect.y = rect.y < 0 ? 0 : rect.y;
    rect.width = rect.x+rect.width > fr->image->cols ? fr->image->cols-rect.x : rect.width;
    rect.height = rect.y+rect.height > fr->image->rows ? fr->image->rows-rect.y : rect.height;
    // Get sub rect
    CvMat *sub = cvCreateMatHeader(rect.height, rect.width, CV_32F);
    cvGetSubRect(fr->image, sub, rect);

    CvMat *res = cvCreateMat(sub->rows - tmpl->rows+1, sub->cols - tmpl->cols+1, CV_32F);

    // Match
    cvMatchTemplate(sub, tmpl, res, CV_TM_SQDIFF);

    // Find value and location of min = upper-left corner of template match
    CvPoint pt;
    double val;
    cvMinMaxLoc(res, &val, 0, &pt, 0, 0);
//    printf("#%d: value of match is %f\n", frame, val);
    if (val > 20000000) { // Works on sample video
//        printf("Doubling search area\n");
        templateMatch(window, frame, diam*2, tmpl);
        return 0;
    }

    // Match result
    struct MatchResult mr;
    mr.x = init_x+pt.x;
    mr.y = init_y+pt.y;
    mr.found = 1;

    fr->match = mr;

    window->guess.x = mr.x;
    window->guess.y = mr.y;

    return 0;
}
