#define MAX_ITEMS     10000
#define MAX_ITEM_LIST 100

#define BOW 3
#define STAFF 5
#define CROSSBOW 7
#define POISON_SPIT 13

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
			item->phys.width = item_list[i].phys.width;
			item->phys.length = item_list[i].phys.length;
			item->phys.height = item_list[i].phys.height;
            item->value = item_list[i].coin_value;
            item->coin_value = item_list[i].coin_value;
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
            item->armor_props.armor_y_offset = item_list[i].armor_props.armor_y_offset;
            item->phys.x_offset = item_list[i].phys.x_offset;
            item->phys.y_offset = item_list[i].phys.y_offset;

            return TRUE;
        }
    }

    // set item to "nothing"
    item->name = "";
    item->description = "";
    item->value = 0;
    item->phys.width = 0;
    item->phys.length = 0;
    item->phys.height = 0;
    item->value = 0;
    item->coin_value = 0;
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
    item->armor_props.armor_y_offset = 0; 
    item->phys.x_offset = 0;
    item->phys.y_offset = 0;

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
    items[num_items].phys.width = item.phys.width;
    items[num_items].phys.length = item.phys.length;
    items[num_items].phys.height = item.phys.height;
    items[num_items].mount_float_angle = 0.0f;
    items[num_items].mounted = FALSE;
    items[num_items].friction = AIR_RESISTANCE;
    items[num_items].tileset_name = item.tileset_name;
    items[num_items].tile_index = item.tile_index;
    items[num_items].highlighted = FALSE;
    items[num_items].consummable = item.consummable;
    items[num_items].type = item.type;
    items[num_items].value = item.value;
    items[num_items].coin_value = item.coin_value;
    items[num_items].weapon_props.attack_speed = item.weapon_props.attack_speed;
    items[num_items].weapon_props.attack_range = item.weapon_props.attack_range;
    items[num_items].weapon_props.min_damage = item.weapon_props.min_damage;
    items[num_items].weapon_props.max_damage = item.weapon_props.max_damage;
    items[num_items].weapon_props.weapon_type = item.weapon_props.weapon_type;
    items[num_items].armor_props.defence = item.armor_props.defence;
    items[num_items].armor_props.armor_type = item.armor_props.armor_type;
    items[num_items].armor_props.armor_y_offset = item.armor_props.armor_y_offset;
    items[num_items].phys.x_offset = item.phys.x_offset;
    items[num_items].phys.y_offset = item.phys.y_offset;

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
        if(items[i].mounted)
        {
            items[i].phys.z = 5 + 2.1*sin(items[i].mount_float_angle);
            items[i].mount_float_angle += PI/60.0f;
			if (items[i].mount_float_angle >= 2*PI)
            {
				items[i].mount_float_angle = 0.0f;
            }
        }
        else if(i == player.item_held_index)
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

            items[i].board_index = current_board_index;
        }
        else
        {
            items[i].phys.x += items[i].phys.x_vel;
            items[i].phys.y += items[i].phys.y_vel;
            items[i].phys.z += items[i].phys.z_vel;

            handle_terrain_collision(items[i].board_index,&items[i].phys);

            // check if it collides with a vendor
            for(int j = 0; j < num_npcs; ++j)
            {
                int index = npc_creature_indices[j];
                if(creatures[index].npc_props.is_vendor)
                {
                    double distance = get_distance(items[i].phys.x + TILE_WIDTH / 2, items[i].phys.y + TILE_HEIGHT / 2, creatures[index].phys.x + TILE_WIDTH / 2, creatures[index].phys.y + TILE_HEIGHT / 2);

                    if(distance <= 20)
                    {
                        if(are_entities_colliding(&items[i].phys, &creatures[index].phys))
                        {

                            spawn_floating_string(items[i].phys.x, items[i].phys.y, "*sold*", 14);

                            int coins_to_spawn = items[i].coin_value;

                            int gold_coins = coins_to_spawn / 100; coins_to_spawn -= (gold_coins*100);
                            int silver_coins = coins_to_spawn / 10; coins_to_spawn -= (silver_coins*10);
                            int bronze_coins = coins_to_spawn / 1; coins_to_spawn -= bronze_coins;

                            for(int c = 0; c < bronze_coins; ++c)
                                spawn_coin(creatures[index].phys.x + (rand() % TILE_WIDTH), creatures[index].phys.y + TILE_HEIGHT, 2.0f, 0.0f, +2.0f, 3.0f, COIN_BRONZE, creatures[index].board_index);
                            for(int c = 0; c < silver_coins; ++c)
                                spawn_coin(creatures[index].phys.x + (rand() % TILE_WIDTH), creatures[index].phys.y + TILE_HEIGHT, 2.0f, 0.0f, +1.0f, 3.0f, COIN_SILVER, creatures[index].board_index);
                            for(int c = 0; c < gold_coins; ++c)
                                spawn_coin(creatures[index].phys.x + (rand() % TILE_WIDTH), creatures[index].phys.y + TILE_HEIGHT, 2.0f, 0.0f, +2.0f, 3.0f, COIN_GOLD, creatures[index].board_index);
                            remove_item(i);
                        }
                    }
                }
            }

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
                
                // check board boundaries
                if(items[i].phys.x < 0)
                {
                    int board_index = get_name_of_board_location(board_list[current_board_index].map_x_index -1, board_list[current_board_index].map_y_index,items[i].board_name);

                    if(board_index < 0)
                        items[i].phys.x = 0;    
                    else
                    {
                        // go to board
                        items[i].phys.x = (BOARD_TILE_HEIGHT - 1)*TILE_HEIGHT;
                        items[i].board_index = board_index;
                    }
                }
                else if(items[i].phys.y < 0)
                {
                    int board_index = get_name_of_board_location(board_list[current_board_index].map_x_index, board_list[current_board_index].map_y_index - 1,items[i].board_name);

                    if(board_index < 0)
                        items[i].phys.y = 0;
                    else
                    {
                        // go to board
                        items[i].phys.y = (BOARD_TILE_HEIGHT-1)*TILE_HEIGHT;
                        items[i].board_index = board_index;
                    }
                }
                else if(items[i].phys.x > TILE_WIDTH*(BOARD_TILE_WIDTH - 1))
                {
                    int board_index = get_name_of_board_location(board_list[current_board_index].map_x_index + 1, board_list[current_board_index].map_y_index,items[i].board_name);

                    if(board_index < 0)
                        items[i].phys.x = TILE_WIDTH*(BOARD_TILE_WIDTH - 1); 
                    else
                    {
                        // go to board
                        items[i].phys.x = 0;
                        items[i].board_index = board_index;
                    }
                }
                else if(items[i].phys.y > TILE_HEIGHT*(BOARD_TILE_HEIGHT - 1))
                {
                    int board_index = get_name_of_board_location(board_list[current_board_index].map_x_index, board_list[current_board_index].map_y_index + 1,items[i].board_name);

                    if(board_index < 0)
                        items[i].phys.y = TILE_HEIGHT*(BOARD_TILE_HEIGHT - 1);
                    else
                    {
                        // go to board
                        items[i].phys.y = 0;
                        items[i].board_index = board_index;
                    }
                }
            }
        }

    }
}

static void draw_item(int i)
{
    if(items[i].board_index != current_board_index)
        return;

    draw_tile_shadow(items[i].phys.x - camera.x, items[i].phys.y - camera.y, items[i].tileset_name,items[i].tile_index,max(0,10 - day_cycle_shade_amount)); // shadow

    if(items[i].highlighted)
        draw_tile(items[i].phys.x - camera.x, items[i].phys.y - camera.y - items[i].phys.z*0.5f,items[i].tileset_name, items[i].tile_index,max(0,day_cycle_shade_amount-3));
    else
        draw_tile(items[i].phys.x - camera.x, items[i].phys.y - camera.y - items[i].phys.z*0.5f,items[i].tileset_name, items[i].tile_index,day_cycle_shade_amount);
}
