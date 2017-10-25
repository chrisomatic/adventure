#define MAX_ITEMS

typedef enum
{
    NONE,
    HEALTH
} ItemType;

typedef struct
{
    char* name;
    int x;
    int y;
    int tile_index;
    int value;
    ItemType type;
} Item;

Item items[MAX_ITEMS];
static int num_items;

static void spawn_item(Item i)
{
    items[num_items].name = i.name;
    items[num_items].x = i.x;
    items[num_items].y = i.y;
    items[num_items].tile_index = i.tile_index;
    items[num_items].value = i.value;
    items[num_items].type = i.type;
    
    num_items++;
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

    }
}

static void draw_items()
{
    for(int i = 0; i < num_items; ++i)
    {

    }
}
