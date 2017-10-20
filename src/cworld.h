#define WORLD_TILE_WIDTH  256
#define WORLD_TILE_HEIGHT 256

#define GRASS 32
#define WATER 33
#define SAND  34
#define MUD   35
#define STONE 36
#define BRICK 37

static int world[WORLD_TILE_WIDTH][WORLD_TILE_HEIGHT];

static void init_world()
{
	for (int j = 0; j < WORLD_TILE_HEIGHT; ++j)
	{
		for(int i = 0; i < WORLD_TILE_WIDTH; ++i)
		{
            world[i][j] = (rand() % 6) + 32;
        }
    }
}

static void draw_world(POINT camera)
{
    int start_x = camera.x / TILE_WIDTH;
    int start_y = camera.y / TILE_HEIGHT;

    int end_x = start_x + (buffer_width/TILE_WIDTH) + 1;
    int end_y = start_y + (buffer_height/TILE_HEIGHT) + 1;

    if (end_x > WORLD_TILE_WIDTH)  end_x = WORLD_TILE_WIDTH;
    if (end_y > WORLD_TILE_HEIGHT) end_y = WORLD_TILE_HEIGHT;

    for(int j = start_y; j < end_y; ++j)
    {
        for(int i = start_x; i < end_x; ++i)
        {
            draw_tile(i*TILE_WIDTH - camera.x,j*TILE_HEIGHT - camera.y,world[i][j]);
        }
    }
}

