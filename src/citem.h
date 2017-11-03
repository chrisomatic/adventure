#define MAX_ITEMS     1000
#define MAX_ITEM_LIST 100

typedef enum
{
    ITEM_TYPE_NONE,
    ITEM_TYPE_HEALTH,
    ITEM_TYPE_WEAPON,
    ITEM_TYPE_ARMOR,
    ITEM_TYPE_QUEST
} ItemType;

typedef struct
{
    char* name;
    char* description;
    float x;
    float y;
    float z;
    float x_vel;
    float y_vel;
    float z_vel;
    float friction;
    int  tile_index;
    int  value;
    BOOL highlighted;
    BOOL consummable;
    ItemType type;
} Item;

Item items[MAX_ITEMS];
Item item_list[MAX_ITEM_LIST];

static int num_items = 0;

static BOOL get_item_by_name(const char* name,Item* item)
{
    for(int i = 0; i < MAX_ITEM_LIST; ++i)
    {
		if (item_list[i].name == NULL)
			continue;

        if(strcmp(item_list[i].name, name) == 0)
        {
            item->name = item_list[i].name;
			item->description = item_list[i].description;
			item->value = item_list[i].value;
			item->tile_index = item_list[i].tile_index;
			item->consummable = item_list[i].consummable;
			item->type = item_list[i].type;

            return TRUE;
        }
    }
}

static BOOL spawn_item(const char* item_name,float x, float y)
{
    Item item = {0};

    if(!get_item_by_name(item_name,&item))
        return;

    items[num_items].name = item.name;
    items[num_items].description = item.description;
    items[num_items].x = x;
    items[num_items].y = y;
    items[num_items].z = 5.0f;
    items[num_items].x_vel = 0.0f;
    items[num_items].y_vel = 0.0f;
    items[num_items].z_vel = 2.0f;
    items[num_items].friction = AIR_RESISTANCE;
    items[num_items].tile_index = item.tile_index;
    items[num_items].highlighted = FALSE;
    items[num_items].consummable = item.consummable;
    items[num_items].type = item.type;
    items[num_items].value = item.value;

    ++num_items;
    if(num_items > MAX_ITEMS -1)
    {
        num_items = MAX_ITEMS - 1;
        return FALSE;
    }

    return TRUE;
    
}

static void init_items()
{
    num_items = 0;
}

static void remove_item(int index)
{
	--num_items;
	items[index] = items[num_items];
}

static void update_items()
{
    for(int i = 0; i < num_items; ++i)
    {
        if(i == player.item_held_index)
        {
            int item_offset_x;
            int item_offset_y;
            
            switch(player.dir)
            {
                case DIR_UP:
                    item_offset_x = 0;
                    item_offset_y = -5;
                    break;
                case DIR_DOWN:
                    item_offset_x = 0;
                    item_offset_y = +5;
                    break;
                case DIR_LEFT:
                    item_offset_x = -5;
                    item_offset_y = +1;
                    break;
                case DIR_RIGHT:
                    item_offset_x = +5;
                    item_offset_y = +1;
                    break;
            }
            items[i].x = player.x + item_offset_x;
            items[i].y = player.y + item_offset_y;
            items[i].z = player.z + 5;
        }
        else
        {
            items[i].x += items[i].x_vel;
            items[i].y += items[i].y_vel;
            items[i].z += items[i].z_vel;

            if(items[i].z < 0)
            {
                // hit the ground
                items[i].z = 0.0f;
                items[i].z_vel = 0.0f;
                items[i].friction = GROUND_FRICTION;
            }
            else
            {
                items[i].z_vel -= GRAVITY; // gravity acceleration
            }

            if(items[i].x_vel != 0 || items[i].y_vel != 0)
            {
                // air resistance
                if(items[i].x_vel < 0) items[i].x_vel += items[i].friction;
                else items[i].x_vel -= items[i].friction;
                if(items[i].y_vel < 0) items[i].y_vel += items[i].friction;
                else items[i].y_vel -= items[i].friction;

                if(abs(items[i].x_vel) <= 0.2f) items[i].x_vel = 0.0f;
                if(abs(items[i].y_vel) <= 0.2f) items[i].y_vel = 0.0f;
            }
        }

    }
}

static void draw_item(int i)
{
		draw_tile_shadow(items[i].x - camera.x, items[i].y - camera.y, items[i].tile_index,max(0,10 - day_cycle_shade_amount)); // shadow

        if(items[i].highlighted)
        {
            draw_tile(items[i].x - camera.x, items[i].y - camera.y - items[i].z*0.5f, items[i].tile_index,max(0,day_cycle_shade_amount-3));
        }
        else
        {
            draw_tile(items[i].x - camera.x, items[i].y - camera.y - items[i].z*0.5f, items[i].tile_index,day_cycle_shade_amount);
        }
}

static void draw_items()
{
    for(int i = 0; i < num_items; ++i)
    {
        draw_item(i);
    }
}
