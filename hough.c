#include "hough.h"
#include "window.h"
#include "frame.h"

#include "stdio.h"

#include <opencv/cv.h>

int houghTransform(struct window *window, int frame, int x, int y) {
    if (x < window->radius || y < window->radius) {
        return 1;
    }
  
    // Initialize
    struct frame *fr = get_frame(window->frames, frame);
    CvRect rect = cvRect(x-window->radius, y-window->radius, window->radius*2, window->radius*2);
    CvMat *sub = cvCreateMatHeader(window->radius*2, window->radius*2, CV_8UC3);
    CvMat *gray = cvCreateMat(window->radius*2, window->radius*2, CV_8UC1);

    // Get sub image in gray
    cvGetSubRect(fr->image, sub, rect);
    cvCvtColor(sub, gray, CV_BGR2GRAY);

    // Algorithm
    CvMemStorage *mem = cvCreateMemStorage(0);
    CvSeq *seq = cvHoughCircles(gray, mem, CV_HOUGH_GRADIENT, 2, 5, 200, 50, 10, window->radius);

    // Result
    struct HoughResult res;
    if (seq->total > 0) {
        CvPoint3D32f *circle = CV_GET_SEQ_ELEM(CvPoint3D32f, seq, 0); // Assume first match is the correct one
        res.found = 1;
        res.x = (float)(x-window->radius)+circle->x;
        res.y = (float)(y-window->radius)+circle->y;
        res.radius = circle->z;
    }
    fr->hough = res;

    return 0;
}
