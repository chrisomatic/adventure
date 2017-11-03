KeyPress keypress = 0x0000;
KeyPress keypress_attack = 0x0000;

int foes_killed = 0; // @TEMP
long next_level = 10;

static void init_player()
{
    player.name = "Hero";
    player.tile_index = 0;
    player.lvl = 1;
    player.xp  = 0;
    player.hp  = 6;
    player.max_hp = 6;
    player.gold = 10;
    player.x   = (WORLD_TILE_WIDTH*(TILE_WIDTH-1))/2;
    player.y   = (WORLD_TILE_HEIGHT*(TILE_HEIGHT-1))/2;
    player.z   = 0;
    player.x_vel = 0;
    player.y_vel = 0;
    player.z_vel = 0;
    player.speed = 1.0f;
    player.base_speed = 1.0f;
    player.dir = DIR_DOWN;
    player.state = PLAYER_STATE_NONE;
    player.throw_coins = FALSE;
    player.pickup = FALSE;
    player.take = FALSE;
    player.jump = FALSE;
    player.notch = FALSE;
    player.shoot = FALSE;
    player.notch_index = -1;
    player.environmental_hurt_counter = 60;
    player.environmental_hurt_max = 60;
    player.item_held_index = -1;
    player.coin_throw_counter = 0;
    player.coin_throw_max = 20;
    player.anim.counter = 0;
    player.anim.max_count = 10;
    player.anim.frame = 0;
    player.anim.num_frames = 4;
    player.anim.frame_order[0] = 0;
    player.anim.frame_order[1] = 1;
    player.anim.frame_order[2] = 0;
    player.anim.frame_order[3] = 2;
    player.attack_angle = 0.0f;
    player.attack_frame_counter = 0;
    get_weapon_by_name("Sword",&player.weapon);
}
static void gain_level()
{
    player.lvl++;
    player.xp -= next_level;
    next_level *= 2.00f;

    spawn_floating_string(player.x + TILE_WIDTH/2, player.y,"+Lvl",8);
    for(int i = 0; i < 10; ++i)
        spawn_particle(rand() % TILE_WIDTH + player.x,player.y,2,5,'*',8);
}

