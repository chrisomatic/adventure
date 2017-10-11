#define FONTDATA_WIDTH    96 
#define FONTDATA_HEIGHT   49 
#define GLYPH_WIDTH       5
#define GLYPH_HEIGHT      6

#define CHAR_MISSILE      127
#define CHAR_MISSILE_R    128
#define CHAR_SHIELD       129
#define CHAR_INFINITY     130
#define CHAR_FLAME        131
#define CHAR_MISSILE_M    132
#define CHAR_HEART        133 

//    int x,y,n;
//    unsigned char *data = stbi_load(filename, &x, &y, &n, 0);
//    // ... process data if not NULL ...
//    // ... x = width, y = height, n = # 8-bit components per pixel ...
//    // ... replace '0' with '1'..'4' to force that many components per pixel
//    // ... but 'n' will always be the number that it would have been if you said 0
//    stbi_image_free(data)

unsigned char *fontdata;
int fontdata_x;
int fontdata_y;
int fontdata_n;

BOOL font_initialized = FALSE;

BOOL init_font(const char* filename)
{
    fontdata = stbi_load(filename,&fontdata_x,&fontdata_y,&fontdata_n,1);

    if(fontdata == NULL)
        return FALSE;

    if(fontdata_x != FONTDATA_WIDTH || fontdata_y != FONTDATA_HEIGHT)
        return FALSE;

    font_initialized = TRUE;

	return TRUE;
}

void free_font()
{
	stbi_image_free(fontdata);
}

void draw_char_scaled(const unsigned char c, int x, int y, float scale_factor, char color)
{
    if (!font_initialized)
        return;

    unsigned char* dst = back_buffer;
    dst = dst + (buffer_width*y) + x;

    int relative_char_position = c - 32;

    int src_x = (relative_char_position % 16)*(GLYPH_WIDTH + 1);
    int src_y = (relative_char_position / 16)*(GLYPH_HEIGHT + 1);

	unsigned char* src = fontdata;
	src = src + (FONTDATA_WIDTH*src_y) + src_x;

    float src_x_tracker = 0.0f;
    float src_y_tracker = 0.0f;

    for(float i = 0; i < ceil(GLYPH_HEIGHT*scale_factor); i += 1)
	{
		for (float j = 0; j < ceil(GLYPH_WIDTH*scale_factor); j += 1)
		{
			if (dst < back_buffer || dst >(unsigned char*)back_buffer + (buffer_width*buffer_height))
				return;

			if(*src > 0)
				*dst = color;
			++dst;

            src_x_tracker += (1/scale_factor);
            if(src_x_tracker >= 1.0f)
            {
                src_x_tracker -= 1.0f;
                ++src;
            }
		}

        src_y_tracker += (1/scale_factor);
        if(src_y_tracker >= 1.0f)
        {
            src_y_tracker -= 1.0f;
            src+=(FONTDATA_WIDTH - GLYPH_WIDTH);
        }
        else
        {
            src -= GLYPH_WIDTH;
        }

		dst+= (int)(buffer_width - ceil(GLYPH_WIDTH*scale_factor));
    }

}
void draw_char(const char c, int x, int y, char color)
{
    if (!font_initialized)
        return;

    unsigned char* dst = back_buffer;
    dst = dst + (buffer_width*y) + x;

    int relative_char_position = c - 32;

    int src_x = (relative_char_position % 16)*(GLYPH_WIDTH + 1);
    int src_y = (relative_char_position / 16)*(GLYPH_HEIGHT + 1);

	unsigned char* src = fontdata;
	src = src + (FONTDATA_WIDTH*src_y) + src_x;

    for(int i = 0; i < GLYPH_HEIGHT; ++i)
	{
		for (int j = 0; j < GLYPH_WIDTH; ++j)
		{
			if (dst < back_buffer || dst >(unsigned char*)back_buffer + (buffer_width*buffer_height))
				return;

			if(*src > 0)
				*dst = color;
			++dst; ++src;
		}

        src+=(FONTDATA_WIDTH - GLYPH_WIDTH);
		dst+=(buffer_width - GLYPH_WIDTH);
    }
}

static void draw_string(char *s, int x, int y, float scale, char color)
{
	char* p = s;

	while (*p)
	{
        if(scale == 1.0f)
            draw_char(*p++, x, y, color);
        else
            draw_char_scaled(*p++, x, y, scale, color);

		x += ceil(GLYPH_WIDTH*scale) + 1;
	}
}
