KeyPress keypress = 0x0000;
KeyPress keypress_attack = 0x0000;

static void init_player()
{
    player.name = "Hero";
    player.board_name = "test2";
    player.tileset_name = "characters";
    player.tile_index = 0;
    player.available_stat_points = 0;
    player.lvl = 1;
    player.xp  = 0;
    player.phys.hp  = 10;
    player.phys.max_hp = 10;
    player.mp  = 6;
    player.max_mp = 6;
    player.gold = 10;
    player.phys.x   = 80.0f*TILE_WIDTH;
    player.phys.y   = 64.0f*TILE_HEIGHT;
    player.phys.z   = 0;
    player.phys.x_offset = 2;
    player.phys.y_offset = 8;
    player.phys.x_vel = 0;
    player.phys.y_vel = 0;
    player.phys.z_vel = 0;
    player.phys.length = 8;
    player.phys.width  = 12;
    player.phys.height = 16;
    player.phys.speed = 1.0f;
    player.phys.base_speed = 1.0f;
    player.dir = DIR_DOWN;
    player.state = PLAYER_STATE_NONE;
    player.throw_coins = FALSE;
    player.pickup = FALSE;
    player.take = FALSE;
    player.jump = FALSE;
    player.notch = FALSE;
    player.shoot = FALSE;
    player.unequip = FALSE;
    player.notch_index = -1;
    player.hurt_counter = 0;
    player.hurt_counter_max = 10;
    player.phys.environmental_hurt_counter = 60;
    player.phys.environmental_hurt_max = 60;
    player.bounce_counter = 0;
    player.bounce_counter_max = 12;
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
    player.stats.strength = 1;
    player.stats.dexterity = 1;
    player.stats.vitality = 1;
    player.stats.energy = 1;

    get_item_by_name("Sword",&player.weapon);
}

static void gain_level()
{
    player.lvl++;
    player.xp -= next_level;
    next_level *= 2.00f;

    spawn_floating_string(player.phys.x + TILE_WIDTH/2, player.phys.y,"+Lvl",8,current_board_index);
    for(int i = 0; i < 10; ++i)
        spawn_particle(rand() % TILE_WIDTH + player.phys.x,player.phys.y,2,5,'*',8,current_board_index);
    player.available_stat_points += 5;
}

static void player_die()
{
    // drop all player gold
    int gold_coins = player.gold / 100; player.gold -= (gold_coins*100);
    int silver_coins = player.gold / 10; player.gold -= (silver_coins*10);
    int bronze_coins = player.gold / 1; player.gold -= bronze_coins;

    for(int c = 0; c < bronze_coins; ++c)
        spawn_coin(player.phys.x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),player.phys.y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)),2,0,0,3.0f, COIN_BRONZE,current_board_index);
    for(int c = 0; c < silver_coins; ++c)
        spawn_coin(player.phys.x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),player.phys.y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)),2,0,0,2.5f, COIN_SILVER, current_board_index);
    for(int c = 0; c < gold_coins; ++c)
        spawn_coin(player.phys.x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),player.phys.y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)),2,0,0,2.0f, COIN_GOLD, current_board_index);

    player.state |= PLAYER_STATE_DEAD;

    generate_palette_file("data\\palettes\\palette_dead.png");
    update_game_colors();

    display_board_title = TRUE;
    board_title_display_counter = 0;
}

