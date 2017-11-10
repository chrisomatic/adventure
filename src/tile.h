#define TILE_WIDTH  16
#define TILE_HEIGHT 16
#define TILES_PER_ROW 16
#define MAX_INDEX   256
#define MAX_TILESET_LENGTH 65536

typedef struct
{
    char name[MAX_PATH];
    unsigned char data[MAX_TILESET_LENGTH];
} Tileset;

Tileset tileset_list[16];
int num_tilesets = 0;

unsigned char *tileset;

static int get_tileset_index_by_name(const char* name)
{
	if (!name)
		return;

    for(int i = 0; i < num_tilesets; ++i)
    {
        if(strcmp(tileset_list[i].name,name) == 0)
            return i;
    }

    return -1;
}

static void generate_indexed_tileset(const char* rgb_image_path,const char* indexed_path)
{
    int w,h,n;
    unsigned char *imgdata = stbi_load(rgb_image_path,&w,&h,&n,0);

    if(imgdata == NULL)
        return;

    unsigned char *p = imgdata;

    FILE * fp;
    fp = fopen (indexed_path, "wb");

    for(int j = 0; j < h; ++j)
    {
        for(int i = 0; i < w; ++i)
        {
            // find closest index for rgb pixel
            
            int  color_delta = 0;
            int  min_color_delta = 256*255*255;
            int  min_delta_index = 255;

			BOOL color_found = FALSE;
            for (int k = 0; k < 256; ++k)
            {
                color_delta = 0;

                color_delta += abs(bmi.acolors[k].rgbRed   - *(p+0));
                color_delta += abs(bmi.acolors[k].rgbGreen - *(p+1));
                color_delta += abs(bmi.acolors[k].rgbBlue  - *(p+2));

                if(color_delta < min_color_delta)
                {
                    min_color_delta = color_delta;
                    min_delta_index = k;
                }
            }

            fputc(min_delta_index,fp);

            p+=n;
        }
    }
    
    fclose(fp);
    stbi_image_free(imgdata);
}

static void generate_all_tilesets()
{
	char paths[100][MAX_PATH] = { 0 };
	int num_files = get_files_in_directory_with_extension("data\\tilesets", ".tileset.png", paths);

    for(int i = 0; i < num_files; ++i)
    {
		char index_path[MAX_PATH] = {0};

        remove_file_extension(paths[i], index_path);
        generate_indexed_tileset(paths[i],index_path);
    }
}

static void load_tileset(const char* tile_map_path, int tileset_index)
{
    FILE* fp_tileset = fopen(tile_map_path,"rb");
	
	int c;
    int i = 0;

	while((c = fgetc(fp_tileset)) != EOF)
    {
		tileset_list[tileset_index].data[i] = (unsigned char)c;
        
        ++i;
        if(i == MAX_TILESET_LENGTH)
            break;
    }

    fclose(fp_tileset);
}

static void load_all_tilesets()
{
	char paths[100][MAX_PATH] = { 0 };
	int num_files = get_files_in_directory_with_extension("data\\tilesets", ".tileset", paths);

    for(int i = 0; i < num_files; ++i)
    {
        char path_without_ext[MAX_PATH] = {0};
        char tileset_name[MAX_PATH] = {0};

        remove_file_extension(paths[i], path_without_ext);
        get_file_name(path_without_ext,tileset_name);

        C_strcpy(tileset_name, tileset_list[num_tilesets].name);
        load_tileset(paths[i],num_tilesets);

        num_tilesets++;
    }
}

static void draw_tile_tinted(int x, int y, const char* tileset_name, int tile_index,int tint_amount)
{
    unsigned char* dst = back_buffer;
    dst = dst + (buffer_width*y) + x;

    int tileset_index = get_tileset_index_by_name(tileset_name);
    if(tileset_index < 0)
        return;

    unsigned char* p_tileset = tileset_list[tileset_index].data;

    int tile_index_x = tile_index % TILES_PER_ROW;
    int tile_index_y = tile_index / TILES_PER_ROW;

	p_tileset = p_tileset + (TILE_WIDTH*TILE_HEIGHT*TILES_PER_ROW*tile_index_y) + tile_index_x*TILE_WIDTH;

    for (int i = 0; i < TILE_HEIGHT; ++i)
    {
        for (int j = 0; j < TILE_WIDTH; ++j)
        {
			if (dst >= (unsigned char*)back_buffer + (buffer_width*buffer_height)) return;
            if (p_tileset >= (tileset_list[tileset_index].data + MAX_TILESET_LENGTH)) return;

			if(*p_tileset != 0xFF)
            {
                if (dst >= (unsigned char*)back_buffer && x+j >= 0 && x+j < buffer_width)
                    *dst = tint_amount;
            }

			++dst;
			++p_tileset;
        }

        dst += (buffer_width - TILE_HEIGHT);
		p_tileset += (TILES_PER_ROW*TILE_WIDTH - TILE_WIDTH);
    }

}
 
