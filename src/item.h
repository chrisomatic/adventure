#define MAX_ITEMS     1000
#define MAX_ITEM_LIST 100

#define BOW 3
#define STAFF 5
#define CROSSBOW 7

Item items[MAX_ITEMS] = {0};
Item item_list[MAX_ITEM_LIST] = {0};

static int num_items = 0;
static int num_item_list = 0;

static BOOL get_item_by_name(const char* name,Item* item)
{
	if (name == NULL)
		return FALSE;

    for(int i = 0; i < MAX_ITEM_LIST; ++i)
    {
		if (item_list[i].name == NULL)
			continue;

        if(strcmp(item_list[i].name, name) == 0)
        {
            item->name = item_list[i].name;
			item->description = item_list[i].description;
			item->value = item_list[i].value;
			item->tileset_name = item_list[i].tileset_name;
			item->tile_index = item_list[i].tile_index;
			item->consummable = item_list[i].consummable;
			item->type = item_list[i].type;
            item->weapon_props.attack_speed = item_list[i].weapon_props.attack_speed;
            item->weapon_props.attack_range = item_list[i].weapon_props.attack_range;
            item->weapon_props.min_damage = item_list[i].weapon_props.min_damage;
            item->weapon_props.max_damage = item_list[i].weapon_props.max_damage;
            item->weapon_props.weapon_type = item_list[i].weapon_props.weapon_type;
            item->armor_props.defence = item_list[i].armor_props.defence;
            item->armor_props.armor_type = item_list[i].armor_props.armor_type;
            item->armor_props.y_offset = item_list[i].armor_props.y_offset;

            return TRUE;
        }
    }

    // set item to "nothing"
    item->name = "";
    item->description = "";
    item->value = 0;
    item->tileset_name = "";
    item->tile_index = 0;
    item->type = 0;
    item->weapon_props.attack_speed = 1.0f;
    item->weapon_props.attack_range = 1;
    item->weapon_props.min_damage = 0;
    item->weapon_props.max_damage = 0;
    item->weapon_props.weapon_type = 0;
    item->armor_props.defence = 0.0f;
    item->armor_props.armor_type = ARMOR_TYPE_NONE; 
    item->armor_props.y_offset = 0; 

	return FALSE;
}

static BOOL spawn_item(const char* item_name,float x, float y, int board_index)
{
    Item item = {0};

    if(!get_item_by_name(item_name,&item))
        return FALSE;

    items[num_items].name = item.name;
    items[num_items].board_index = board_index;
    items[num_items].description = item.description;
    items[num_items].phys.x = x;
    items[num_items].phys.y = y;
    items[num_items].phys.z = 5.0f;
    items[num_items].phys.x_vel = 0.0f;
    items[num_items].phys.y_vel = 0.0f;
    items[num_items].phys.z_vel = 2.0f;
    items[num_items].phys.speed = 1.0f;
    items[num_items].phys.base_speed = 1.0f;
    items[num_items].friction = AIR_RESISTANCE;
    items[num_items].tileset_name = item.tileset_name;
    items[num_items].tile_index = item.tile_index;
    items[num_items].highlighted = FALSE;
    items[num_items].consummable = item.consummable;
    items[num_items].type = item.type;
    items[num_items].value = item.value;
    items[num_items].weapon_props.attack_speed = item.weapon_props.attack_speed;
    items[num_items].weapon_props.attack_range = item.weapon_props.attack_range;
    items[num_items].weapon_props.min_damage = item.weapon_props.min_damage;
    items[num_items].weapon_props.max_damage = item.weapon_props.max_damage;
    items[num_items].weapon_props.weapon_type = item.weapon_props.weapon_type;
    items[num_items].armor_props.defence = item.armor_props.defence;
    items[num_items].armor_props.armor_type = item.armor_props.armor_type;
    items[num_items].armor_props.y_offset = item.armor_props.y_offset;

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
			items[i].phys.x = player.phys.x + item_offset_x;
			items[i].phys.y = player.phys.y + item_offset_y;
			items[i].phys.z = player.phys.z + 5;
        }
        else
        {
            items[i].phys.x += items[i].phys.x_vel;
            items[i].phys.y += items[i].phys.y_vel;
            items[i].phys.z += items[i].phys.z_vel;

            handle_terrain_collision(items[i].board_index,&items[i].phys);

			if (items[i].phys.z < 0)
            {
                // hit the ground
                items[i].phys.z = 0.0f;
                items[i].phys.z_vel = 0.0f;
                items[i].friction = GROUND_FRICTION;
            }
            else
            {
                items[i].phys.z_vel -= GRAVITY; // gravity acceleration
            }

            if(items[i].phys.x_vel != 0 || items[i].phys.y_vel != 0)
            {
                // air resistance
                if(items[i].phys.x_vel < 0) items[i].phys.x_vel += items[i].friction;
                else items[i].phys.x_vel -= items[i].friction;
                if(items[i].phys.y_vel < 0) items[i].phys.y_vel += items[i].friction;
                else items[i].phys.y_vel -= items[i].friction;

                if(abs(items[i].phys.x_vel) <= 0.2f) items[i].phys.x_vel = 0.0f;
                if(abs(items[i].phys.y_vel) <= 0.2f) items[i].phys.y_vel = 0.0f;
            }
        }

    }
}

static void draw_item(int i)
{
    draw_tile_shadow(items[i].phys.x - camera.x, items[i].phys.y - camera.y, items[i].tileset_name,items[i].tile_index,max(0,10 - day_cycle_shade_amount)); // shadow

    if(items[i].highlighted)
        draw_tile(items[i].phys.x - camera.x, items[i].phys.y - camera.y - items[i].phys.z*0.5f,items[i].tileset_name, items[i].tile_index,max(0,day_cycle_shade_amount-3));
    else
        draw_tile(items[i].phys.x - camera.x, items[i].phys.y - camera.y - items[i].phys.z*0.5f,items[i].tileset_name, items[i].tile_index,day_cycle_shade_amount);
}
