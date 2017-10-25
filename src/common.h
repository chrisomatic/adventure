#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef int8_t s8;
typedef uint8_t u8;
typedef int32_t s32;
typedef uint32_t u32;

void* back_buffer;
POINT camera  = {0};

extern int buffer_width;
extern int buffer_height;

extern int window_width;
extern int window_height;


typedef enum
{
	DIR_UP = 0,
	DIR_DOWN = 3,
	DIR_LEFT = 6,
	DIR_RIGHT = 9
} Direction;

