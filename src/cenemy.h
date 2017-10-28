#define MAX_ENEMIES 1000

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
    int action_counter;
    int action_counter_max;
    int action_duration_counter;
    int action_duration_counter_max;
    int particle_spawn_counter;
    BOOL untargetable;
    EnemyState state;
    EnemyMode  mode;
    Direction dir;
    Animation anim;
} Enemy;

Enemy enemies[MAX_ENEMIES];

int num_enemies;

static void init_enemies()
{
    for (int i = 0; i < MAX_ENEMIES ; ++i)
    {
        int test_x, test_y;
        int test_collision_1, test_collision_2, test_collision_3, test_collision_4;
        BOOL collision = TRUE;

        // make sure not to spawn enemy in a solid object
        while(collision)
        {
            test_x = rand() % (TILE_WIDTH*(WORLD_TILE_WIDTH -1));
            test_y = rand() % (TILE_HEIGHT*(WORLD_TILE_HEIGHT -1));

            test_collision_1 = world_collision[test_x/TILE_WIDTH][test_y/TILE_HEIGHT]; 
            test_collision_2 = world_collision[(test_x + TILE_WIDTH)/TILE_WIDTH][(test_y + TILE_HEIGHT)/TILE_HEIGHT];
            test_collision_3 = world_collision[test_x/TILE_WIDTH][(test_y+TILE_HEIGHT)/TILE_HEIGHT]; 
            test_collision_4 = world_collision[(test_x + TILE_WIDTH)/TILE_WIDTH][test_y/TILE_HEIGHT];
            
            collision = (test_collision_1 == 5 && test_collision_2 == 5 || test_collision_3 == 5 || test_collision_4 == 5);
        }

        if(i < 200)
        {
            enemies[num_enemies].x = test_x; 
            enemies[num_enemies].y = test_y;
            enemies[num_enemies].x_vel = 0;
            enemies[num_enemies].y_vel = 0;
            enemies[num_enemies].speed = 0.9f;
            enemies[num_enemies].base_speed = 0.9f;
            enemies[num_enemies].dir = DIR_DOWN;
            enemies[num_enemies].state = ENEMY_STATE_NEUTRAL;
            enemies[num_enemies].mode  = ENEMY_MODE_WANDER;
            enemies[num_enemies].name = "White Rat";
            enemies[num_enemies].tile_index = 144;
            enemies[num_enemies].hp = 10;
            enemies[num_enemies].max_hp = 10;
            enemies[num_enemies].xp = 2;
            enemies[num_enemies].gold_drop_max = 5;
            enemies[num_enemies].aggro_radius = 50;
            enemies[num_enemies].particle_spawn_counter = 0;
            enemies[num_enemies].stun_counter = 0;
            enemies[num_enemies].stun_duration = 20;
            enemies[num_enemies].untargetable = FALSE;
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
        }
        else if(i < 700)
        {
            enemies[num_enemies].x = test_x; 
            enemies[num_enemies].y = test_y;
            enemies[num_enemies].x_vel = 0;
            enemies[num_enemies].y_vel = 0;
            enemies[num_enemies].speed = 0.5f;
            enemies[num_enemies].base_speed = 0.5f;
            enemies[num_enemies].dir = DIR_DOWN;
            enemies[num_enemies].state = ENEMY_STATE_NEUTRAL;
            enemies[num_enemies].mode  = ENEMY_MODE_WANDER;
            enemies[num_enemies].name = "Rat";
            enemies[num_enemies].tile_index = 16;
            enemies[num_enemies].hp = 5;
            enemies[num_enemies].max_hp = 5;
            enemies[num_enemies].xp = 1;
            enemies[num_enemies].gold_drop_max = 2;
            enemies[num_enemies].aggro_radius = 50;
            enemies[num_enemies].particle_spawn_counter = 0;
            enemies[num_enemies].stun_counter = 0;
            enemies[num_enemies].stun_duration = 20;
            enemies[num_enemies].untargetable = FALSE;
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
        }
        else
        {
            enemies[num_enemies].x = test_x; 
            enemies[num_enemies].y = test_y;
            enemies[num_enemies].x_vel = 0;
            enemies[num_enemies].y_vel = 0;
            enemies[num_enemies].speed = 0.8f;
            enemies[num_enemies].base_speed = 0.8f;
            enemies[num_enemies].dir = DIR_DOWN;
            enemies[num_enemies].state = ENEMY_STATE_NEUTRAL;
            enemies[num_enemies].mode  = ENEMY_MODE_WANDER;
            enemies[num_enemies].name = "Orc";
            enemies[num_enemies].tile_index = 128;
            enemies[num_enemies].hp = 20;
            enemies[num_enemies].max_hp = 20; 
            enemies[num_enemies].xp = 5;
            enemies[num_enemies].gold_drop_max = 12;
            enemies[num_enemies].aggro_radius = 50;
            enemies[num_enemies].particle_spawn_counter = 0;
            enemies[num_enemies].stun_counter = 0;
            enemies[num_enemies].stun_duration = 20;
            enemies[num_enemies].untargetable = FALSE;
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
        }

        num_enemies++;
    }

}

static void remove_enemy(int index)
{
	num_enemies--;
	enemies[index] = enemies[num_enemies];
}

static void update_enemies()
{
    for(int i = 0; i < num_enemies;++i)
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

static void draw_enemies()
{
	for (int i = 0; i < num_enemies; ++i)
	{
        int enemy_x = enemies[i].x - camera.x;
        int enemy_y = enemies[i].y - camera.y;

        if (enemy_x+TILE_WIDTH < 0 || enemy_x > buffer_width)
            continue;
        if (enemy_y+TILE_HEIGHT < 0 || enemy_y > buffer_height)
            continue;
        
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

}

