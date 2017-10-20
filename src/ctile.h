#define TILE_WIDTH  16
#define TILE_HEIGHT 16
#define TILES_PER_ROW 16
#define MAX_INDEX   256

unsigned char *tileset;

static void load_tileset(const char* tile_map_path)
{
    tileset = (unsigned char *)malloc(256*256);

    FILE* fp_tileset = fopen(tile_map_path,"rb");
    unsigned char* d = tileset;
	
	int c;
	while((c = fgetc(fp_tileset)) != EOF)
		*d++ = (unsigned char)c;

    fclose(fp_tileset);
}

static void draw_tile(int x, int y, int tile_index)
{
    unsigned char* dst = back_buffer;
    dst = dst + (buffer_width*y) + x;

    unsigned char* p_tileset = tileset;

    int tile_index_x = tile_index % TILES_PER_ROW;
    int tile_index_y = tile_index / TILES_PER_ROW;

	p_tileset = p_tileset + (TILE_WIDTH*TILE_HEIGHT*TILES_PER_ROW*tile_index_y) + tile_index_x*TILE_WIDTH;

    for (int i = 0; i < TILE_HEIGHT; ++i)
    {
        for (int j = 0; j < TILE_WIDTH; ++j)
        {
			if (dst > (unsigned char*)back_buffer + (buffer_width*(buffer_height))) return;

			if(*p_tileset != 0xFF)
            {
                if (dst >= (unsigned char*)back_buffer && x+j >= 0 && x+j <= buffer_width)
                    *dst = *p_tileset;
            }

			++dst;
			++p_tileset;

            if(p_tileset > (tileset + 256*256))
                return;
        }

        dst += (buffer_width - TILE_HEIGHT);
		p_tileset += (TILES_PER_ROW*TILE_WIDTH - TILE_WIDTH);
    }

}

static void draw_tile_rotated(int x_center, int y_center, int tile_index, float angle)
{
    unsigned char* dst = back_buffer;
    dst = dst + (buffer_width*y_center) + x_center;

    unsigned char* p_tileset = tileset;

    int tile_index_x = tile_index % TILES_PER_ROW;
    int tile_index_y = tile_index / TILES_PER_ROW;

    p_tileset = p_tileset + (TILE_WIDTH*TILE_HEIGHT*TILES_PER_ROW*tile_index_y) + tile_index_x*TILE_WIDTH;

    int  x_diff, y_diff;
    char val;

    float sin_a,cos_a;

    sin_a = sin(angle);
    cos_a = cos(angle);

    int x,y;
    for (int j = 0; j < TILE_HEIGHT; j++)
    {
        y_diff = y_center - j;

        for (int i = 0; i < TILE_WIDTH; i++)
        {
            x_diff = x_center - i;

            x = x_center + (int)(-x_diff * cos_a - y_diff * sin_a);
            if (x < 0 || x > TILE_WIDTH - 1)
                continue;
            
            y = y_center + (int)(-y_diff * cos_a + x_diff * sin_a);
            if (y < 0 || y > TILE_HEIGHT - 1)
                continue;

            val = *(p_tileset + (TILE_WIDTH*y) + x);
            *(dst + (TILE_WIDTH)*j + i) = val;
        }
    }
}
