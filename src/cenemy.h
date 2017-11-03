#define MAX_ENEMIES    1000
#define MAX_ENEMY_LIST 100

typedef enum
{
    ENEMY_STATE_NEUTRAL,
    ENEMY_STATE_ACTING,
    ENEMY_STATE_STUNNED
} EnemyState;

typedef enum
{
    ENEMY_MODE_WANDER,
    ENEMY_MODE_PURSUE
} EnemyMode;

typedef enum
{
    PASSIVE,
    AGGRESSIVE
} EnemyBehavior;

typedef struct
{
    char* name;
    int tile_index;
    int xp;
    int hp;
    int max_hp;
    float x;
    float y;
    int x_vel;
    int y_vel;
    int stun_counter;
    int stun_duration;
    int gold_drop_max;
    int aggro_radius;
    float speed;
    float base_speed;
    int environmental_hurt_counter;
    int environmental_hurt_max;
    int action_counter;
    int action_counter_max;
    int action_duration_counter;
    int action_duration_counter_max;
    int particle_spawn_counter;
    BOOL untargetable;
    EnemyState state;
    EnemyMode  mode;
    EnemyBehavior behavior;
    Direction dir;
    Animation anim;
} Enemy;

Enemy enemies[MAX_ENEMIES];
Enemy enemy_list[MAX_ENEMY_LIST];

int num_enemies = 0;

static BOOL get_enemy_by_name(const char* name,Enemy* enemy)
{
    for(int i = 0; i < MAX_ENEMY_LIST; ++i)
    {
		if (enemy_list[i].name == NULL)
			continue;

        if(strcmp(enemy_list[i].name, name) == 0)
        {
            enemy->name = enemy_list[i].name;
			enemy->hp = enemy_list[i].hp;
			enemy->max_hp = enemy_list[i].max_hp;
			enemy->xp = enemy_list[i].xp;
            enemy->speed = enemy_list[i].speed;
            enemy->gold_drop_max = enemy_list[i].gold_drop_max;
            enemy->behavior = enemy_list[i].behavior;
            enemy->untargetable = enemy_list[i].untargetable;
            enemy->tile_index = enemy_list[i].tile_index;

            return TRUE;
        }
    }
}
static BOOL spawn_enemy(const char* enemy_name,float x, float y)
{
    Enemy enemy = {0};

    if(!get_enemy_by_name(enemy_name,&enemy))
        return;

    enemies[num_enemies].x = x; 
    enemies[num_enemies].y = y;
    enemies[num_enemies].x_vel = 0;
    enemies[num_enemies].y_vel = 0;
    enemies[num_enemies].speed = enemy.speed;
    enemies[num_enemies].base_speed = enemy.speed;
    enemies[num_enemies].dir = DIR_DOWN;
    enemies[num_enemies].state = ENEMY_STATE_NEUTRAL;
    enemies[num_enemies].mode  = ENEMY_MODE_WANDER;
    enemies[num_enemies].name = enemy_name;
    enemies[num_enemies].tile_index = enemy.tile_index;
    enemies[num_enemies].hp = enemy.hp;
    enemies[num_enemies].max_hp = enemy.max_hp;
    enemies[num_enemies].xp = enemy.xp;
    enemies[num_enemies].gold_drop_max = enemy.gold_drop_max;
    enemies[num_enemies].aggro_radius = 50;
    enemies[num_enemies].particle_spawn_counter = 0;
    enemies[num_enemies].environmental_hurt_counter = 60;
    enemies[num_enemies].environmental_hurt_max = 60;
    enemies[num_enemies].stun_counter = 0;
    enemies[num_enemies].stun_duration = 20;
    enemies[num_enemies].untargetable = enemy.untargetable;
    enemies[num_enemies].behavior = enemy.behavior;
    enemies[num_enemies].action_counter_max = 60;
    enemies[num_enemies].action_counter = rand() % enemies[num_enemies].action_counter_max;
    enemies[num_enemies].action_duration_counter = 0;
    enemies[num_enemies].anim.counter = 0;
    enemies[num_enemies].anim.max_count = 10;
    enemies[num_enemies].anim.frame = 0;
    enemies[num_enemies].anim.num_frames = 4;
    enemies[num_enemies].anim.frame_order[0] = 0;
    enemies[num_enemies].anim.frame_order[1] = 1;
    enemies[num_enemies].anim.frame_order[2] = 0;
    enemies[num_enemies].anim.frame_order[3] = 2;
    
    ++num_enemies;

    if(num_enemies > MAX_ENEMIES -1)
    {
        num_enemies = MAX_ENEMIES - 1;
        return FALSE;
    }

    return TRUE;
}

