#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG 
#include "stb_image.h"

//    int x,y,n;
//    unsigned char *data = stbi_load(filename, &x, &y, &n, 0);
//    // ... process data if not NULL ...
//    // ... x = width, y = height, n = # 8-bit components per pixel ...
//    // ... replace '0' with '1'..'4' to force that many components per pixel
//    // ... but 'n' will always be the number that it would have been if you said 0
//    stbi_image_free(data)

static int palette_num_channels;

static BOOL draw_image(const char* image_path,int x, int y,float scale)
{
    int w,h,n;
    unsigned char *imgdata = stbi_load(image_path,&w,&h,&n,1);

    if(imgdata == NULL)
        return FALSE;

    unsigned char* dst = back_buffer;
    dst = dst + (buffer_width*y) + x;

    int dst_w = w*scale;
    int dst_h = h*scale;

    if( x < 0 || x + dst_w > buffer_width)  return;
    if( y < 0 || y + dst_h > buffer_height) return;

    for(int i = 0; i < dst_w;++i)
    {
        for(int j = 0; j < dst_h;++j)
        {

            int index = floor((i*dst_w + j)/scale);
            unsigned char color = *(imgdata + index);

            *dst++ = color;
        }
    }
    stbi_image_free(imgdata);

    return TRUE;
}


static void generate_palette_file(const char* image_path)
{
    int w,h,n;
    unsigned char *imgdata = stbi_load(image_path,&w,&h,&n,0);

	palette_num_channels = n;

    if(imgdata == NULL)
        return;

    unsigned char *p = imgdata;

    FILE * fp;
    fp = fopen ("data\\palette", "wb");

    for(int j = 0; j < h; ++j)
    {
        for(int i = 0; i < w; ++i)
        {
            for(int k = 0; k < n; ++k)
            {
                fprintf(fp,"%c",*p++);
            }
        }
    }
    
    fclose(fp);
    stbi_image_free(imgdata);
}



static void draw_rect8(int x, int y, int w, int h, char color, BOOL filled)
{
    unsigned char* dst = (unsigned char*)back_buffer;
    dst = dst + (buffer_width*y) + x;

    if (dst < (unsigned char*)back_buffer)
        return;
    if (dst + (buffer_width*h)+w > (unsigned char*)back_buffer + (buffer_width*(buffer_height + 1)))
        return;

    if(filled)
    {
        for(int i = 0; i < h; ++i)
        {
            memset(dst,color,w);
            dst += buffer_width;
        }
    }
    else
    {
        //top line
        memset(dst,color,w);
        dst += buffer_width;

        for(int i = 0; i < h; ++i)
        {
            *dst = color;
            dst+= w;
            *dst = color;
            dst += (buffer_width - w);
        }
        
        //bottom line
        memset(dst,color,w);
    }
}

static void shade_pixel8(int x, int y,int shade_amount)
{
	// c is brightness. 0 <= c <= 1
	unsigned char* dst = (unsigned char*)back_buffer;
	dst = dst + (buffer_width*y) + x;

	if (dst < (unsigned char*)back_buffer)
		return;

	if (dst > (unsigned char*)back_buffer + (buffer_width*buffer_height))
		return;

    if (x < 0 || x >= buffer_width)
        return;

	int current = *dst;
	int offset = (16 * shade_amount);

	*dst = min(current + offset,254);
}

static void draw_pixel8(int x, int y, unsigned char color)
{
	// c is brightness. 0 <= c <= 1
	unsigned char* dst = (unsigned char*)back_buffer;
	dst = dst + (buffer_width*y) + x;

	if (dst < (unsigned char*)back_buffer)
		return;

	if (dst > (unsigned char*)back_buffer + (buffer_width*buffer_height))
		return;

    if (x < 0 || x >= buffer_width)
        return;
    
	*dst = color;
}

