KeyPress keypress = 0x0000;
int foes_killed = 0; // @TEMP

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
    player.x_vel = 0;
    player.y_vel = 0;
    player.speed = 1.0f;
    player.base_speed = 1.0f;
    player.dir = DIR_DOWN;
    player.state = PLAYER_STATE_NONE;
    player.throw_coins = FALSE;
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
    player.weapon.name = weapons[1].name;
    player.weapon.attack_speed = weapons[1].attack_speed;
    player.weapon.attack_range = weapons[1].attack_range;
    player.weapon.min_damage = weapons[1].min_damage;
    player.weapon.max_damage = weapons[1].max_damage;
    player.weapon.tile_index = weapons[1].tile_index;
}

static void update_player()
{
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
        // handle player in mud
        if(player.x_vel != 0 || player.y_vel != 0)
        {
            spawn_particle(rand() % TILE_WIDTH + player.x,player.y+TILE_HEIGHT,2,1,0,4);
        }

        player.speed = player.base_speed/2.0f;
    }
    
    else if(collision_value_1 == 4 || collision_value_2 == 4 || collision_value_3 == 4 || collision_value_4 == 4)
    {
        // handle player in water
        if(player.x_vel != 0 || player.y_vel != 0)
        {
            spawn_particle(rand() % TILE_WIDTH + player.x,player.y+TILE_HEIGHT,2,2,0,8);
        }

        player.speed = player.base_speed/3.0f;
    }
    else
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

    // update player attacking
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
                                        
                                        // add blood particles
                                        for(int p = 0; p < 10; ++p)
                                            spawn_particle(start_weapon_x+delta_x+camera.x,start_weapon_y+delta_y+camera.y,rand() % 4 + 1,3,0,6);

										int coins_to_spawn = rand() % (enemies[i].gold_drop_max+1);

                                        int gold_coins = coins_to_spawn / 100; coins_to_spawn -= (gold_coins*100);
                                        int silver_coins = coins_to_spawn / 10; coins_to_spawn -= (silver_coins*10);
                                        int bronze_coins = coins_to_spawn / 1; coins_to_spawn -= bronze_coins;
										for(int c = 0; c < bronze_coins; ++c)
											spawn_coin(enemies[i].x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),enemies[i].y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)),2,0,0,3.0f, COIN_BRONZE);
										for(int c = 0; c < silver_coins; ++c)
											spawn_coin(enemies[i].x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),enemies[i].y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)),2,0,0,2.5f, COIN_SILVER);
										for(int c = 0; c < gold_coins; ++c)
											spawn_coin(enemies[i].x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),enemies[i].y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)),2,0,0,2.0f, COIN_GOLD);

                                        // drop item(s)
                                        spawn_item(ITEM_MEAT,enemies[i].x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),enemies[i].y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)));

                                        remove_enemy(i);
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

        if(player.attack_frame_counter >= 15.0f/player.weapon.attack_speed)
        {
            // end attacking
            player.attack_frame_counter = 0;
            player.state ^= PLAYER_STATE_ATTACK;
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
                        spawn_floating_number(coins[i].x,coins[i].y,amount,color);
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
                

                if(spawn_coin(player.x + coin_x_offset, player.y + coin_y_offset,5,player.x_vel*player.speed + coin_x_vel,player.y_vel*player.speed+coin_y_vel,2.0f,COIN_BRONZE))
                    player.gold--;
            }
        }
    }
}

static void draw_player()
{
    // draw player shadow
    int shadow_y = player.y + TILE_HEIGHT - 3;

    for(int i = 7; i > 0; --i)
    {
		for (int j = 1; j < 15; ++j)
        {
            shade_pixel8(player.x - camera.x + j,shadow_y - camera.y,max(0,i - day_cycle_shade_amount));
        }
        shadow_y++;
    }

    // draw player
	if (player.dir != DIR_UP)
		draw_tile(player.x - camera.x,player.y - camera.y,player.tile_index + player.dir+player.anim.frame_order[player.anim.frame],day_cycle_shade_amount);

    if((player.state & PLAYER_STATE_ATTACK) == PLAYER_STATE_ATTACK)
    {
        // draw weapon
        draw_tile_rotated(player.x - camera.x + cos(player.attack_angle)*2*TILE_WIDTH/3,player.y - camera.y - sin(player.attack_angle) * 2*TILE_HEIGHT/3,player.weapon.tile_index,player.attack_angle, day_cycle_shade_amount);
    }
    
	if (player.dir == DIR_UP)
		draw_tile(player.x - camera.x, player.y - camera.y, player.tile_index + player.dir + player.anim.frame_order[player.anim.frame],day_cycle_shade_amount);

}
