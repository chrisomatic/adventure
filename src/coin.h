#define MAX_COINS 10000

const char* coin_tileset_name = "items";

typedef struct
{
    float friction;
    char* board_name;
    int board_index;
    CoinType type;
    Animation anim;
    PhysicalProperties phys;
} Coin;

Coin coins[MAX_COINS];
int num_coins = 0;

static BOOL spawn_coin(float x, float y, float z, float x_vel, float y_vel, float z_vel,CoinType type, int board_index)
{
	coins[num_coins].phys.x = x;
    coins[num_coins].phys.y = y;
    coins[num_coins].phys.z = z;
    coins[num_coins].phys.x_vel = x_vel;
    coins[num_coins].phys.y_vel = y_vel;
    coins[num_coins].phys.z_vel = z_vel;
    coins[num_coins].phys.x_offset = 6;
    coins[num_coins].phys.y_offset = 6;
    coins[num_coins].phys.width = 4;
    coins[num_coins].phys.length = 4;
    coins[num_coins].phys.height = 4;
    coins[num_coins].phys.speed = 1.0f;
    coins[num_coins].phys.base_speed = 1.0f;
    coins[num_coins].board_index = board_index;
    coins[num_coins].friction = AIR_RESISTANCE;
    coins[num_coins].type = type;
    coins[num_coins].anim.counter = rand() % 5;
    coins[num_coins].anim.max_count = 5;
    coins[num_coins].anim.frame = 0;
    coins[num_coins].anim.num_frames = 4;
    coins[num_coins].anim.frame_order[0] = 0;
    coins[num_coins].anim.frame_order[1] = 1;
    coins[num_coins].anim.frame_order[2] = 2;
    coins[num_coins].anim.frame_order[3] = 3;

    if(is_colliding_with_solid_terrain(board_index, &coins[num_coins].phys))
    {
        switch (player.dir)
        {
			case DIR_UP: coins[num_coins].phys.y += 6; break;
			case DIR_DOWN: coins[num_coins].phys.y -= 6; break;
			case DIR_LEFT: coins[num_coins].phys.x += 6; break;
			case DIR_RIGHT: coins[num_coins].phys.x -= 6; break;
        }
    }

    num_coins++;
    if(num_coins > MAX_COINS -1)
    {
        num_coins = MAX_COINS - 1;
        return FALSE;
    }

    return TRUE;
}

static void remove_coin(int index)
{
	num_coins--;
	coins[index] = coins[num_coins];
}