static void draw_line2(int x, int y, int x2, int y2,char color) {
   	BOOL yLonger=FALSE;
	int shortLen=y2-y;
	int longLen=x2-x;
	if (abs(shortLen)>abs(longLen)) {
		int swap=shortLen;
		shortLen=longLen;
		longLen=swap;				
		yLonger=TRUE;
	}
	int decInc;
	if (longLen==0) decInc=0;
	else decInc = (shortLen << 16) / longLen;

	if (yLonger) {
		if (longLen>0) {
			longLen+=y;
			for (int j=0x8000+(x<<16);y<=longLen;++y) {
                draw_pixel8(j>>16,y,color);
				j+=decInc;
			}
			return;
		}
		longLen+=y;
		for (int j=0x8000+(x<<16);y>=longLen;--y) {
            draw_pixel8(j>>16,y,color);
			j-=decInc;
		}
		return;	
	}

	if (longLen>0) {
		longLen+=x;
		for (int j=0x8000+(y<<16);x<=longLen;++x) {
            draw_pixel8(x,j>>16,color);
			j+=decInc;
		}
		return;
	}
	longLen+=x;
	for (int j=0x8000+(y<<16);x>=longLen;--x) {
        draw_pixel8(x,j>>16,color);
		j-=decInc;
	}

}
// THE EXTREMELY FAST LINE ALGORITHM Variation C (Addition)
static void draw_line(int x, int y, int x2, int y2, char color)
{
	BOOL yLonger = FALSE;
	int incrementVal, endVal;

	int shortLen = y2 - y;
	int longLen = x2 - x;
	if (abs(shortLen)>abs(longLen)) {
		int swap = shortLen;
		shortLen = longLen;
		longLen = swap;
		yLonger = TRUE;
	}

	endVal = longLen;
	if (longLen<0) {
		incrementVal = -1;
		longLen = -longLen;
	}
	else incrementVal = 1;

	double decInc;
	if (longLen == 0) decInc = (double)shortLen;
	else decInc = ((double)shortLen / (double)longLen);
	double j = 0.0;
	if (yLonger) {
		for (int i = 0; i != endVal; i += incrementVal) {
			draw_pixel8(x + (int)j, y + i, color);
			j += decInc;
		}
	}
	else {
		for (int i = 0; i != endVal; i += incrementVal) {
			draw_pixel8(x + i, y + (int)j, color);
			j += decInc;
		}
	}
}

static void draw_circle(int x0, int y0, int r, int color, BOOL filled)
{
	int x = r;
	int y = 0;
	int err = 0;

	while (x >= y)
	{
		if (filled)
		{
			draw_line(x0 + x, y0 + y, x0 - x, y0 + y, color);
			draw_line(x0 + y, y0 + x, x0 - y, y0 + x, color);
			draw_line(x0 - x, y0 - y, x0 + x, y0 - y, color);
			draw_line(x0 - y, y0 - x, x0 + y, y0 - x, color);
		}
		else
		{
			draw_pixel8(x0 + x, y0 + y, color);
			draw_pixel8(x0 + y, y0 + x, color);
			draw_pixel8(x0 - y, y0 + x, color);
			draw_pixel8(x0 - x, y0 + y, color);
			draw_pixel8(x0 - x, y0 - y, color);
			draw_pixel8(x0 - y, y0 - x, color);
			draw_pixel8(x0 + y, y0 - x, color);
			draw_pixel8(x0 + x, y0 - y, color);
		}
		

		if (err <= 0)
		{
			y += 1;
			err += 2 * y + 1;
		}
		if (err > 0)
		{
			x -= 1;
			err -= 2 * x + 1;
		}
	}
}

static void draw_ellipse(int origin_x,int origin_y, int w, int h, int color, BOOL filled)
{
	int hh = h * h;
	int ww = w * w;
	int hhww = hh * ww;
	int x0 = w;
	int dx = 0;

	// do the horizontal diameter
	if (filled)
		for (int x = -w; x <= w; x++)
			draw_pixel8(origin_x + x, origin_y, color);
	else
	{
		draw_pixel8(origin_x - w, origin_y, color);
		draw_pixel8(origin_x + w, origin_y, color);
	}

	// now do both halves at the same time, away from the diameter
	for (int y = 1; y <= h; y++)
	{
		int x1 = x0 - (dx - 1);  // try slopes of dx - 1 or more
		for (; x1 > 0; x1--)
			if (x1*x1*hh + y*y*ww <= hhww)
				break;
		dx = x0 - x1;  // current approximation of the slope
		x0 = x1;

		if (filled)
		{
			for (int x = -x0; x <= x0; x++)
			{
				draw_pixel8(origin_x + x, origin_y - y, color);
				draw_pixel8(origin_x + x, origin_y + y, color);
			}
		}
		else
		{
			if (dx <= 0)
			{

				draw_pixel8(origin_x - x0, origin_y - y, color);
				draw_pixel8(origin_x - x0, origin_y + y, color);
				draw_pixel8(origin_x + x0, origin_y - y, color);
				draw_pixel8(origin_x + x0, origin_y + y, color);
			}
			else
			{
				for (int i = 0; i < dx; ++i)
				{
					draw_pixel8(origin_x - x0 - i, origin_y - y, color);
					draw_pixel8(origin_x - x0 - i, origin_y + y, color);
					draw_pixel8(origin_x + x0 + i, origin_y - y, color);
					draw_pixel8(origin_x + x0 + i, origin_y + y, color);
				}
			}

		}
	}
}
