#define MAX_ITEMS 1000

typedef enum
{
    ITEM_MEAT
} ItemIndex;

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
    float x;
    float y;
    float z;
    float x_vel;
    float y_vel;
    float z_vel;
    float friction;
    int tile_index;
    BOOL consummable;
    ItemType type;
} Item;

Item items[MAX_ITEMS];
static int num_items;

static BOOL spawn_item(ItemIndex i,float x, float y)
{
    switch(i)
    {
        case ITEM_MEAT:
            items[num_items].name = "Meat";
            items[num_items].x = x;
            items[num_items].y = y;
            items[num_items].z = 5.0f;
            items[num_items].x_vel = 0.0f;
            items[num_items].y_vel = 0.0f;
            items[num_items].z_vel = 2.0f;
            items[num_items].friction = 0.05f;
            items[num_items].tile_index = 112;
            items[num_items].consummable = TRUE;
            items[num_items].type = ITEM_TYPE_HEALTH;

            break;
        default:
            return;
    }
    
    num_items++;

    if(num_items > MAX_ITEMS -1)
    {
        num_items = MAX_ITEMS - 1;
        return FALSE;
    }

    return TRUE;
    
}

static void remove_item(int index)
{
	num_items--;
	items[index] = items[num_items];
}

static void update_items()
{
    for(int i = 0; i < num_items; ++i)
    {
        items[i].x += items[i].x_vel;
        items[i].y += items[i].y_vel;
        items[i].z += items[i].z_vel;

        if(items[i].z < 0)
        {
            // hit the ground
            items[i].z = 0.0f;
            items[i].z_vel = 0.0f;
			items[i].friction = 0.1f;
        }
		else
		{
			items[i].z_vel -= GRAVITY; // gravity acceleration
		}

        if(items[i].x_vel != 0 || items[i].y_vel != 0)
        {
            // air resistance
            if(items[i].x_vel < 0) items[i].x_vel += coins[i].friction;
            else items[i].x_vel -= items[i].friction;
            if(items[i].y_vel < 0) items[i].y_vel += coins[i].friction;
            else items[i].y_vel -= items[i].friction;

            if(abs(items[i].x_vel) <= 0.1f) items[i].x_vel = 0.0f;
            if(abs(items[i].y_vel) <= 0.1f) items[i].y_vel = 0.0f;
        }

    }
}

static void draw_items()
{
    for(int i = 0; i < num_items; ++i)
    {
		draw_tile(items[i].x - camera.x, items[i].y - camera.y, items[i].tile_index,254); // shadow
		draw_tile(items[i].x - camera.x, items[i].y - camera.y - items[i].z*0.5f, items[i].tile_index,day_cycle_shade_amount); // shadow

    }
}