static void update_player()
{
    // check if died
    if(player.hp <= 0)
    {
        // drop all player gold
        int gold_coins = player.gold / 100; player.gold -= (gold_coins*100);
        int silver_coins = player.gold / 10; player.gold -= (silver_coins*10);
        int bronze_coins = player.gold / 1; player.gold -= bronze_coins;

        for(int c = 0; c < bronze_coins; ++c)
            spawn_coin(player.x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),player.y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)),2,0,0,3.0f, COIN_BRONZE);
        for(int c = 0; c < silver_coins; ++c)
            spawn_coin(player.x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),player.y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)),2,0,0,2.5f, COIN_SILVER);
        for(int c = 0; c < gold_coins; ++c)
            spawn_coin(player.x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),player.y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)),2,0,0,2.0f, COIN_GOLD);

        player.state = PLAYER_STATE_DEAD;
    }

    if(player.state == PLAYER_STATE_DEAD)
    {
        // TODO
        return;
    }
    
    // update player movement
	if ((keypress & KEYPRESS_LEFT) == KEYPRESS_LEFT)
	{
        player.state |= PLAYER_STATE_MOVE;
		player.dir = DIR_LEFT;
		player.x_vel = -1.0f;
	}
	else if ((keypress & KEYPRESS_RIGHT) == KEYPRESS_RIGHT)
	{
        player.state |= PLAYER_STATE_MOVE;
		player.dir = DIR_RIGHT;
		player.x_vel = +1.0f;
	}
	else
		player.x_vel = 0.0f;

	if ((keypress & KEYPRESS_UP) == KEYPRESS_UP)
	{
        player.state |= PLAYER_STATE_MOVE;
		player.dir = DIR_UP;
		player.y_vel = -1.0f;
	}
	else if ((keypress & KEYPRESS_DOWN) == KEYPRESS_DOWN)
	{
        player.state |= PLAYER_STATE_MOVE;
		player.dir = DIR_DOWN;
		player.y_vel = +1.0f;
	}
	else
		player.y_vel = 0.0f;

    player.x += player.x_vel*player.speed;
    player.y += player.y_vel*player.speed;
    player.z += player.z_vel;

    // update z vel
    if((player.state & PLAYER_STATE_MIDAIR) == PLAYER_STATE_MIDAIR)
    {
        if(player.z <= 0.0f)
        {
            player.z = 0.0f;
            player.z_vel = 0.0f;
            player.state ^= PLAYER_STATE_MIDAIR;
        }
        else
        {
            player.z_vel -= GRAVITY;
        }
    }
    
    // handle player collision
    // terrain collision
    //
    // 1            2
    //  x----------x
    //  |          |
    //  |          |
    //  x----------x
    // 3            4
    //

    int player_check_x1 = (player.x + TILE_WIDTH/4) / TILE_WIDTH;
    int player_check_y1 = (player.y + TILE_HEIGHT/2) / TILE_HEIGHT;

    int player_check_x2 = (player.x + 3*TILE_WIDTH/4) / TILE_WIDTH;
    int player_check_y2 = (player.y + TILE_HEIGHT/2) / TILE_HEIGHT;

    int player_check_x3 = (player.x + TILE_WIDTH/4) / TILE_WIDTH;
    int player_check_y3 = (player.y + TILE_HEIGHT) / TILE_HEIGHT;

    int player_check_x4 = (player.x + 3*TILE_WIDTH/4) / TILE_WIDTH;
    int player_check_y4 = (player.y + TILE_HEIGHT) / TILE_HEIGHT;

    int collision_value_1 = world_collision[player_check_x1][player_check_y1];
    int collision_value_2 = world_collision[player_check_x2][player_check_y2];
    int collision_value_3 = world_collision[player_check_x3][player_check_y3];
    int collision_value_4 = world_collision[player_check_x4][player_check_y4];


    if(collision_value_1 == 5 || collision_value_2 == 5 || collision_value_3 == 5 || collision_value_4 == 5)
    {
        BOOL correct_x = FALSE;
        BOOL correct_y = FALSE;

        if((collision_value_1 == 5 && collision_value_3 == 5) || (collision_value_2 == 5 && collision_value_4 == 5))
        {
            //correct collision x
            player.x -= player.x_vel*player.speed;
            correct_x = TRUE;
        }
        if((collision_value_1 == 5 && collision_value_2 == 5) || (collision_value_3 == 5 && collision_value_4 == 5))
        {
            //correct collision y
            player.y -= player.y_vel*player.speed;
            correct_y = TRUE;
        }
        
        if(!correct_x && !correct_y)
        {
            if(collision_value_1 == 5 || collision_value_3 == 5)
				player.x += 1.0f*player.speed;
            
            if(collision_value_2 == 5 || collision_value_4 == 5)
				player.x -= 1.0f*player.speed;
        }
    }
    else if(collision_value_1 == 3 || collision_value_2 == 3 || collision_value_3 == 3 || collision_value_4 == 3)
    {
        if((player.state & PLAYER_STATE_MIDAIR) != PLAYER_STATE_MIDAIR)
        {
            // handle player in mud
            if(player.x_vel != 0 || player.y_vel != 0)
            {
                spawn_particle(rand() % TILE_WIDTH + player.x,player.y+TILE_HEIGHT,2,1,0,4);
            }

            player.speed = player.base_speed/2.0f;
        }
    }
    
    else if(collision_value_1 == 4 || collision_value_2 == 4 || collision_value_3 == 4 || collision_value_4 == 4)
    {
        // handle player in water
        if((player.state & PLAYER_STATE_MIDAIR) != PLAYER_STATE_MIDAIR)
        {
            if(player.x_vel != 0 || player.y_vel != 0)
            {
                spawn_particle(rand() % TILE_WIDTH + player.x,player.y+TILE_HEIGHT,2,2,0,8);
            }

            player.speed = player.base_speed/3.0f;
        }
    }
    else if(collision_value_1 == 6 || collision_value_2 == 6 || collision_value_3 == 6 || collision_value_4 == 6)
    {
        if((player.state & PLAYER_STATE_MIDAIR) != PLAYER_STATE_MIDAIR)
        {
            // handle player in lava
            if(player.x_vel != 0 || player.y_vel != 0)
            {
                spawn_particle(rand() % TILE_WIDTH + player.x,player.y+TILE_HEIGHT,2,2,0,6);
            }

            player.environmental_hurt_counter++;
            if(player.environmental_hurt_counter >= player.environmental_hurt_max)
            {
                player.environmental_hurt_counter = 0;
                player.hp--;
                spawn_floating_number(player.x+TILE_WIDTH/2,player.y,1,6);
            }
            player.speed = player.base_speed/3.0f;
        }
    }
    else if((player.state & PLAYER_STATE_MIDAIR) != PLAYER_STATE_MIDAIR)
    {
        player.speed = player.base_speed;
    }

    // keep player in world
    if(player.x < 0) player.x = 0;
    if(player.y < 0) player.y = 0;
    if(player.x >TILE_WIDTH*(WORLD_TILE_WIDTH - 1)) player.x = TILE_WIDTH*(WORLD_TILE_WIDTH - 1);
    if(player.y >TILE_HEIGHT*(WORLD_TILE_HEIGHT - 1)) player.y = TILE_HEIGHT*(WORLD_TILE_HEIGHT - 1);

    // update player movement animation
    if(player.x_vel != 0 || player.y_vel != 0)
    {
        // make player bounce
        if(player.z == 0.0f && player.base_speed > 1.5f)
        {
            player.state |= PLAYER_STATE_MIDAIR;
            player.z_vel = 1.0f;

        }

        player.anim.counter++;

        if(player.anim.counter >= 10/player.speed)
        {
            // cycle_animation
            player.anim.counter = 0;
            player.anim.frame += 1;
            if(player.anim.frame >= player.anim.num_frames)
                player.anim.frame = 0;
        }
    }
    else
    {
        // clear animation frame
        player.state ^= PLAYER_STATE_MOVE;
        player.anim.counter = 0;
        player.anim.frame = 0;
    }

    // update player melee attacking
    if((player.state & PLAYER_STATE_ATTACK) == PLAYER_STATE_ATTACK)
    {
        // switch player direction to attacking direction
        switch(player.attack_dir)
        {
            case PLAYER_ATTACK_LEFT: player.dir = DIR_LEFT; break;
            case PLAYER_ATTACK_RIGHT: player.dir = DIR_RIGHT; break;
            case PLAYER_ATTACK_UP: player.dir = DIR_UP; break;
            case PLAYER_ATTACK_DOWN: player.dir = DIR_DOWN; break; 
        }

        if(player.weapon.type == WEAPON_TYPE_MELEE)
        {
            player.attack_angle += player.weapon.attack_speed*(PI/30.0f);
            player.attack_frame_counter++;
            
            // check for collision with enemies/objects
            float cosa = cos(player.attack_angle);
            float sina = sin(player.attack_angle);

            for(int i = num_enemies-1; i >= 0;--i)
            {
                if(enemies[i].untargetable)
                {
                    continue;
                }

                int relative_enemy_position_x = enemies[i].x - camera.x;
                int relative_enemy_position_y = enemies[i].y - camera.y;

                // only care about enemies on screen...
                if(relative_enemy_position_x > 0 && relative_enemy_position_x < buffer_width)
                {
                    if(relative_enemy_position_y > 0 && relative_enemy_position_y < buffer_height)
                    {
                        // check along weapon line
                        for(int j = 0; j < player.weapon.attack_range; ++j)
                        {
                            // only care about hitting an enemy if it hasn't been hit yet.
                            if(enemies[i].state != ENEMY_STATE_STUNNED)
                            {
                                int start_weapon_x = player.x - camera.x + TILE_WIDTH/2;
                                int start_weapon_y = player.y - camera.y + TILE_HEIGHT/2;
                            
                                float delta_x = cosa*j;
                                float delta_y = -sina*j;

                                if(start_weapon_x+delta_x >= relative_enemy_position_x && start_weapon_x+delta_x <= relative_enemy_position_x+0.75*TILE_WIDTH)
                                {
                                    if(start_weapon_y+delta_y >= relative_enemy_position_y && start_weapon_y+delta_y <= relative_enemy_position_y+0.75*TILE_HEIGHT)
                                    {
                                        int damage = (rand() % (player.weapon.max_damage - player.weapon.min_damage + 1)) + player.weapon.min_damage;
                                        
                                        // add floating number
                                        spawn_floating_number(start_weapon_x+delta_x+camera.x,start_weapon_y+delta_y+camera.y,damage,6);

                                        
                                        // enemy hurt!
                                        enemies[i].hp -= damage;

                                        // check if enemy died
                                        if (enemies[i].hp <= 0)
                                        {
                                            foes_killed++;
                                            
                                            // player get xp
                                            player.xp += enemies[i].xp;
                                            if(player.xp >= next_level)
                                            {
                                                gain_level();
                                            }

                                            enemy_death(i);

                                        }
                                        else
                                        {
                                            enemies[i].state = ENEMY_STATE_STUNNED;
                                        }
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        if(player.attack_frame_counter >= 15.0f/player.weapon.attack_speed)
        {
            // end attacking
            player.attack_frame_counter = 0;
            player.state ^= PLAYER_STATE_ATTACK;
        }
    }
	else if ((player.state & PLAYER_STATE_NOTCHED) == PLAYER_STATE_NOTCHED)
	{
        // switch player direction to attacking direction
        switch(player.attack_dir)
        {
            case PLAYER_ATTACK_LEFT: player.dir = DIR_LEFT; break;
            case PLAYER_ATTACK_RIGHT: player.dir = DIR_RIGHT; break;
            case PLAYER_ATTACK_UP: player.dir = DIR_UP; break;
            case PLAYER_ATTACK_DOWN: player.dir = DIR_DOWN; break; 
        }
        
        // check if player notched a projectile
        if (player.notch_index > -1)
        {
            // update notched projectile

            float offset_y = 0.0f;
            float offset_x = 0.0f;
            float offset_z = 5.0f;

            int offset_amount = 0;

            switch(player.weapon.tile_index)
            {
                case BOW: offset_amount = 10; break;
                case STAFF: offset_amount = 15; break;
            }

            switch(player.dir)
            {
                case DIR_UP:
                    offset_y = -offset_amount;
                    offset_x = +0;
                    break;
                case DIR_DOWN:
                    offset_y = +offset_amount;
                    offset_x = +0;
                    break;
                case DIR_LEFT:
                    offset_y = +0;
                    offset_x = -offset_amount;
                    break;
                case DIR_RIGHT:
                    offset_y = +0;
                    offset_x = +offset_amount;
                    break;
            }

            projectiles[player.notch_index].x = player.x + offset_x;
            projectiles[player.notch_index].y = player.y + offset_y;
            projectiles[player.notch_index].z = player.z + offset_z;
            projectiles[player.notch_index].z_vel = 0.0f;
            projectiles[player.notch_index].angle = player.attack_angle;
        }
        else
        {
            // notch a projectile
            switch(player.weapon.tile_index)
            {
                case BOW:
                    player.notch_index = spawn_projectile(player.x, player.y, 5, 0, 0, 0, ARROW, player.attack_angle, 1.0f);
                    break;
                case STAFF:
                    player.notch_index = spawn_projectile(player.x, player.y, 5, 0, 0, 0, FIREBALL, player.attack_angle, 1.0f);
                    break;
            }
        }
    
		if (player.shoot)
		{
			player.shoot = FALSE;
            player.state ^= PLAYER_STATE_NOTCHED;

			float shoot_x_vel = 0.0f;
			float shoot_y_vel = 0.0f;
			float shoot_z_vel = 3.0f;

			switch (player.attack_dir)
			{
			case PLAYER_ATTACK_LEFT:
				shoot_x_vel = -1.0f;
				shoot_y_vel = 0.0f;
				break;
			case PLAYER_ATTACK_RIGHT:
				shoot_x_vel = +1.0f;
				shoot_y_vel = 0.0f;
				break;
			case PLAYER_ATTACK_UP:
				shoot_x_vel = 0.0f;
				shoot_y_vel = -1.0f;
				break;
			case PLAYER_ATTACK_DOWN:
				shoot_x_vel = 0.0f;
				shoot_y_vel = +1.0f;
				break;
			}

			if (player.notch_index > -1)
			{
				projectiles[player.notch_index].x_vel += ((shoot_x_vel*player.weapon.attack_range) + player.x_vel*player.speed);
				projectiles[player.notch_index].y_vel += ((shoot_y_vel*player.weapon.attack_range) + player.y_vel*player.speed);
				projectiles[player.notch_index].z_vel += (shoot_z_vel + player.z_vel);

				player.notch_index = -1;
			}
		}
	}

    // check if player picked up coins
    for(int i = num_coins -1; i>= 0; --i)
    {
        int coin_x = coins[i].x + TILE_WIDTH/2 -2;
        int coin_y = coins[i].y + TILE_HEIGHT/2 -2;

        // only consider coins on screen
        if(coin_x - camera.x >= 0 && coin_x - camera.x <= buffer_width)
        {
            if(coin_y - camera.y >= 0 && coin_y - camera.y <= buffer_height)
            {
                // check for player collision
                if(coin_x >= player.x && coin_x <= player.x + TILE_WIDTH)
                {
                    if(coin_y >= player.y + TILE_HEIGHT/2 && coin_y <= player.y + TILE_HEIGHT)
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
                        player.gold += amount;
                        spawn_floating_string(coins[i].x,coins[i].y,"$",color);
						remove_coin(i);
                    }
                }
            }
        }
    }

    // check if player threw coins
    if(player.throw_coins)
    {
        if(player.gold > 0)
        {
            player.coin_throw_counter++;
            if(player.coin_throw_counter >= player.coin_throw_max)
            {
                player.coin_throw_counter = 0;

                float coin_x_vel;
                float coin_y_vel;
                float coin_x_offset = 0;
                float coin_y_offset = 0;

                switch(player.dir)
                {
                    case DIR_UP:
                        coin_x_vel = 0;
                        coin_y_vel = -2.0f;
                        break;
                    case DIR_DOWN:
                        coin_x_vel = 0;
                        coin_y_vel = +2.0f;
                        coin_y_offset = 10;
                        break;
                    case DIR_LEFT:
                        coin_x_vel = -2.0f;
                        coin_y_vel = 0;
                        coin_x_offset = -5;
                        coin_y_offset = 5;
                        break;
                    case DIR_RIGHT:
                        coin_x_vel = +2.0f;
                        coin_y_vel = 0;
                        coin_x_offset = 10;
                        coin_y_offset = 5;
                        break;
                }
                

                if(spawn_coin(player.x + coin_x_offset, player.y + coin_y_offset,player.z + 5,player.x_vel*player.speed + coin_x_vel,player.y_vel*player.speed+coin_y_vel,2.0f,COIN_BRONZE))
                    player.gold--;
            }
        }
    }

    // check if player is near items
    float closest_item_distance = 100.0f;
    int closest_item_index = -1;
    float distance = 0.0f;

    for(int i = 0; i < num_items; ++i)
    {
        distance = get_distance(player.x + TILE_WIDTH/2,player.y + TILE_HEIGHT,items[i].x + TILE_WIDTH/2,items[i].y + TILE_HEIGHT/2);

        if(distance <= 14)
        {
            if(distance < closest_item_distance)
            {
                closest_item_distance = distance;
                closest_item_index = i;
            }
        }
        
        items[i].highlighted = FALSE;
    }

    if(closest_item_index > -1)
    {
        items[closest_item_index].highlighted = TRUE;

        message.name = items[closest_item_index].name;
        message.message = items[closest_item_index].description;
        message.color = 6;
        message.active = TRUE;
    }
    
    // check if player picked up / dropped an item
    if(player.pickup)
    {
        player.pickup = FALSE;

        if(player.item_held_index == -1)
        {
            for(int i = 0; i < num_items; ++i)
            {
                if(items[i].highlighted == TRUE)
                {
                    player.item_held_index = i;
                    break;
                }
            }
        }
        else
        {

            float item_x_vel;
            float item_y_vel;
            float item_x_offset = 0;
            float item_y_offset = 0;

            switch(player.dir)
            {
                case DIR_UP:
                    item_x_vel = 0;
                    item_y_vel = -2.0f;
                    break;
                case DIR_DOWN:
                    item_x_vel = 0;
                    item_y_vel = +2.0f;
                    item_y_offset = +10;
                    break;
                case DIR_LEFT:
                    item_x_vel = -2.0f;
                    item_y_vel = 0;
                    item_x_offset = -5;
                    item_y_offset = 5;
                    break;
                case DIR_RIGHT:
                    item_x_vel = +2.0f;
                    item_y_vel = 0;
                    item_x_offset = +5;
                    item_y_offset = 5;
                    break;
            }

            items[player.item_held_index].x += item_x_offset;
            items[player.item_held_index].y += item_y_offset;
            items[player.item_held_index].friction = AIR_RESISTANCE;
            items[player.item_held_index].x_vel = player.x_vel*player.speed + item_x_vel;
            items[player.item_held_index].y_vel = player.y_vel*player.speed + item_y_vel;
            items[player.item_held_index].z_vel = 2.0f;

            player.item_held_index = -1;
        }
    }

    // check if player has "taken" an item (used)
    if(player.take)
    {
        player.take = FALSE;

        int item_index_taken = -1;

        if(player.item_held_index > -1)
        {
            item_index_taken = player.item_held_index;
            player.item_held_index = -1;
        }
        else
        {
            // find nearest highlighted item
            for(int i = 0; i < num_items; ++i)
            {
                if(items[i].highlighted == TRUE)
                {
                    item_index_taken = i;
                    break;
                }
            }

        }

        if(item_index_taken > -1)
        {
            if(items[item_index_taken].consummable)
            {
                switch(items[item_index_taken].type)
                {
                    case ITEM_TYPE_HEALTH:
                        player.hp += items[item_index_taken].value;
                        player.hp = min(player.max_hp,player.hp);

                        spawn_floating_number(player.x+TILE_WIDTH/2,player.y,items[item_index_taken].value,11);

                        for(int i = 0; i < 20; ++i)
                        {
                            spawn_particle(rand() % TILE_WIDTH + player.x,player.y,rand() % 4 + 1,3,0,6);
                        }
                        break;
                }
            }
            remove_item(item_index_taken);
        }
    }

    // player jump
    if(player.jump)
    {
        player.jump = FALSE;

        if(player.z < 4.0f)
        {
            player.state |= PLAYER_STATE_MIDAIR;
            player.z_vel = 3.0f;
        }
    }
}

static void draw_player()
{
    if(player.state == PLAYER_STATE_DEAD)
    {
        // draw tombstone
		draw_tile(player.x - camera.x, player.y - camera.y,66, day_cycle_shade_amount);
    }
    else
    {
        // draw player
        if (player.dir != DIR_UP)
        {
            draw_tile_shadow(player.x - camera.x,player.y - camera.y,player.tile_index + player.dir+player.anim.frame_order[player.anim.frame],max(10-day_cycle_shade_amount,0));
            draw_tile(player.x - camera.x,player.y - camera.y - 0.5*player.z,player.tile_index + player.dir+player.anim.frame_order[player.anim.frame],day_cycle_shade_amount);
        }

        if((player.state & PLAYER_STATE_ATTACK) == PLAYER_STATE_ATTACK || (player.state & PLAYER_STATE_NOTCHED) == PLAYER_STATE_NOTCHED)
        {
            // draw weapon
            draw_tile_rotated_shadow(player.x - camera.x + cos(player.attack_angle)*2*TILE_WIDTH/3,player.y - camera.y - sin(player.attack_angle) * 2*TILE_HEIGHT/3,player.weapon.tile_index,player.attack_angle,max(10-day_cycle_shade_amount,0));
            draw_tile_rotated(player.x - camera.x + cos(player.attack_angle)*2*TILE_WIDTH/3,player.y - camera.y - sin(player.attack_angle) * 2*TILE_HEIGHT/3 - 0.5*player.z,player.weapon.tile_index,player.attack_angle, day_cycle_shade_amount);
        }
        
        if (player.dir == DIR_UP)
        {
            draw_tile_shadow(player.x - camera.x,player.y - camera.y,player.tile_index + player.dir+player.anim.frame_order[player.anim.frame],max(10-day_cycle_shade_amount,0));
            draw_tile(player.x - camera.x,player.y - camera.y - 0.5*player.z,player.tile_index + player.dir+player.anim.frame_order[player.anim.frame],day_cycle_shade_amount);
        }
    }

}