static void init_enemies()
{
    num_enemies = 0;

    for (int i = 0; i < MAX_ENEMIES ; ++i)
    {
        int enemy_x, enemy_y;
        int test_collision_1, test_collision_2, test_collision_3, test_collision_4;
        BOOL collision = TRUE;

        // make sure not to spawn enemy in a solid object
        while(collision)
        {
            enemy_x = rand() % (TILE_WIDTH*(WORLD_TILE_WIDTH -1));
            enemy_y = rand() % (TILE_HEIGHT*(WORLD_TILE_HEIGHT -1));

            test_collision_1 = world_collision[enemy_x/TILE_WIDTH][enemy_y/TILE_HEIGHT]; 
            test_collision_2 = world_collision[(enemy_x + TILE_WIDTH)/TILE_WIDTH][(enemy_y + TILE_HEIGHT)/TILE_HEIGHT];
            test_collision_3 = world_collision[enemy_x/TILE_WIDTH][(enemy_y+TILE_HEIGHT)/TILE_HEIGHT]; 
            test_collision_4 = world_collision[(enemy_x + TILE_WIDTH)/TILE_WIDTH][enemy_y/TILE_HEIGHT];
            
            collision = (test_collision_1 == 5 && test_collision_2 == 5 || test_collision_3 == 5 || test_collision_4 == 5);
        }

        int enemy_type = rand() % 3 + 1;

        switch(enemy_type)
        {
            case 1:
                spawn_enemy("Orc",enemy_x,enemy_y);
                break;
            case 2:
                spawn_enemy("Rat",enemy_x,enemy_y);
                break;
            case 3:
                spawn_enemy("White Rat",enemy_x,enemy_y);
                break;
        }
    }
}

static void remove_enemy(int index)
{
	num_enemies--;
	enemies[index] = enemies[num_enemies];
}

static void enemy_death(int i)
{
    for(int p = 0; p < 10; ++p)
        spawn_particle(rand()%TILE_WIDTH + enemies[i].x,rand()%TILE_HEIGHT+enemies[i].y,rand() % 4 + 1,3,0,6);

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
    int item_percent = rand() % 100 + 1;
    if(item_percent >= 1 && item_percent <= 40)
    {
        spawn_item("Meat",enemies[i].x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),enemies[i].y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)));

    }
    if(item_percent >= 41 && item_percent <= 50)
    {
        spawn_item("Iron Sword",enemies[i].x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),enemies[i].y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)));

    }

    remove_enemy(i);

}

