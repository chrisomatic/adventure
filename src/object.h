#pragma once

#define MAX_OBJECTS     1000
#define MAX_OBJECT_LIST 100

typedef struct
{
    char name[MAX_PATH];
    char* board_name;
    unsigned char data[256*256];
    float x;
    float y;
    int width;
	int height;
} Object;


Object objects[MAX_OBJECTS] = {0};
Object object_list[MAX_OBJECT_LIST] = {0};

static int num_objects = 0;
static int num_object_list = 0;

static void generate_object_from_image_file(const char* rgb_image_path)
{
    int w,h,n;
    unsigned char *imgdata = stbi_load(rgb_image_path,&w,&h,&n,0);

    if(imgdata == NULL)
        return;

    char _name[MAX_PATH] = {0};
	get_file_name(rgb_image_path, _name);
	str_replace(_name, length_of(_name), ".object.png", "", _name);

    strncpy(object_list[num_object_list].name, _name,MAX_PATH);

    object_list[num_object_list].board_name = "";
    object_list[num_object_list].x = 0;
    object_list[num_object_list].y = 0;
    object_list[num_object_list].width = w;
    object_list[num_object_list].height = h;

    unsigned char *p = imgdata;

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

            object_list[num_object_list].data[j*w + i] = min_delta_index;
            p+=n;
        }
    }

    ++num_object_list;
    
    stbi_image_free(imgdata);
}

static void generate_all_objects()
{
	char paths[100][MAX_PATH] = { 0 };
	int num_files = get_files_in_directory_with_extension("data\\objects", ".object.png", paths);

    for(int i = 0; i < num_files; ++i)
    {
		char index_path[MAX_PATH] = {0};
        generate_object_from_image_file(paths[i]);
    }
}

static int get_object_index_by_name(const char* name)
{
	if (name == NULL)
		return -1;

    for(int i = 0; i < MAX_OBJECT_LIST; ++i)
    {
		if (object_list[i].name == NULL)
			continue;

        if(strcmp(object_list[i].name, name) == 0)
            return i;
    }

    return -1;
}

static BOOL get_object_by_name(const char* name,Object* object)
{
	if (name == NULL)
		return FALSE;

    for(int i = 0; i < MAX_OBJECT_LIST; ++i)
    {
		if (object_list[i].name == NULL)
			continue;

        if(strcmp(object_list[i].name, name) == 0)
        {
			strncpy(object->name, object_list[i].name, MAX_PATH);
            object->board_name = object_list[i].board_name;
			memcpy(object->data, object_list[i].data, 256 * 256);
            object->width = object_list[i].width;
            object->height = object_list[i].height;

            return TRUE;
        }
    }

	return FALSE;
}

static BOOL spawn_object(const char* object_name,const char* board_name,int x, int y)
{
    Object object = {0};

    if(!get_object_by_name(object_name,&object))
        return FALSE;

	strncpy(objects[num_objects].name, object.name, MAX_PATH);
    objects[num_objects].board_name = board_name;
    objects[num_objects].width = object.width;
    objects[num_objects].height = object.height;
    objects[num_objects].x = (float)x;
    objects[num_objects].y = (float)y;

    ++num_objects;
    if(num_objects > MAX_OBJECTS -1)
    {
        num_objects = MAX_OBJECTS - 1;
        return FALSE;
    }

    return TRUE;
}

static void remove_object(int index)
{
	--num_objects;
	objects[index] = objects[num_objects];
}

static void draw_object(int i)
{
    Object* o = &objects[i];
	int x = o->x - camera.x;
	int y = o->y - camera.y;

    unsigned char* dst = back_buffer;
	dst = dst + (int)(buffer_width*y + x);

    int shade_amount = day_cycle_shade_amount;

    int object_index = get_object_index_by_name(objects[i].name);

    if(object_index < 0)
        return;

    unsigned char* p_object = object_list[object_index].data;

    for(int i = 0; i < o->height; ++i)
    {
        for(int j = 0; j < o->width; ++j)
        {
			if (dst >= (unsigned char*)back_buffer + (buffer_width*buffer_height)) return;

            if(*p_object != 0xFF)
            {
                if (dst >= (unsigned char*)back_buffer && x+j >= 0 && x+j < buffer_width)
                    *dst = min(254,*p_object + (16*shade_amount));
            }

			++dst;
			++p_object;
        }
        dst += (buffer_width - o->width);
    }
}