static void draw_tile(int x, int y,const char* tileset_name, int tile_index,int shade_amount)
{
    unsigned char* dst = back_buffer;
    dst = dst + (buffer_width*y) + x;

    int tileset_index = get_tileset_index_by_name(tileset_name);

    if(tileset_index < 0)
        return;

    unsigned char* p_tileset = tileset_list[tileset_index].data;

    int tile_index_x = tile_index % TILES_PER_ROW;
    int tile_index_y = tile_index / TILES_PER_ROW;

	p_tileset = p_tileset + (TILE_WIDTH*TILE_HEIGHT*TILES_PER_ROW*tile_index_y) + tile_index_x*TILE_WIDTH;

    for (int i = 0; i < TILE_HEIGHT; ++i)
    {
        for (int j = 0; j < TILE_WIDTH; ++j)
        {
			if (dst >= (unsigned char*)back_buffer + (buffer_width*buffer_height)) return;
            if (p_tileset >= (tileset_list[tileset_index].data + MAX_TILESET_LENGTH)) return;

			if(*p_tileset != 0xFF)
            {
                if (dst >= (unsigned char*)back_buffer && x+j >= 0 && x+j < buffer_width)
                    *dst = min(254,*p_tileset + (16*shade_amount));
            }

			++dst;
			++p_tileset;
        }

        dst += (buffer_width - TILE_HEIGHT);
		p_tileset += (TILES_PER_ROW*TILE_WIDTH - TILE_WIDTH);
    }

}

static void draw_tile_shadow(int x, int y,const char* tileset_name, int tile_index,int shade_amount)
{
    unsigned char* dst = back_buffer;
    dst = dst + (buffer_width*y) + x;

    int tileset_index = get_tileset_index_by_name(tileset_name);
    if(tileset_index < 0)
        return;

    unsigned char* p_tileset = tileset_list[tileset_index].data;

    int tile_index_x = tile_index % TILES_PER_ROW;
    int tile_index_y = tile_index / TILES_PER_ROW;

	p_tileset = p_tileset + (TILE_WIDTH*TILE_HEIGHT*TILES_PER_ROW*tile_index_y) + tile_index_x*TILE_WIDTH;

    for (int i = 0; i < TILE_HEIGHT; ++i)
    {
        for (int j = 0; j < TILE_WIDTH; ++j)
        {
			if (dst >= (unsigned char*)back_buffer + (buffer_width*buffer_height)) return;
            if (p_tileset >= (tileset_list[tileset_index].data + MAX_TILESET_LENGTH)) return;

			if(*p_tileset != 0xFF)
            {
                if (dst >= (unsigned char*)back_buffer && x+j >= 0 && x+j < buffer_width)
                    *dst = min(*dst + (16*shade_amount),254);
            }

			++dst;
			++p_tileset;
        }

        dst += (buffer_width - TILE_HEIGHT);
		p_tileset += (TILES_PER_ROW*TILE_WIDTH - TILE_WIDTH);
    }

}

static void draw_tile_rotated(int x, int y,const char* tileset_name, int tile_index, float angle,int shade_amount)
{
    int tile_index_x = tile_index % TILES_PER_ROW;
    int tile_index_y = tile_index / TILES_PER_ROW;

    int tileset_index = get_tileset_index_by_name(tileset_name);
    if(tileset_index < 0)
        return;

    unsigned char* src = tileset_list[tileset_index].data;

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
                    if (dst >= (unsigned char*)back_buffer && x+i >= 0 && x+i < buffer_width)
                        *dst = *src_t + (16*shade_amount);
            }

            dst++;
			if (dst >= (unsigned char*)back_buffer + (buffer_width*buffer_height)) return;
        }

        dst += (buffer_width - TILE_HEIGHT);

    }
}

static void draw_tile_rotated_shadow(int x, int y, const char* tileset_name, int tile_index, float angle,int shade_amount)
{
    int tile_index_x = tile_index % TILES_PER_ROW;
    int tile_index_y = tile_index / TILES_PER_ROW;

    int tileset_index = get_tileset_index_by_name(tileset_name);
    if(tileset_index < 0)
        return;

    unsigned char* src = tileset_list[tileset_index].data;

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
                    if (dst >= (unsigned char*)back_buffer && x+i >= 0 && x+i < buffer_width)
                        *dst = min(*dst + (16*shade_amount),254);
            }

            dst++;
			if (dst >= (unsigned char*)back_buffer + (buffer_width*buffer_height)) return;
        }

        dst += (buffer_width - TILE_HEIGHT);

    }
}
