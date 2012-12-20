#include "hough.h"

#include "stdio.h"

HoughResult houghTransform() {
    #if OPENCV_BACKEND
        printf("Use openCV\n");
        // Do opencv hough transform
    #else
        printf("Use own implementation\n");
        // Implement own hough transform
    #endif
}
