typedef enum
{
	KEYPRESS_NONE  = 0,
	KEYPRESS_UP    = 1,
	KEYPRESS_DOWN  = 2,
	KEYPRESS_LEFT  = 4,
	KEYPRESS_RIGHT = 8
} KeyPress;

typedef enum
{
    PLAYER_ATTACK_UP,
    PLAYER_ATTACK_DOWN,
    PLAYER_ATTACK_LEFT,
    PLAYER_ATTACK_RIGHT
} AttackDirection;

typedef enum
{
	PLAYER_STATE_NONE   = 0,
	PLAYER_STATE_MOVE   = 1,
	PLAYER_STATE_ATTACK = 2,
	PLAYER_STATE_HURT   = 4,
	PLAYER_STATE_DEAD   = 8
} PlayerState;

typedef struct
{
    char* name;
    int tile_index;
    int lvl;
    int xp;
    int hp;
    int max_hp;
    float x;
    float y;
    int x_vel;
    int y_vel;
    int gold;
    float speed;
    float attack_angle;
    int attack_frame_counter;
    BOOL attack_hit;
    Weapon weapon;
    Direction dir;
    AttackDirection attack_dir;
    Animation anim;
    PlayerState state;
} Player;
Player player;

KeyPress keypress = 0x0000;
int rats_killed = 0; // @TEMP

static void init_player()
{
    player.name = "Hero";
    player.tile_index = 0;
    player.lvl = 1;
    player.xp  = 0;
    player.hp  = 6;
    player.max_hp = 6;
    player.gold = 0;
    player.x   = (WORLD_TILE_WIDTH*(TILE_WIDTH-1))/2;
    player.y   = (WORLD_TILE_HEIGHT*(TILE_HEIGHT-1))/2;
    player.x_vel = 0;
    player.y_vel = 0;
    player.speed = 1.0f;
    player.dir = DIR_DOWN;
    player.state = PLAYER_STATE_NONE;
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
    player.attack_hit = FALSE;
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
        //if(!player.attack_hit)
        //{
        float cosa = cos(player.attack_angle);
        float sina = sin(player.attack_angle);

        for(int i = num_enemies-1; i >= 0;--i)
        {
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
                        if(enemies[i].state != ENEMY_STATE_HIT)
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

                                    player.attack_hit = TRUE;
                                    
                                    // check if enemy died
                                    if (enemies[i].hp <= 0)
                                    {
										rats_killed++;
										int coins_spawned = rand() % (enemies[i].gold_drop_max+1);

										for(int c = 0; c < coins_spawned; ++c)
											spawn_coin(enemies[i].x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),enemies[i].y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)));

                                        remove_enemy(i);
                                    }
                                    else
                                    {
                                        enemies[i].state = ENEMY_STATE_HIT;
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
            player.attack_hit = FALSE;
        }
    }

    // check if player got coins
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
                        player.gold++;
                        spawn_floating_number(coins[i].x,coins[i].y,1,10);
						remove_coin(i);
                    }
                }
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
            shade_pixel8(player.x - camera.x + j,shadow_y - camera.y,i);
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
