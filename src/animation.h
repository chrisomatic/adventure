#pragma once

#define MAX_ANIMATION_FRAMES 16

typedef struct
{
    int counter;
    int max_count;
    int frame;
    int num_frames;
    int frame_order[MAX_ANIMATION_FRAMES];
} Animation;

