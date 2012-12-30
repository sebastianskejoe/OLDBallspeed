#include "edge.h"
#include "window.h"

#include "stdio.h"

#include <opencv/cv.h>

struct EdgeResult edgeDetect(struct window *window, int x, int y) {
    struct EdgeResult er;
    // Create cvMat
    unsigned char *data = get_sub_image(window, x, y);
    CvMat *mat = cvCreateMatHeader(window->radius*2, window->radius*2, CV_8UC3);
    cvSetData(mat, data, window->radius*2*3);
    CvMat *gray = cvCreateMat(window->radius*2, window->radius*2, CV_8UC1);
    cvCvtColor(mat, gray, CV_RGB2GRAY);

    // Result
    er.x = x;
    er.y = y;
    er.radius = window->radius;
    er.data = malloc(window->radius*window->radius*4);
    CvMat *res = cvCreateMatHeader(window->radius*2, window->radius*2, CV_8UC1);
    cvSetData(res, er.data, window->radius*2);

    // Algorithm
    cvCanny(gray, res, 100, 300, 3);

    // Clean up
    free(data);

    return er;
}
