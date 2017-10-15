#define TILE_WIDTH  16
#define TILE_HEIGHT 16
#define TILES_PER_ROW 16
#define MAX_INDEX   256

unsigned char tileset[256*256] = {0};

static void load_tileset(const char* tile_map_path)
{
    FILE* fp_tileset = fopen(tile_map_path,"r");
    unsigned char* d = tileset;
	
	char c;
	while((c = fgetc(fp_tileset)) != EOF)
		*d++ = c;

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

			if(*p_tileset != 16)
            {
                if (dst >= (unsigned char*)back_buffer)
                    *dst = *p_tileset;
            }

			++dst;
			++p_tileset;
        }

        dst += (buffer_width - TILE_HEIGHT);
		p_tileset += (TILES_PER_ROW*TILE_WIDTH - TILE_WIDTH);
    }

}