static void update_player()
{
    if((player.state & PLAYER_STATE_DEAD) == PLAYER_STATE_DEAD)
    {
        // TODO
        //return;
    }
    else if(player.phys.hp <= 0)
    {
        player_die();
    }
    
    if((player.state & PLAYER_STATE_HURT) == PLAYER_STATE_HURT)
    {
        ++player.hurt_counter;
        if(player.hurt_counter == player.hurt_counter_max)
        {
            player.hurt_counter = 0;
            player.state ^= PLAYER_STATE_HURT;
        }
    }
    
    // update player movement
	if ((keypress & KEYPRESS_LEFT) == KEYPRESS_LEFT)
	{
        player.state |= PLAYER_STATE_MOVE;
		player.dir = DIR_LEFT;
		player.phys.x_vel = -1.0f;
	}
	else if ((keypress & KEYPRESS_RIGHT) == KEYPRESS_RIGHT)
	{
        player.state |= PLAYER_STATE_MOVE;
		player.dir = DIR_RIGHT;
		player.phys.x_vel = +1.0f;
	}
	else
		player.phys.x_vel = 0.0f;

	if ((keypress & KEYPRESS_UP) == KEYPRESS_UP)
	{
        player.state |= PLAYER_STATE_MOVE;
		player.dir = DIR_UP;
		player.phys.y_vel = -1.0f;
	}
	else if ((keypress & KEYPRESS_DOWN) == KEYPRESS_DOWN)
	{
        player.state |= PLAYER_STATE_MOVE;
		player.dir = DIR_DOWN;
		player.phys.y_vel = +1.0f;
	}
	else
		player.phys.y_vel = 0.0f;

    player.phys.x += player.phys.x_vel*player.phys.speed;
    player.phys.y += player.phys.y_vel*player.phys.speed;
    player.phys.z += player.phys.z_vel;

    // update z vel
    if((player.state & PLAYER_STATE_MIDAIR) == PLAYER_STATE_MIDAIR)
    {
        if(player.phys.z <= 0.0f)
        {
            player.phys.z = 0.0f;
            player.phys.z_vel = 0.0f;
            player.state ^= PLAYER_STATE_MIDAIR;
        }
        else
        {
            player.phys.z_vel -= GRAVITY;
        }
    }
    
    handle_terrain_collision(current_board_index,&player.phys);
    
    // check board boundaries
    if(player.phys.x < 0)
    {
        int board_index = get_name_of_board_location(board_list[current_board_index].map_x_index -1, board_list[current_board_index].map_y_index,player.board_name);

        if(board_index < 0)
            player.phys.x = 0;    
        else
        {
            // go to board
            player.phys.x = (BOARD_TILE_HEIGHT - 1)*TILE_HEIGHT;
            current_board_index = board_index;
            display_board_title = TRUE;
            board_title_display_counter = 0;
        }
    }
    else if(player.phys.y < 0)
    {
        int board_index = get_name_of_board_location(board_list[current_board_index].map_x_index, board_list[current_board_index].map_y_index - 1,player.board_name);

        if(board_index < 0)
            player.phys.y = 0;
        else
        {
            // go to board
            player.phys.y = (BOARD_TILE_HEIGHT-1)*TILE_HEIGHT;
            current_board_index = board_index;
            display_board_title = TRUE;
            board_title_display_counter = 0;
        }
    }
    else if(player.phys.x > TILE_WIDTH*(BOARD_TILE_WIDTH - 1))
    {
        int board_index = get_name_of_board_location(board_list[current_board_index].map_x_index + 1, board_list[current_board_index].map_y_index,player.board_name);

        if(board_index < 0)
            player.phys.x = TILE_WIDTH*(BOARD_TILE_WIDTH - 1); 
        else
        {
            // go to board
            player.phys.x = 0;
            current_board_index = board_index;
            display_board_title = TRUE;
            board_title_display_counter = 0;
        }
    }
    else if(player.phys.y > TILE_HEIGHT*(BOARD_TILE_HEIGHT - 1))
    {
        int board_index = get_name_of_board_location(board_list[current_board_index].map_x_index, board_list[current_board_index].map_y_index + 1,player.board_name);

        if(board_index < 0)
            player.phys.y = TILE_HEIGHT*(BOARD_TILE_HEIGHT - 1);
        else
        {
            // go to board
            player.phys.y = 0;
            current_board_index = board_index;
            display_board_title = TRUE;
            board_title_display_counter = 0;
        }
    }

    // check portals
    for(int i = 0; i < num_portal_links; ++i)
    {
        if(portal_links[i].a.board_index == current_board_index)
        {
            // check if player is colliding with portal
            if(player.phys.x + player.phys.x_offset >= portal_links[i].a.x && player.phys.x + player.phys.x_offset + player.phys.width <= portal_links[i].a.x + TILE_WIDTH)
            {
				if (player.phys.y + player.phys.y_offset >= portal_links[i].a.y && player.phys.y + player.phys.y_offset + player.phys.length <= portal_links[i].a.y + TILE_HEIGHT)
                {
                    current_board_index = portal_links[i].b.board_index;
                    player.phys.x = portal_links[i].b.x;
                    player.phys.y = portal_links[i].b.y;
                    display_board_title = TRUE;
                    board_title_display_counter = 0;

                    if(player.phys.x_vel > 0) player.phys.x += TILE_WIDTH;
                    else if(player.phys.x_vel < 0) player.phys.x -= TILE_WIDTH;

                    if(player.phys.y_vel > 0) player.phys.y += TILE_HEIGHT;
                    else if(player.phys.y_vel < 0) player.phys.y -= TILE_HEIGHT;
                }

            }
        }

        if(portal_links[i].b.board_index == current_board_index)
        {
            // check if player is colliding with portal
            if(player.phys.x + player.phys.x_offset >= portal_links[i].b.x && player.phys.x + player.phys.x_offset + player.phys.width <= portal_links[i].b.x + TILE_WIDTH)
            {
                if(player.phys.y + player.phys.y_offset >= portal_links[i].b.y && player.phys.y + player.phys.y_offset + player.phys.length <= portal_links[i].b.y + TILE_HEIGHT)
                {
                    current_board_index = portal_links[i].a.board_index;
                    player.phys.x = portal_links[i].a.x;
                    player.phys.y = portal_links[i].a.y;
                    display_board_title = TRUE;
                    board_title_display_counter = 0;

                    player.phys.x += player.phys.x_vel*player.phys.speed*16;
                    player.phys.y += player.phys.y_vel*player.phys.speed*16;
                }
            }
        }
    }
    
    // update player movement animation
    if(player.phys.x_vel != 0 || player.phys.y_vel != 0)
    {
        // make player bounce
        if(player.phys.z == 0.0f && player.phys.base_speed > 1.5f && player.bounce_counter >= player.bounce_counter_max)
        {
            player.state |= PLAYER_STATE_MIDAIR;
            player.phys.z_vel = 0.6f;
            player.bounce_counter = 0;
        }

        player.bounce_counter++;

        player.anim.counter++;

        if(player.anim.counter >= 10/player.phys.speed)
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

        if(player.weapon.weapon_props.weapon_type == WEAPON_TYPE_MELEE)
        {
            player.attack_angle += player.weapon.weapon_props.attack_speed*(PI/30.0f);
            player.attack_frame_counter++;
            
            // check for collision with creatures/objects
            float cosa = cos(player.attack_angle);
            float sina = sin(player.attack_angle);

            for(int i = num_creatures-1; i >= 0;--i)
            {
                if(creatures[i].untargetable)
                    continue;

                if(creatures[i].board_index != current_board_index)
                    continue;

                int relative_creature_position_x = creatures[i].phys.x - camera.x;
                int relative_creature_position_y = creatures[i].phys.y - camera.y;

                // only care about creatures on screen...
                if(relative_creature_position_x > 0 && relative_creature_position_x < buffer_width)
                {
                    if(relative_creature_position_y > 0 && relative_creature_position_y < buffer_height)
                    {
                        // check along weapon line
                        for(int j = 0; j < player.weapon.weapon_props.attack_range; ++j)
                        {
                            // only care about hitting an creature if it hasn't been hit yet.
                            if(!creatures[i].stunned)
                            {
                                int start_weapon_x = player.phys.x - camera.x + TILE_WIDTH/2;
                                int start_weapon_y = player.phys.y - camera.y + TILE_HEIGHT/2;
                            
                                float delta_x = cosa*j;
                                float delta_y = -sina*j;

                                if(start_weapon_x+delta_x >= relative_creature_position_x && start_weapon_x+delta_x <= relative_creature_position_x+0.75*TILE_WIDTH)
                                {
                                    if(start_weapon_y+delta_y >= relative_creature_position_y && start_weapon_y+delta_y <= relative_creature_position_y+0.75*TILE_HEIGHT)
                                    {
                                        int damage = (rand() % (player.weapon.weapon_props.max_damage - player.weapon.weapon_props.min_damage + 1)) + player.weapon.weapon_props.min_damage;
                                        
                                        // add floating number
                                        spawn_floating_number(start_weapon_x+delta_x+camera.x,start_weapon_y+delta_y+camera.y,damage,6,current_board_index);

                                        // creature hurt!
                                        creatures[i].phys.hp -= damage;

                                        // check if creature died
                                        if (creatures[i].phys.hp <= 0)
                                        {
                                            foes_killed++;
                                            
                                            // player get xp
                                            player.xp += creatures[i].xp;
                                            if(player.xp >= next_level)
                                            {
                                                gain_level();
                                            }

                                            creature_death(i);

                                        }
                                        else
                                        {
                                            creatures[i].stunned = TRUE;
                                            if(creatures[i].behavior == CREATURE_BEHAVIOR_PASSIVE)
                                            {
                                                creatures[i].behavior = CREATURE_BEHAVIOR_AGGRESSIVE;
                                                creatures[i].deaggress = TRUE;
                                            }
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

        if(player.attack_frame_counter >= 15.0f/player.weapon.weapon_props.attack_speed)
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
                case CROSSBOW: offset_amount = 10; break;
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

			projectiles[player.notch_index].phys.x = player.phys.x + offset_x;
			projectiles[player.notch_index].phys.y = player.phys.y + offset_y;
			projectiles[player.notch_index].phys.z = player.phys.z + offset_z;
			projectiles[player.notch_index].phys.z_vel = 0.0f;
            projectiles[player.notch_index].angle = player.attack_angle;
        }
        else
        {
            // notch a projectile
            switch(player.weapon.tile_index)
            {
                case BOW:
                case CROSSBOW:
                    player.notch_index = spawn_projectile(player.phys.x, player.phys.y, 5, 0, 0, 0, ARROW, player.attack_angle, player.weapon.weapon_props.min_damage, player.weapon.weapon_props.max_damage,TRUE);
                    break;
                case STAFF:
                    if(player.mp > 0)
                        player.notch_index = spawn_projectile(player.phys.x, player.phys.y, 5, 0, 0, 0, FIREBALL, player.attack_angle, player.weapon.weapon_props.min_damage, player.weapon.weapon_props.max_damage,TRUE);

                    --player.mp;
                    player.mp = max(0,player.mp);
                    break;
                case POISON_SPIT:
                    player.notch_index = spawn_projectile(player.phys.x, player.phys.y, 5, 0, 0, 0, POISON_SPIT, player.attack_angle, player.weapon.weapon_props.min_damage, player.weapon.weapon_props.max_damage,TRUE); 
                    break;
            }
        }
    
		if (player.shoot)
		{
            player.state ^= PLAYER_STATE_NOTCHED;
			player.shoot = FALSE;

			float shoot_x_vel = 0.0f;
			float shoot_y_vel = 0.0f;
			float shoot_z_vel = 2.0f;

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
                projectiles[player.notch_index].shot = TRUE;

				projectiles[player.notch_index].phys.x_vel += ((shoot_x_vel*player.weapon.weapon_props.attack_range) + player.phys.x_vel*player.phys.speed);
				projectiles[player.notch_index].phys.y_vel += ((shoot_y_vel*player.weapon.weapon_props.attack_range) + player.phys.y_vel*player.phys.speed);
				projectiles[player.notch_index].phys.z_vel += (shoot_z_vel + player.phys.z_vel);

				player.notch_index = -1;
			}
		}
	}

    // check if player picked up coins
    for(int i = num_coins -1; i>= 0; --i)
    {
        if(coins[i].board_index != current_board_index)
            continue;

		int coin_x = coins[i].phys.x + TILE_WIDTH / 2 - 2;
		int coin_y = coins[i].phys.y + TILE_HEIGHT / 2 - 2;

        // only consider coins on screen
        if(coin_x - camera.x >= 0 && coin_x - camera.x <= buffer_width)
        {
            if(coin_y - camera.y >= 0 && coin_y - camera.y <= buffer_height)
            {
                // check for player collision
                if(coin_x >= player.phys.x && coin_x <= player.phys.x + TILE_WIDTH)
                {
                    if(coin_y >= player.phys.y + TILE_HEIGHT/2 && coin_y <= player.phys.y + TILE_HEIGHT)
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
						spawn_floating_string(coins[i].phys.x, coins[i].phys.y, "$", color,current_board_index);
						remove_coin(i);
                        //PlaySound("data\\sfx\\pickup_coin.wav", NULL, SND_FILENAME | SND_ASYNC);
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
                

                if(spawn_coin(player.phys.x + coin_x_offset, player.phys.y + coin_y_offset,player.phys.z + 5,player.phys.x_vel*player.phys.speed + coin_x_vel,player.phys.y_vel*player.phys.speed+coin_y_vel,2.0f,COIN_BRONZE,current_board_index))
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
        if(items[i].board_index != current_board_index)
            continue;

		distance = get_distance(player.phys.x + TILE_WIDTH / 2, player.phys.y + TILE_HEIGHT, items[i].phys.x + TILE_WIDTH / 2, items[i].phys.y + TILE_HEIGHT / 2);

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
        message.value = items[closest_item_index].coin_value;
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
                    if(items[i].mounted)
                    {
                        if(items[i].vendor_index < 0)
                            continue;

                        if(creatures[items[i].vendor_index].npc_props.vendor_credit >= items[i].coin_value)
                        {
                            creatures[items[i].vendor_index].npc_props.vendor_credit -= items[i].coin_value;
                            items[i].mounted = FALSE;
                            items[i].vendor_index = -1;
                            spawn_floating_string(items[i].phys.x, items[i].phys.y, "*purchased*", 14,current_board_index);
                        }
                    }
                    else
                    {
                        player.item_held_index = i;
                    }
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

            items[player.item_held_index].phys.x += item_x_offset;
			items[player.item_held_index].phys.y += item_y_offset;
            items[player.item_held_index].friction = AIR_RESISTANCE;
			items[player.item_held_index].phys.x_vel = player.phys.x_vel*player.phys.speed + item_x_vel;
			items[player.item_held_index].phys.y_vel = player.phys.y_vel*player.phys.speed + item_y_vel;
			items[player.item_held_index].phys.z_vel = 2.0f;

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
            if(items[item_index_taken].mounted)
            {
                // @TODO
            }
            else
            {
                if(items[item_index_taken].consummable)
                {
                    switch(items[item_index_taken].type)
                    {
                        case ITEM_TYPE_HEALTH:
                            player.phys.hp += items[item_index_taken].value;
                            player.phys.hp = min(player.phys.max_hp,player.phys.hp);

                            spawn_floating_number(player.phys.x+TILE_WIDTH/2,player.phys.y,items[item_index_taken].value,11,current_board_index);

                            for(int i = 0; i < 20; ++i)
                            {
                                spawn_particle(rand() % TILE_WIDTH + player.phys.x,player.phys.y,rand() % 4 + 1,3,0,6,current_board_index);
                            }
                            break;
                        case ITEM_TYPE_MANA:
                            player.mp += items[item_index_taken].value;
                            player.mp = min(player.max_mp,player.mp);

                            spawn_floating_number(player.phys.x+TILE_WIDTH/2,player.phys.y,items[item_index_taken].value,8,current_board_index);

                            for(int i = 0; i < 20; ++i)
                            {
                                spawn_particle(rand() % TILE_WIDTH + player.phys.x,player.phys.y,rand() % 2 + 1,3,'*',8,current_board_index);
                            }
                            break;
                    }
                }
                if(items[item_index_taken].type == ITEM_TYPE_WEAPON)
                {
                    if(player.weapon.name != "")
                        spawn_item(player.weapon.name,player.phys.x,player.phys.y, current_board_index);

                    get_item_by_name(items[item_index_taken].name,&player.weapon);
                }
                else if(items[item_index_taken].type == ITEM_TYPE_ARMOR)
                {
                    switch(items[item_index_taken].armor_props.armor_type)
                    {
                        case ARMOR_TYPE_HEAD: 
                            if(player.armor_head.name != "")
                                spawn_item(player.armor_head.name,player.phys.x,player.phys.y, current_board_index);

                            get_item_by_name(items[item_index_taken].name,&player.armor_head);

                            break;
                        case ARMOR_TYPE_BODY:
                            if(player.armor_body.name != "")
                                spawn_item(player.armor_body.name,player.phys.x,player.phys.y, current_board_index);

                            get_item_by_name(items[item_index_taken].name,&player.armor_body);

                            break;
                        case ARMOR_TYPE_HANDS:
                            if(player.armor_hands.name != "")
                                spawn_item(player.armor_hands.name,player.phys.x,player.phys.y, current_board_index);

                            get_item_by_name(items[item_index_taken].name,&player.armor_hands);
                            
                            break;
                        case ARMOR_TYPE_FEET:
                            if(player.armor_feet.name != "")
                                spawn_item(player.armor_feet.name,player.phys.x,player.phys.y, current_board_index);

                            get_item_by_name(items[item_index_taken].name,&player.armor_feet);
                            
                            break;
                    }
                }

            remove_item(item_index_taken);

            }

        }
    }

    // player jump
    if(player.jump)
    {
        player.jump = FALSE;

        if(player.phys.z < 4.0f)
        {
            player.state |= PLAYER_STATE_MIDAIR;
            player.phys.z_vel = 3.0f;
        }
    }
    
}
static void draw_player_and_armor()
{
    // player
    draw_tile_shadow(player.phys.x - camera.x,player.phys.y - camera.y,player.tileset_name,player.tile_index + player.dir+player.anim.frame_order[player.anim.frame],max(10-day_cycle_shade_amount,0));
    if((player.state & PLAYER_STATE_HURT) == PLAYER_STATE_HURT)
    {
        draw_tile_tinted(player.phys.x - camera.x,player.phys.y - camera.y - 0.5*player.phys.z,player.tileset_name,player.tile_index + player.dir+player.anim.frame_order[player.anim.frame],6);

    }
    else
    {
        draw_tile(player.phys.x - camera.x,player.phys.y - camera.y - 0.5*player.phys.z,player.tileset_name,player.tile_index + player.dir+player.anim.frame_order[player.anim.frame],day_cycle_shade_amount);

    }

    // armor ...
    
    // helm
    if(player.armor_head.name != NULL)
        draw_tile(player.phys.x - camera.x, player.phys.y - camera.y + player.armor_head.armor_props.armor_y_offset - 0.5*player.phys.z, player.armor_head.tileset_name, player.armor_head.tile_index + player.dir + player.anim.frame_order[player.anim.frame], day_cycle_shade_amount);
    
    // body
    

    // hands
    
    // feet
}

static void draw_weapon()
{
    draw_tile_rotated_shadow(player.phys.x - camera.x + cos(player.attack_angle)*2*TILE_WIDTH/3,player.phys.y - camera.y - sin(player.attack_angle) * 2*TILE_HEIGHT/3,player.weapon.tileset_name,player.weapon.tile_index,player.attack_angle,max(10-day_cycle_shade_amount,0));
    draw_tile_rotated(player.phys.x - camera.x + cos(player.attack_angle)*2*TILE_WIDTH/3,player.phys.y - camera.y - sin(player.attack_angle) * 2*TILE_HEIGHT/3 - 0.5*player.phys.z,player.weapon.tileset_name,player.weapon.tile_index,player.attack_angle, day_cycle_shade_amount);
}

static void draw_player()
{
    if((player.state & PLAYER_STATE_DEAD) == PLAYER_STATE_DEAD)
    {
        // draw tombstone
		draw_tile(player.phys.x - camera.x, player.phys.y - camera.y,"objects",2,day_cycle_shade_amount);
    }
    else
    {
        // draw player
        if (player.dir == DIR_UP)
        {
            if((player.state & PLAYER_STATE_ATTACK) == PLAYER_STATE_ATTACK || (player.state & PLAYER_STATE_NOTCHED) == PLAYER_STATE_NOTCHED)
                draw_weapon();

            draw_player_and_armor();
        }
        else 
        {
            draw_player_and_armor();

            if((player.state & PLAYER_STATE_ATTACK) == PLAYER_STATE_ATTACK || (player.state & PLAYER_STATE_NOTCHED) == PLAYER_STATE_NOTCHED)
                draw_weapon();
        }
    }

}