static void update_enemies()
{
    for(int i = num_enemies -1; i >= 0;--i)
    {
        if(enemies[i].state == ENEMY_STATE_STUNNED)
        {
            enemies[i].stun_counter++;
            if(enemies[i].stun_counter >= enemies[i].stun_duration)
            {
                enemies[i].stun_counter = 0;
                enemies[i].state = ENEMY_STATE_NEUTRAL;
            }
        }
        else
        {
            // check distance from player
            if(get_distance(player.x + TILE_WIDTH/2,player.y + TILE_HEIGHT/2,enemies[i].x + TILE_WIDTH/2,enemies[i].y+TILE_HEIGHT/2) <= enemies[i].aggro_radius)
            {
                if(enemies[i].mode == ENEMY_MODE_WANDER)
                {
                    enemies[i].state = ENEMY_STATE_NEUTRAL;
                    enemies[i].action_counter = enemies[i].action_counter_max;
                }

                enemies[i].mode = ENEMY_MODE_PURSUE;
            }
            else
            {
                enemies[i].mode = ENEMY_MODE_WANDER;
            }

            // if enemy is performing an action, let it play out
            if(enemies[i].state == ENEMY_STATE_ACTING)
            {

                enemies[i].action_duration_counter++;

                if(enemies[i].action_duration_counter >= enemies[i].action_duration_counter_max)
                {
                        enemies[i].action_duration_counter = 0;
                        enemies[i].state = ENEMY_STATE_NEUTRAL; // return to neutral state
                        
                        if(enemies[i].mode != ENEMY_MODE_PURSUE)
                        {
                            enemies[i].x_vel = +0;
                            enemies[i].y_vel = +0;
                        }
                }
                
                // update enemy position
                enemies[i].x += enemies[i].x_vel*enemies[i].speed;
                enemies[i].y += enemies[i].y_vel*enemies[i].speed;

                if(enemies[i].x_vel != 0 || enemies[i].y_vel != 0)
                {
                    // resolve collisions
                    //
                    // 1            2
                    //  x----------x
                    //  |          |
                    //  |          |
                    //  x----------x
                    // 3            4
                    //

                    // handle terrain collisions
                    int enemy_check_x1 = (enemies[i].x + TILE_WIDTH/4) / TILE_WIDTH;
                    int enemy_check_y1 = (enemies[i].y + TILE_HEIGHT/2) / TILE_HEIGHT;

                    int enemy_check_x2 = (enemies[i].x + 3*TILE_WIDTH/4) / TILE_WIDTH;
                    int enemy_check_y2 = (enemies[i].y + TILE_HEIGHT/2) / TILE_HEIGHT;

                    int enemy_check_x3 = (enemies[i].x + TILE_WIDTH/4) / TILE_WIDTH;
                    int enemy_check_y3 = (enemies[i].y + TILE_HEIGHT) / TILE_HEIGHT;

                    int enemy_check_x4 = (enemies[i].x + 3*TILE_WIDTH/4) / TILE_WIDTH;
                    int enemy_check_y4 = (enemies[i].y + TILE_HEIGHT) / TILE_HEIGHT;

                    int collision_value_1 = world_collision[enemy_check_x1][enemy_check_y1];
                    int collision_value_2 = world_collision[enemy_check_x2][enemy_check_y2];
                    int collision_value_3 = world_collision[enemy_check_x3][enemy_check_y3];
                    int collision_value_4 = world_collision[enemy_check_x4][enemy_check_y4];

                    if(collision_value_1 == 5 || collision_value_2 == 5 || collision_value_3 == 5 || collision_value_4 == 5)
                    {
                        BOOL correct_x = FALSE;
                        BOOL correct_y = FALSE;

                        if((collision_value_1 == 5 && collision_value_3 == 5) || (collision_value_2 == 5 && collision_value_4 == 5))
                        {
                            //correct collision x
                            enemies[i].x -= enemies[i].x_vel*enemies[i].speed;
                            correct_x = TRUE;
                        }
                        if((collision_value_1 == 5 && collision_value_2 == 5) || (collision_value_3 == 5 && collision_value_4 == 5))
                        {
                            //correct collision y
                            enemies[i].y -= enemies[i].y_vel*enemies[i].speed;
                            correct_y = TRUE;
                        }
                        
                        if(!correct_x && !correct_y)
                        {
                            if(collision_value_1 == 5 || collision_value_3 == 5)
                                enemies[i].x += 1.0f*enemies[i].speed;
                            
                            if(collision_value_2 == 5 || collision_value_4 == 5)
                                enemies[i].x -= 1.0f*enemies[i].speed;
                        }
                    }
                    else if(collision_value_1 == 3 || collision_value_2 == 3 || collision_value_3 == 3 || collision_value_4 == 3)
                    {
                        // handle enemy in mud
                        if(enemies[i].x_vel != 0 || enemies[i].y_vel != 0)
                        {
                            spawn_particle(rand() % TILE_WIDTH + enemies[i].x,enemies[i].y+TILE_HEIGHT,2,1,0,4);
                        }

                        enemies[i].speed = enemies[i].base_speed/2.0f;
                    }
                    
                    else if(collision_value_1 == 4 || collision_value_2 == 4 || collision_value_3 == 4 || collision_value_4 == 4)
                    {
                        // handle enemy in water
                        if(enemies[i].x_vel != 0 || enemies[i].y_vel != 0)
                        {
                            spawn_particle(rand() % TILE_WIDTH + enemies[i].x,enemies[i].y+TILE_HEIGHT,2,2,0,8);
                        }

                        enemies[i].speed = enemies[i].base_speed/3.0f;
                    }
                    else if(collision_value_1 == 6 || collision_value_2 == 6 || collision_value_3 == 6 || collision_value_4 == 6)
                    {
                        // handle enemy in lava
                        if(enemies[i].x_vel != 0 || enemies[i].y_vel != 0)
                        {
                            spawn_particle(rand() % TILE_WIDTH + enemies[i].x,enemies[i].y+TILE_HEIGHT,2,2,0,6);
                        }

                        enemies[i].environmental_hurt_counter++;
                        if(enemies[i].environmental_hurt_counter >= enemies[i].environmental_hurt_max)
                        {
                            enemies[i].environmental_hurt_counter = 0;
                            enemies[i].hp--;
                            if(enemies[i].hp <= 0)
                            {
                                enemy_death(i);
                            }

                            spawn_floating_number(enemies[i].x+TILE_WIDTH/2,enemies[i].y,1,6);
                        }

                        enemies[i].speed = enemies[i].base_speed/3.0f;
                    }
                    else
                    {
                        enemies[i].speed = enemies[i].base_speed;
                    }

                    // keep enemy in world boundaries
                    if(enemies[i].x < 0) enemies[i].x = 0;
                    if(enemies[i].y < 0) enemies[i].y = 0;
                    if(enemies[i].x >TILE_WIDTH*(WORLD_TILE_WIDTH-1)) enemies[i].x = TILE_WIDTH*(WORLD_TILE_WIDTH-1);
                    if(enemies[i].y >TILE_HEIGHT*(WORLD_TILE_HEIGHT-1)) enemies[i].y = TILE_HEIGHT*(WORLD_TILE_HEIGHT-1);

                    if(enemies[i].mode == ENEMY_MODE_PURSUE)
                    {
                        // update particles
                        enemies[i].particle_spawn_counter++;
                        if(enemies[i].particle_spawn_counter >= 30)
                        {
                            enemies[i].particle_spawn_counter = 0;
                            spawn_particle(rand() % TILE_WIDTH + enemies[i].x,enemies[i].y,1,2,CHAR_HEART,12);
                            spawn_particle(rand() % TILE_WIDTH + enemies[i].x,enemies[i].y,1,2,CHAR_HEART,12);
                            spawn_particle(rand() % TILE_WIDTH + enemies[i].x,enemies[i].y,1,2,CHAR_HEART,12);
                        }

                    }
                    
                }
            }
            else
            {
                enemies[i].action_counter++;

                if(enemies[i].action_counter >= enemies[i].action_counter_max)
                {
                    // choose an action to make
                    enemies[i].action_counter = 0;
                    enemies[i].state = ENEMY_STATE_ACTING;

                    switch(enemies[i].mode)
                    {
                        case ENEMY_MODE_WANDER:

                            enemies[i].action_counter_max = 60;
                            enemies[i].action_duration_counter_max = rand() % 60 + 1;

                            int d = rand() % 9;

                            switch (d)
                            {
                                case 0:
                                    enemies[i].x_vel = +0;
                                    enemies[i].y_vel = +0;
                                case 1: 
                                    enemies[i].dir = DIR_UP;
                                    enemies[i].x_vel = +0;
                                    enemies[i].y_vel = -1;
                                    break;
                                case 2: 
                                    enemies[i].dir = DIR_DOWN;
                                    enemies[i].x_vel = +0;
                                    enemies[i].y_vel = +1;
                                    break;
                                case 3: 
                                    enemies[i].dir = DIR_LEFT;
                                    enemies[i].x_vel = -1;
                                    enemies[i].y_vel = +0;
                                    break;
                                case 4: 
                                    enemies[i].dir = DIR_RIGHT;
                                    enemies[i].x_vel = +1;
                                    enemies[i].y_vel = +0;
                                    break;
                                case 5: 
                                    enemies[i].dir = DIR_UP;
                                    enemies[i].x_vel = -1;
                                    enemies[i].y_vel = -1;
                                    break;
                                case 6: 
                                    enemies[i].dir = DIR_UP;
                                    enemies[i].x_vel = +1;
                                    enemies[i].y_vel = -1;
                                    break;
                                case 7: 
                                    enemies[i].dir = DIR_DOWN;
                                    enemies[i].x_vel = -1;
                                    enemies[i].y_vel = +1;
                                    break;
                                case 8: 
                                    enemies[i].dir = DIR_DOWN;
                                    enemies[i].x_vel = +1;
                                    enemies[i].y_vel = +1;
                                    break;
                            }

                            break;

                        case ENEMY_MODE_PURSUE:
                            
                            enemies[i].action_counter_max  = 0;
                            enemies[i].action_duration_counter_max = 20; 

                            float diff_x = enemies[i].x - player.x;
                            float diff_y = enemies[i].y - player.y;

                            float angle  = atan(diff_y/diff_x);
                            float EIGHTH_PI = PI/8.0f;

                            if(angle >= -4*EIGHTH_PI && angle < -3*EIGHTH_PI)
                            {
                                if(diff_y > 0)
                                {
                                    // UP 
                                    enemies[i].dir = DIR_UP;
                                    enemies[i].x_vel = +0;
                                    enemies[i].y_vel = -1;
                                }
                                else
                                {
                                    // DOWN
                                    enemies[i].dir = DIR_DOWN;
                                    enemies[i].x_vel = +0;
                                    enemies[i].y_vel = +1;
                                }
                            }
                            else if(angle >= -3*EIGHTH_PI && angle < -1*EIGHTH_PI)
                            {
                                if(diff_x > 0)
                                {
                                    // DOWN LEFT
                                    enemies[i].dir = DIR_DOWN;
                                    enemies[i].x_vel = -1;
                                    enemies[i].y_vel = +1;
                                }
                                else
                                {
                                    // UP RIGHT
                                    enemies[i].dir = DIR_UP;
                                    enemies[i].x_vel = +1;
                                    enemies[i].y_vel = -1;
                                }
                            }
                            else if(angle >= -1*EIGHTH_PI && angle < +1*EIGHTH_PI)
                            {
                                if(diff_x > 0)
                                {
                                    // LEFT
                                    enemies[i].dir = DIR_LEFT;
                                    enemies[i].x_vel = -1;
                                    enemies[i].y_vel = +0;
                                }
                                else
                                {
                                    // RIGHT
                                    enemies[i].dir = DIR_RIGHT;
                                    enemies[i].x_vel = +1;
                                    enemies[i].y_vel = +0;
                                }
                            }
                            else if(angle >= +1*EIGHTH_PI && angle < +3*EIGHTH_PI)
                            {
                                if(diff_x > 0)
                                {
                                    // UP LEFT
                                    enemies[i].dir = DIR_UP;
                                    enemies[i].x_vel = -1;
                                    enemies[i].y_vel = -1;
                                }
                                else
                                {
                                    // DOWN RIGHT
                                    enemies[i].dir = DIR_DOWN;
                                    enemies[i].x_vel = +1;
                                    enemies[i].y_vel = +1;
                                }
                            }
                            else if(angle <= +4*EIGHTH_PI && angle >= +3*EIGHTH_PI)
                            {
                                if(diff_y > 0)
                                {
                                    // UP 
                                    enemies[i].dir = DIR_UP;
                                    enemies[i].x_vel = +0;
                                    enemies[i].y_vel = -1;
                                }
                                else
                                {
                                    // DOWN
                                    enemies[i].dir = DIR_DOWN;
                                    enemies[i].x_vel = +0;
                                    enemies[i].y_vel = +1;
                                }
                            }
                        break;
                    }
                }
            }
            
            // handle enemy animation
            if(enemies[i].x_vel != 0 || enemies[i].y_vel != 0)
            {
                enemies[i].anim.counter++;

                if(enemies[i].anim.counter >= 10/enemies[i].speed)
                {
                    // cycle_animation
                    enemies[i].anim.counter = 0;
                    enemies[i].anim.frame += 1;
                    if(enemies[i].anim.frame >= enemies[i].anim.num_frames)
                        enemies[i].anim.frame = 0;
                }
            }
            else
            {
                // clear animation frame
                enemies[i].anim.counter = 0;
                enemies[i].anim.frame = 0;
            }
        }
    }
}

static void draw_enemy(int i)
{
        int enemy_x = enemies[i].x - camera.x;
        int enemy_y = enemies[i].y - camera.y;

        if (enemies[i].state == ENEMY_STATE_STUNNED)
        {
            // draw red tint
            draw_tile_tinted(enemies[i].x - camera.x, enemies[i].y - camera.y, enemies[i].tile_index + enemies[i].dir + enemies[i].anim.frame_order[enemies[i].anim.frame],6,day_cycle_shade_amount);
        }
        else
        {
            draw_tile(enemies[i].x - camera.x, enemies[i].y - camera.y, enemies[i].tile_index + enemies[i].dir + enemies[i].anim.frame_order[enemies[i].anim.frame],day_cycle_shade_amount);
        }

}

static void draw_enemies()
{
	for (int i = 0; i < num_enemies; ++i)
	{
        draw_enemy(i);
	}

}

