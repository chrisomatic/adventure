#define MAX_ONSCREEN_ENTITIES 10000

typedef struct
{
    EntityType type;
    int index;
    int y_value;
} SortEntity;

SortEntity entities[MAX_ONSCREEN_ENTITIES];
int num_entities = 0;

static void swap(SortEntity* a, SortEntity* b)
{
    SortEntity t = *a;
    *a = *b;
    *b = t;
}
 
static int partition (SortEntity arr[], int low, int high)
{
    SortEntity pivot = arr[high];
    int i = (low - 1);
 
    for (int j = low; j <= high- 1; j++)
    {
        if (arr[j].y_value <= pivot.y_value)
        {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}
 
static void quick_sort(SortEntity arr[], int low, int high)
{
    if (low < high)
    {
        int pi = partition(arr, low, high);
 
        quick_sort(arr, low, pi - 1);
        quick_sort(arr, pi + 1, high);
    }
}

static void sort_entities()
{
    num_entities = 0;

    int screen_x = 0;
    int screen_y = 0;
    
    // items
    for(int i = 0; i < num_items;++i)
    {
		screen_x = items[i].phys.x - camera.x;
		screen_y = items[i].phys.y - camera.y;
        
        if (screen_x+TILE_WIDTH < 0 || screen_x >= buffer_width)
            continue;
        if (screen_y+TILE_HEIGHT < 0 || screen_y >= buffer_height)
            continue;

        entities[num_entities].type = ENTITY_TYPE_ITEM;
        entities[num_entities].index = i;
        entities[num_entities].y_value = screen_y;
        num_entities++;
    }
    
    // coins
    for(int i = 0; i < num_coins;++i)
    {
		screen_x = coins[i].phys.x - camera.x;
		screen_y = coins[i].phys.y - camera.y;
        
        if (screen_x+TILE_WIDTH < 0 || screen_x >= buffer_width)
            continue;
        if (screen_y+TILE_HEIGHT < 0 || screen_y >= buffer_height)
            continue;

        entities[num_entities].type = ENTITY_TYPE_COIN;
        entities[num_entities].index = i;
        entities[num_entities].y_value = screen_y;
        num_entities++;
    }
    
    // creatures
    for(int i = 0; i < num_creatures;++i)
    {
        screen_x = creatures[i].phys.x - camera.x;
        screen_y = creatures[i].phys.y - camera.y;
        
        if (screen_x+TILE_WIDTH < 0 || screen_x >= buffer_width)
            continue;
        if (screen_y+TILE_HEIGHT < 0 || screen_y >= buffer_height)
            continue;

        entities[num_entities].type = ENTITY_TYPE_CREATURE;
        entities[num_entities].index = i;
        entities[num_entities].y_value = screen_y;
        num_entities++;
    }
    
    // npcs
    for(int i = 0; i < num_npcs;++i)
    {
		screen_x = npcs[i].phys.x - camera.x;
		screen_y = npcs[i].phys.y - camera.y;
        
        if (screen_x+TILE_WIDTH < 0 || screen_x >= buffer_width)
            continue;
        if (screen_y+TILE_HEIGHT < 0 || screen_y >= buffer_height)
            continue;

        entities[num_entities].type = ENTITY_TYPE_NPC;
        entities[num_entities].index = i;
        entities[num_entities].y_value = screen_y;
        num_entities++;
    }
    
    // particles 
    for(int i = 0; i < num_particles;++i)
    {
        screen_x = particles[i].x - camera.x;
        screen_y = particles[i].y - camera.y;
        
        if (screen_x+TILE_WIDTH < 0 || screen_x >= buffer_width)
            continue;
        if (screen_y+TILE_HEIGHT < 0 || screen_y >= buffer_height)
            continue;

        entities[num_entities].type = ENTITY_TYPE_PARTICLE;
        entities[num_entities].index = i;
        entities[num_entities].y_value = screen_y;
        num_entities++;
    }
    
    // projectiles
    for(int i = 0; i < num_projectiles;++i)
    {
		screen_x = projectiles[i].phys.x - camera.x;
		screen_y = projectiles[i].phys.y - camera.y;
        
        if (screen_x+TILE_WIDTH < 0 || screen_x >= buffer_width)
            continue;
        if (screen_y+TILE_HEIGHT < 0 || screen_y >= buffer_height)
            continue;

        entities[num_entities].type = ENTITY_TYPE_PROJECTILE;
        entities[num_entities].index = i;
        entities[num_entities].y_value = screen_y;
        num_entities++;
    }
    
    // floating numbers 
    for(int i = 0; i < num_floating_numbers;++i)
    {
        screen_x = floating_numbers[i].x - camera.x;
        screen_y = floating_numbers[i].y - camera.y;
        
        if (screen_x+TILE_WIDTH < 0 || screen_x >= buffer_width)
            continue;
        if (screen_y+TILE_HEIGHT < 0 || screen_y >= buffer_height)
            continue;

        entities[num_entities].type = ENTITY_TYPE_FLOATING_NUMBER;
        entities[num_entities].index = i;
        entities[num_entities].y_value = screen_y;
        num_entities++;
    }

    // player
    entities[num_entities].type = ENTITY_TYPE_PLAYER;
    entities[num_entities].index = 0;
    entities[num_entities].y_value = player.phys.y - camera.y;
    num_entities++;

    // sort entities
    quick_sort(entities,0,num_entities-1);
}

static void draw_entities()
{
    for(int i = 0; i < num_entities;++i)
    {
        switch(entities[i].type)
        {
            case ENTITY_TYPE_PLAYER:
                draw_player();
                break;
            case ENTITY_TYPE_ITEM:
                draw_item(entities[i].index);
                break;
            case ENTITY_TYPE_COIN:
                draw_coin(entities[i].index);
                break;
            case ENTITY_TYPE_CREATURE:
                draw_creature(entities[i].index);
                break;
            case ENTITY_TYPE_NPC:
                draw_npc(entities[i].index);
                break;
            case ENTITY_TYPE_PARTICLE:
                draw_particle(entities[i].index);
                break;
            case ENTITY_TYPE_PROJECTILE:
                draw_projectile(entities[i].index);
                break;
            case ENTITY_TYPE_FLOATING_NUMBER:
                draw_floating_number(entities[i].index);
                break;
        }
    }
}

static BOOL are_entities_colliding(PhysicalProperties* phys_a, PhysicalProperties* phys_b)
{
    BoundingBox a,b;

    a.x = phys_a->x + TILE_WIDTH/4.0f;
    a.y = phys_a->y + TILE_HEIGHT/4.0f;
    a.z = phys_a->z;

    a.width  = TILE_WIDTH/2.0f;
    a.length = TILE_HEIGHT/2.0f;
    a.height = phys_a->height;

    b.x = phys_b->x;
    b.y = phys_b->y;
    b.z = phys_b->z;

    b.width  = TILE_WIDTH;
    b.length = TILE_HEIGHT/2.0f;
    b.height = phys_b->height;

    if(a.x <= (b.x + b.width) && (a.x + a.width) >= b.x)
        if(a.y <= (b.y + b.length) && (a.y + a.length) >= b.y)
            if(a.z <= (b.z + b.height) && (a.z + a.height) >= b.z)
                return TRUE;

    return FALSE;
}
