#include "output.h"
#include <stdio.h>

void print_pos_speed(struct window *window) {
    int i;
    struct frame *fr;
    printf("Frame\tX\tY\tSpeed\tSpeed(m per frame)\n");
    for (i = 1; i <= window->nfr; i++) {
        fr = get_frame(window->frames, i);
        if (!(fr->flag & HAS_MATCH)) {
            continue;
        }

        // Print positions
        printf("#%d\t%.0f\t%.0f\t%.2f\t%.2f\n", i, fr->match.x, fr->match.y, fr->speed, fr->speed*window->mpp);
    }
}
