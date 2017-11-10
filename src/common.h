#define TARGET_FPS 60.0f

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef int8_t s8;
typedef uint8_t u8;
typedef int32_t s32;
typedef uint32_t u32;

void* back_buffer;
POINT camera  = {0};
WNDCLASSEX wc;
HDC dc;
HWND main_window;

extern int buffer_width;
extern int buffer_height;

extern int window_width;
extern int window_height;

typedef struct
{
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD          acolors[256];
} dibinfo_t;
dibinfo_t bmi = { 0 };

typedef struct
{
    char* name;
    char* message;
    char color;
    BOOL active;
} Message;

Message message;

typedef enum
{
    ENTITY_TYPE_PLAYER,
    ENTITY_TYPE_ITEM,
    ENTITY_TYPE_COIN,
    ENTITY_TYPE_CREATURE,
    ENTITY_TYPE_NPC,
    ENTITY_TYPE_PARTICLE,
    ENTITY_TYPE_PROJECTILE,
    ENTITY_TYPE_FLOATING_NUMBER
} EntityType;

static void spawn_floating_number(float x, float y, int number,int color);
static void spawn_floating_string(float x, float y, char* string,int color);
static void spawn_particle(int x, int y, int size,int intensity,unsigned char character,unsigned char color);