static void update_coins()
{
    for(int i = num_coins -1; i >= 0; --i)
    {
        coins[i].phys.x += coins[i].phys.x_vel*coins[i].phys.speed;
        coins[i].phys.y += coins[i].phys.y_vel*coins[i].phys.speed;
        coins[i].phys.z += coins[i].phys.z_vel*coins[i].phys.speed;

        handle_terrain_collision(coins[i].board_index, &coins[i].phys);

        // check if it collides with a vendor
        for(int j = 0; j < num_npcs; ++j)
        {
            int index = npc_creature_indices[j];
            if(creatures[index].npc_props.is_vendor)
            {
                double distance = get_distance(coins[i].phys.x + TILE_WIDTH / 2, coins[i].phys.y + TILE_HEIGHT / 2, creatures[index].phys.x + TILE_WIDTH / 2, creatures[index].phys.y + TILE_HEIGHT / 2);

                if(distance <= 20)
                {
                    if(are_entities_colliding(&coins[i].phys, &creatures[index].phys))
                    {
                        char color = 0;
                        int  amount = 0;

                        switch(coins[i].type)
                        {
                            case COIN_BRONZE:
                                amount = 1;
                                color = 9;
                                break;
                            case COIN_SILVER:
                                amount = 10;
                                color = 10;
                                break;
                            case COIN_GOLD:
                                amount = 100;
                                color = 14;
                                break;
                        }
                        creatures[index].npc_props.vendor_credit += amount;
                        spawn_floating_string(coins[i].phys.x, coins[i].phys.y, "$", color,coins[i].board_index);
						spawn_floating_string(coins[i].phys.x, coins[i].phys.y, "*thanks*", 14, coins[i].board_index);
                        remove_coin(i);
                    }
                }
            }
        }

        if(coins[i].phys.z < 0)
        {
            // hit the ground
            coins[i].phys.z = 0.0f;
            coins[i].phys.z_vel = 0.0f;
			coins[i].friction = GROUND_FRICTION;
        }
		else
		{
			coins[i].phys.z_vel -= GRAVITY; // gravity acceleration
		}

        if(coins[i].phys.x_vel != 0 || coins[i].phys.y_vel != 0)
        {
            // friction
            if(coins[i].phys.x_vel < 0) coins[i].phys.x_vel += coins[i].friction;
            else coins[i].phys.x_vel -= coins[i].friction;
            if(coins[i].phys.y_vel < 0) coins[i].phys.y_vel += coins[i].friction;
            else coins[i].phys.y_vel -= coins[i].friction;

            if(abs(coins[i].phys.x_vel) <= 0.1f) coins[i].phys.x_vel = 0.0f;
            if(abs(coins[i].phys.y_vel) <= 0.1f) coins[i].phys.y_vel = 0.0f;

            if(coins[i].phys.x < 0)
            {
                int board_index = get_name_of_board_location(board_list[current_board_index].map_x_index -1, board_list[current_board_index].map_y_index,coins[i].board_name);

                if(board_index < 0)
                    coins[i].phys.x = 0;    
                else
                {
                    // go to board
                    coins[i].phys.x = (BOARD_TILE_HEIGHT - 1)*TILE_HEIGHT;
                    coins[i].board_index = board_index;
                }
            }
            else if(coins[i].phys.y < 0)
            {
                int board_index = get_name_of_board_location(board_list[current_board_index].map_x_index, board_list[current_board_index].map_y_index - 1,coins[i].board_name);

                if(board_index < 0)
                    coins[i].phys.y = 0;
                else
                {
                    // go to board
                    coins[i].phys.y = (BOARD_TILE_HEIGHT-1)*TILE_HEIGHT;
                    coins[i].board_index = board_index;
                }
            }
            else if(coins[i].phys.x > TILE_WIDTH*(BOARD_TILE_WIDTH - 1))
            {
                int board_index = get_name_of_board_location(board_list[current_board_index].map_x_index + 1, board_list[current_board_index].map_y_index,coins[i].board_name);

                if(board_index < 0)
                    coins[i].phys.x = TILE_WIDTH*(BOARD_TILE_WIDTH - 1); 
                else
                {
                    // go to board
                    coins[i].phys.x = 0;
                    coins[i].board_index = board_index;
                }
            }
            else if(coins[i].phys.y > TILE_HEIGHT*(BOARD_TILE_HEIGHT - 1))
            {
                int board_index = get_name_of_board_location(board_list[current_board_index].map_x_index, board_list[current_board_index].map_y_index + 1,coins[i].board_name);

                if(board_index < 0)
                    coins[i].phys.y = TILE_HEIGHT*(BOARD_TILE_HEIGHT - 1);
                else
                {
                    // go to board
                    coins[i].phys.y = 0;
                    coins[i].board_index = board_index;
                }
            }
        }

        // update coin animation
        coins[i].anim.counter++;

        if(coins[i].anim.counter >= coins[i].anim.max_count)
        {
            // cycle_animation
            coins[i].anim.counter = 0;
            coins[i].anim.frame += 1;
            if(coins[i].anim.frame >= coins[i].anim.num_frames)
                coins[i].anim.frame = 0;
        }
    }

}

static void draw_coin(int i)
{
    if(coins[i].board_index != current_board_index)
        return;

    draw_tile_shadow(coins[i].phys.x - camera.x, coins[i].phys.y - camera.y, coin_tileset_name,coins[i].type + coins[i].anim.frame_order[coins[i].anim.frame],max(0,10 - day_cycle_shade_amount)); // shadow
	draw_tile(coins[i].phys.x - camera.x, coins[i].phys.y - camera.y - coins[i].phys.z*0.5f, coin_tileset_name, coins[i].type + coins[i].anim.frame_order[coins[i].anim.frame], day_cycle_shade_amount);
}
