#define WORLD_TILE_WIDTH  256
#define WORLD_TILE_HEIGHT 256

#define GRASS    32
#define MARSH    33
#define WATER    34
#define WATER2   35
#define SAND     36
#define MUD      37
#define MOUNTAIN 38
#define SNOW     39
#define ST0NE    40
#define BRICK    41 
#define TREE     48

static int world[WORLD_TILE_HEIGHT][WORLD_TILE_WIDTH];
static int world_collision[WORLD_TILE_HEIGHT][WORLD_TILE_WIDTH];

static void init_world(const char* path_to_world_file)
{
    FILE* fp_world = fopen(path_to_world_file,"rb");

	if (fp_world == NULL)
		return;

    int c;
	unsigned char uc;

	for (int j = 0; j < WORLD_TILE_HEIGHT; ++j)
	{
		for(int i = 0; i < WORLD_TILE_WIDTH; ++i)
		{
            c = fgetc(fp_world);
            if(c == EOF) return;

			uc = (unsigned char)c;

			world[j][i] = uc;

            switch(uc)
            {
                case GRASS: world_collision[i][j] = 1; break;
                case SAND:  world_collision[i][j] = 2; break;
                case MUD:   world_collision[i][j] = 3; break;
                case MOUNTAIN: world_collision[i][j] = 5; break;
                case TREE:  world_collision[i][j] = 5; break;
                case WATER: world_collision[i][j] = 5; break;
            }
        }
    }

    fclose(fp_world);
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
            draw_tile(i*TILE_WIDTH - camera.x,j*TILE_HEIGHT - camera.y,world[j][i]);
        }
    }
}

