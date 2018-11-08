#pragma once

#define MAX_ITEM_STANDS 100

typedef struct
{
    float x;
    float y;
    int board_index;
    int item_lifetime_counter;
    int vendor_index;
    int item_index;
} ItemStand;

ItemStand item_stands[MAX_ITEM_STANDS];
int num_item_stands = 0;

static void spawn_random_item(int i)
{
    int rand_item = rand() % num_item_list;
    spawn_item(item_list[rand_item].name,item_stands[i].x, item_stands[i].y, item_stands[i].board_index);
	item_stands[i].item_index = num_items - 1;
    items[item_stands[i].item_index].mounted = TRUE;
    items[item_stands[i].item_index].vendor_index = item_stands[i].vendor_index;
}

static void spawn_item_stand(float x, float y,int board_index, int vendor_index)
{
    item_stands[num_item_stands].x = x;
    item_stands[num_item_stands].y = y;
    item_stands[num_item_stands].board_index = board_index;
    item_stands[num_item_stands].item_lifetime_counter = 0;
    item_stands[num_item_stands].vendor_index = vendor_index;

    spawn_random_item(num_item_stands);

    ++num_item_stands;
}

static void init_item_stands()
{
	int vendor_index = get_creature_index_by_name("Gary");
    spawn_item_stand(1232,896,current_board_index, vendor_index);
    spawn_item_stand(1232,912,current_board_index, vendor_index);
    spawn_item_stand(1232,928,current_board_index, vendor_index);
}

static void update_item_stands()
{
    for(int i = 0; i < num_item_stands; ++i)
    {
        ++item_stands[i].item_lifetime_counter;

        if(item_stands[i].item_lifetime_counter >= DAY_CYCLE_COUNTER_MAX)
        {
            // spawn a new item
            item_stands[i].item_lifetime_counter = 0;
            remove_item(item_stands[i].item_index);
            spawn_random_item(i);
        }
    }
}
