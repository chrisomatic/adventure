#define MAX_OBJECTS     1000
#define MAX_OBJECT_LIST 100

typedef struct
{
    char* name;
    char* board_name;
    char* image_file_name;
    int image_x;
    int image_y;
    int x;
    int y;
    int width;
    int height
} Object;

Object objects[MAX_OBJECTS] = {0};
Object object_list[MAX_OBJECT_LIST] = {0};

static int num_objects = 0;
static int num_object_list = 0;

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
            object->name = object_list[i].name;
            object->board_name = object_list[i].board_name;
            object->image_file_name = object_list[i].image_file_name;
            object->image_x = object_list[i].image_x;
            object->image_y = object_list[i].image_y;
            object->width = object_list[i].width;
            object->height = object_list[i].height;

            return TRUE;
        }
    }

    // set object to "nothing"
    object->name = "";
    object->board_name = "";
    object->image_file_name = "";
    object->image_x = 0; 
    object->image_y = 0; 
    object->width = 0;
    object->height = 0; 

	return FALSE;
}

static BOOL spawn_object(const char* object_name,float x, float y)
{
    Object object = {0};

    if(!get_object_by_name(object_name,&object))
        return FALSE;

    objects[num_objects].name = object.name;
    objects[num_objects].board_name = object.board_name;
    objects[num_objects].image_file_name = object.image_file_name;
    objects[num_objects].image_x = object.image_x;
    objects[num_objects].image_y = object.image_y;
    objects[num_objects].width = object.width;
    objects[num_objects].height = object.height;
    objects[num_objects].x = x;
    objects[num_objects].y = y;

    ++num_objects;
    if(num_objects > MAX_OBJECTS -1)
    {
        num_objects = MAX_OBJECTS - 1;
        return FALSE;
    }

    return TRUE;
}

static void init_objects()
{
    num_objects = 0;
}

static void remove_object(int index)
{
	--num_objects;
	objects[index] = objects[num_objects];
}

static void draw_object(int i)
{

}

