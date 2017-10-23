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

static void draw_tile_tinted(int x, int y, int tile_index,int tint_amount)
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
                if (dst >= (unsigned char*)back_buffer && x+j >= 0 && x+j < buffer_width)
                    *dst = tint_amount;
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
                if (dst >= (unsigned char*)back_buffer && x+j >= 0 && x+j < buffer_width)
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

static void draw_tile_rotated(int x, int y, int tile_index, float angle)
{
    int tile_index_x = tile_index % TILES_PER_ROW;
    int tile_index_y = tile_index / TILES_PER_ROW;

    unsigned char* src = tileset;
    src = src + (TILE_WIDTH*TILE_HEIGHT*TILES_PER_ROW*tile_index_y) + tile_index_x*TILE_WIDTH;

    unsigned char* dst = back_buffer;
    dst = dst + (buffer_width*y) + x;

    float cosa = cos(-angle);
    float sina = sin(-angle);

    int x_center = TILE_WIDTH/2;
    int y_center = TILE_HEIGHT/2;

    int x_diff, y_diff;
    int u,v;

    unsigned char* src_t = src;

    // (i,j) [dst] -> (u,v) [src]
    for (int j = 0; j < TILE_HEIGHT; ++j)
    {
        y_diff = y_center - j;

        for (int i = 0; i < TILE_WIDTH; ++i)
        {
            x_diff = x_center -i;

            u = x_center - (int)(+cosa*x_diff + sina*y_diff);
            v = y_center - (int)(-sina*x_diff + cosa*y_diff);

            if (u < 0 || u >= TILE_WIDTH || v < 0 || v >= TILE_HEIGHT)
            {
                // don't draw anything
            }
            else
            {
                src_t = src+(TILE_WIDTH*TILES_PER_ROW*v) + u;

				if (*src_t != 0xFF)
                    *dst = *src_t;
            }

            dst++;
			if (dst > (unsigned char*)back_buffer + (buffer_width*buffer_height)) return;
        }

        dst += (buffer_width - TILE_HEIGHT);

    }
}
