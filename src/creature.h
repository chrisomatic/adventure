#define MAX_CREATURES    10000
#define MAX_CREATURE_LIST 100

typedef enum
{
    CREATURE_STATE_NEUTRAL,
    CREATURE_STATE_ACTING,
    CREATURE_STATE_STUNNED
} CreatureState;

typedef enum
{
    CREATURE_MODE_WANDER,
    CREATURE_MODE_PURSUE
} CreatureMode;

typedef enum
{
    CREATURE_BEHAVIOR_PASSIVE,
    CREATURE_BEHAVIOR_AGGRESSIVE
} CreatureBehavior;

typedef enum
{
    MALE,
    FEMALE
} Gender;

typedef struct
{
    char* name;
    char* species;
    char* tileset_name;
    int tile_index;
    int xp;
    int hp;
    int max_hp;
    float x;
    float y;
    float z;
    float height;
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
    CreatureState state;
    CreatureMode  mode;
    CreatureBehavior behavior;
    Direction dir;
    Animation anim;
    Gender gender;
    BOOL reproductive;
    BOOL pregnant;
    int gestation_period;
    int gestation_counter; 
    int mating_radius;
    int grouping_radius;
    int age;
    int age_counter;
    int adult_age;
    int life_span_min;
    int life_span_max;
} Creature;

Creature creatures[MAX_CREATURES];
Creature creature_list[MAX_CREATURE_LIST];

int num_creatures = 0;
int num_creature_list = 0;

static BOOL get_creature_by_name(const char* name,Creature* creature)
{
	if (name == NULL)
		return FALSE;
	
    for(int i = 0; i < MAX_CREATURE_LIST; ++i)
    {
		if (creature_list[i].name == NULL)
			continue;

        if(strcmp(creature_list[i].name, name) == 0)
        {
            creature->name = creature_list[i].name;
			creature->hp = creature_list[i].hp;
			creature->max_hp = creature_list[i].max_hp;
			creature->xp = creature_list[i].xp;
            creature->speed = creature_list[i].speed;
            creature->gold_drop_max = creature_list[i].gold_drop_max;
            creature->behavior = creature_list[i].behavior;
            creature->untargetable = creature_list[i].untargetable;
            creature->tileset_name = creature_list[i].tileset_name;
            creature->tile_index = creature_list[i].tile_index;
            creature->reproductive = creature_list[i].reproductive;
            creature->species = creature_list[i].species;

            return TRUE;
        }
    }

	return FALSE;
}

static BOOL spawn_creature(const char* creature_name,float x, float y)
{
    Creature creature = {0};

    if(!get_creature_by_name(creature_name,&creature))
        return FALSE;

    creatures[num_creatures].x = x; 
    creatures[num_creatures].y = y;
    creatures[num_creatures].z = 0.0f;
    creatures[num_creatures].x_vel = 0;
    creatures[num_creatures].y_vel = 0;
    creatures[num_creatures].height = 16.0f;
    creatures[num_creatures].speed = creature.speed;
    creatures[num_creatures].base_speed = creature.speed;
    creatures[num_creatures].dir = DIR_DOWN;
    creatures[num_creatures].state = CREATURE_STATE_NEUTRAL;
    creatures[num_creatures].mode  = CREATURE_MODE_WANDER;
    creatures[num_creatures].name = creature.name;
    creatures[num_creatures].tileset_name = creature.tileset_name;
    creatures[num_creatures].tile_index = creature.tile_index;
    creatures[num_creatures].hp = creature.hp;
    creatures[num_creatures].max_hp = creature.max_hp;
    creatures[num_creatures].xp = creature.xp;
    creatures[num_creatures].gold_drop_max = creature.gold_drop_max;
    creatures[num_creatures].aggro_radius = 50;
    creatures[num_creatures].particle_spawn_counter = 0;
    creatures[num_creatures].environmental_hurt_counter = 60;
    creatures[num_creatures].environmental_hurt_max = 60;
    creatures[num_creatures].stun_counter = 0;
    creatures[num_creatures].stun_duration = 20;
    creatures[num_creatures].untargetable = creature.untargetable;
    creatures[num_creatures].behavior = creature.behavior;
    creatures[num_creatures].gender = rand() % 2;
    creatures[num_creatures].action_counter_max = 60;
    creatures[num_creatures].action_counter = rand() % creatures[num_creatures].action_counter_max;
    creatures[num_creatures].action_duration_counter = 0;
    creatures[num_creatures].anim.counter = 0;
    creatures[num_creatures].anim.max_count = 10;
    creatures[num_creatures].anim.frame = 0;
    creatures[num_creatures].anim.num_frames = 4;
    creatures[num_creatures].anim.frame_order[0] = 0;
    creatures[num_creatures].anim.frame_order[1] = 1;
    creatures[num_creatures].anim.frame_order[2] = 0;
    creatures[num_creatures].anim.frame_order[3] = 2;
    creatures[num_creatures].gestation_period = 3600;
    creatures[num_creatures].gestation_counter = 0;
    creatures[num_creatures].mating_radius = 16;
    creatures[num_creatures].grouping_radius = 50;
    creatures[num_creatures].age = (rand()%60 + 1);
    creatures[num_creatures].age_counter = 0;
    creatures[num_creatures].adult_age = 60;
    creatures[num_creatures].life_span_min = 120;
    creatures[num_creatures].life_span_max = 300;
	creatures[num_creatures].reproductive = creature.reproductive;
	creatures[num_creatures].species = creature.species;
	creatures[num_creatures].pregnant = FALSE;

    ++num_creatures;

    if(num_creatures > MAX_CREATURES -1)
    {
        num_creatures = MAX_CREATURES - 1;
        return FALSE;
    }

    return TRUE;
}

static void init_creatures()
{
    num_creatures = 0;

    for (int i = 0; i < MAX_CREATURES ; ++i)
    {
        int creature_x, creature_y;
        int test_collision_1, test_collision_2, test_collision_3, test_collision_4;
        BOOL collision = TRUE;

        // make sure not to spawn creature in a solid object
        while(collision)
        {
            creature_x = rand() % (TILE_WIDTH*(WORLD_TILE_WIDTH -1));
            creature_y = rand() % (TILE_HEIGHT*(WORLD_TILE_HEIGHT -1));

            test_collision_1 = world_collision[creature_x/TILE_WIDTH][creature_y/TILE_HEIGHT]; 
            test_collision_2 = world_collision[(creature_x + TILE_WIDTH)/TILE_WIDTH][(creature_y + TILE_HEIGHT)/TILE_HEIGHT];
            test_collision_3 = world_collision[creature_x/TILE_WIDTH][(creature_y+TILE_HEIGHT)/TILE_HEIGHT]; 
            test_collision_4 = world_collision[(creature_x + TILE_WIDTH)/TILE_WIDTH][creature_y/TILE_HEIGHT];
            
            collision = (test_collision_1 == 5 && test_collision_2 == 5 || test_collision_3 == 5 || test_collision_4 == 5);
        }

		if (num_creature_list == 0)
			return;

		int creature_type = rand() % 2;//num_creature_list;
        //spawn_creature(creature_list[creature_type].name,creature_x,creature_y);

        // @TEMP
        if(creature_type == 0)
            spawn_creature("Rat",creature_x,creature_y);
        else
            spawn_creature("White Rat",creature_x,creature_y);


    }
}

static void remove_creature(int index)
{
	num_creatures--;
	creatures[index] = creatures[num_creatures];
}

static void creature_death(int i)
{
    for(int p = 0; p < 10; ++p)
        spawn_particle(rand()%TILE_WIDTH + creatures[i].x,rand()%TILE_HEIGHT+creatures[i].y,rand() % 4 + 1,3,0,6);

    int coins_to_spawn = rand() % (creatures[i].gold_drop_max+1);

    int gold_coins = coins_to_spawn / 100; coins_to_spawn -= (gold_coins*100);
    int silver_coins = coins_to_spawn / 10; coins_to_spawn -= (silver_coins*10);
    int bronze_coins = coins_to_spawn / 1; coins_to_spawn -= bronze_coins;

    for(int c = 0; c < bronze_coins; ++c)
        spawn_coin(creatures[i].x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),creatures[i].y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)),2,0,0,3.0f, COIN_BRONZE);
    for(int c = 0; c < silver_coins; ++c)
        spawn_coin(creatures[i].x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),creatures[i].y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)),2,0,0,2.5f, COIN_SILVER);
    for(int c = 0; c < gold_coins; ++c)
        spawn_coin(creatures[i].x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),creatures[i].y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)),2,0,0,2.0f, COIN_GOLD);

    // drop item(s)
    int item_percent = rand() % 100 + 1;
    if(item_percent >= 1 && item_percent <= 20)
    {
        spawn_item("Meat",creatures[i].x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),creatures[i].y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)));
    }
    else if(item_percent >= 21 && item_percent <= 40)
    {
        spawn_item("Mana Vial",creatures[i].x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),creatures[i].y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)));
    }
    else if(item_percent >= 41 && item_percent <= 42)
    {
        spawn_item("Knife",creatures[i].x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),creatures[i].y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)));
    }
    else if(item_percent >= 43 && item_percent <= 44)
    {
        spawn_item("Sword",creatures[i].x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),creatures[i].y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)));
    }
    else if(item_percent >= 45 && item_percent <= 46)
    {
        spawn_item("Axe",creatures[i].x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),creatures[i].y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)));
    }
    else if(item_percent >= 47 && item_percent <= 48)
    {
        spawn_item("Bow",creatures[i].x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),creatures[i].y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)));
    }
    else if(item_percent >= 49 && item_percent <= 50)
    {
        spawn_item("Staff",creatures[i].x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),creatures[i].y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)));
    }

    remove_creature(i);

}

static void update_creatures()
{
    for(int i = num_creatures -1; i >= 0;--i)
    {
        if(creatures[i].state == CREATURE_STATE_STUNNED)
        {
            creatures[i].stun_counter++;
            if(creatures[i].stun_counter >= creatures[i].stun_duration)
            {
                creatures[i].stun_counter = 0;
                creatures[i].state = CREATURE_STATE_NEUTRAL;
            }
        }
        else
        {
            if(creatures[i].behavior == CREATURE_BEHAVIOR_AGGRESSIVE)
            {
                // check distance from player
                if(get_distance(player.x + TILE_WIDTH/2,player.y + TILE_HEIGHT/2,creatures[i].x + TILE_WIDTH/2,creatures[i].y+TILE_HEIGHT/2) <= creatures[i].aggro_radius)
                {
                    if(creatures[i].mode == CREATURE_MODE_WANDER)
                    {
                        creatures[i].state = CREATURE_STATE_NEUTRAL;
                        creatures[i].action_counter = creatures[i].action_counter_max;
                    }

                    creatures[i].mode = CREATURE_MODE_PURSUE;
                }
                else
                {
                    creatures[i].mode = CREATURE_MODE_WANDER;
                }
            }
            else
            {
                creatures[i].mode = CREATURE_MODE_WANDER;
            }
            
            // if creature is performing an action, let it play out
            if(creatures[i].state == CREATURE_STATE_ACTING)
            {

                creatures[i].action_duration_counter++;

                if(creatures[i].action_duration_counter >= creatures[i].action_duration_counter_max)
                {
                        creatures[i].action_duration_counter = 0;
                        creatures[i].state = CREATURE_STATE_NEUTRAL; // return to neutral state
                        
                        if(creatures[i].mode != CREATURE_MODE_PURSUE)
                        {
                            creatures[i].x_vel = +0;
                            creatures[i].y_vel = +0;
                        }
                }
                
                // update creature position
                creatures[i].x += creatures[i].x_vel*creatures[i].speed;
                creatures[i].y += creatures[i].y_vel*creatures[i].speed;

                if(creatures[i].x_vel != 0 || creatures[i].y_vel != 0)
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
                    int creature_check_x1 = (creatures[i].x + TILE_WIDTH/4) / TILE_WIDTH;
                    int creature_check_y1 = (creatures[i].y + TILE_HEIGHT/2) / TILE_HEIGHT;

                    int creature_check_x2 = (creatures[i].x + 3*TILE_WIDTH/4) / TILE_WIDTH;
                    int creature_check_y2 = (creatures[i].y + TILE_HEIGHT/2) / TILE_HEIGHT;

                    int creature_check_x3 = (creatures[i].x + TILE_WIDTH/4) / TILE_WIDTH;
                    int creature_check_y3 = (creatures[i].y + TILE_HEIGHT) / TILE_HEIGHT;

                    int creature_check_x4 = (creatures[i].x + 3*TILE_WIDTH/4) / TILE_WIDTH;
                    int creature_check_y4 = (creatures[i].y + TILE_HEIGHT) / TILE_HEIGHT;

                    int collision_value_1 = world_collision[creature_check_x1][creature_check_y1];
                    int collision_value_2 = world_collision[creature_check_x2][creature_check_y2];
                    int collision_value_3 = world_collision[creature_check_x3][creature_check_y3];
                    int collision_value_4 = world_collision[creature_check_x4][creature_check_y4];

                    if(collision_value_1 == 5 || collision_value_2 == 5 || collision_value_3 == 5 || collision_value_4 == 5)
                    {
                        BOOL correct_x = FALSE;
                        BOOL correct_y = FALSE;

                        if((collision_value_1 == 5 && collision_value_3 == 5) || (collision_value_2 == 5 && collision_value_4 == 5))
                        {
                            //correct collision x
                            creatures[i].x -= creatures[i].x_vel*creatures[i].speed;
                            correct_x = TRUE;
                        }
                        if((collision_value_1 == 5 && collision_value_2 == 5) || (collision_value_3 == 5 && collision_value_4 == 5))
                        {
                            //correct collision y
                            creatures[i].y -= creatures[i].y_vel*creatures[i].speed;
                            correct_y = TRUE;
                        }
                        
                        if(!correct_x && !correct_y)
                        {
                            if(collision_value_1 == 5 || collision_value_3 == 5)
                                creatures[i].x += 1.0f*creatures[i].speed;
                            
                            if(collision_value_2 == 5 || collision_value_4 == 5)
                                creatures[i].x -= 1.0f*creatures[i].speed;
                        }
                    }
                    else if(collision_value_1 == 3 || collision_value_2 == 3 || collision_value_3 == 3 || collision_value_4 == 3)
                    {
                        // handle creature in mud
                        if(creatures[i].x_vel != 0 || creatures[i].y_vel != 0)
                        {
                            spawn_particle(rand() % TILE_WIDTH + creatures[i].x,creatures[i].y+TILE_HEIGHT,2,1,0,4);
                        }

                        creatures[i].speed = creatures[i].base_speed/2.0f;
                    }
                    
                    else if(collision_value_1 == 4 || collision_value_2 == 4 || collision_value_3 == 4 || collision_value_4 == 4)
                    {
                        // handle creature in water
                        if(creatures[i].x_vel != 0 || creatures[i].y_vel != 0)
                        {
                            spawn_particle(rand() % TILE_WIDTH + creatures[i].x,creatures[i].y+TILE_HEIGHT,2,2,0,8);
                        }

                        creatures[i].speed = creatures[i].base_speed/3.0f;
                    }
                    else if(collision_value_1 == 6 || collision_value_2 == 6 || collision_value_3 == 6 || collision_value_4 == 6)
                    {
                        // handle creature in lava
                        if(creatures[i].x_vel != 0 || creatures[i].y_vel != 0)
                        {
                            spawn_particle(rand() % TILE_WIDTH + creatures[i].x,creatures[i].y+TILE_HEIGHT,2,2,0,6);
                        }

                        creatures[i].environmental_hurt_counter++;
                        if(creatures[i].environmental_hurt_counter >= creatures[i].environmental_hurt_max)
                        {
                            creatures[i].environmental_hurt_counter = 0;
                            creatures[i].hp--;
                            if(creatures[i].hp <= 0)
                            {
                                creature_death(i);
                            }

                            spawn_floating_number(creatures[i].x+TILE_WIDTH/2,creatures[i].y,1,6);
                        }

                        creatures[i].speed = creatures[i].base_speed/3.0f;
                    }
                    else
                    {
                        creatures[i].speed = creatures[i].base_speed;
                    }

                    // keep creature in world boundaries
                    if(creatures[i].x < 0) creatures[i].x = 0;
                    if(creatures[i].y < 0) creatures[i].y = 0;
                    if(creatures[i].x >TILE_WIDTH*(WORLD_TILE_WIDTH-1)) creatures[i].x = TILE_WIDTH*(WORLD_TILE_WIDTH-1);
                    if(creatures[i].y >TILE_HEIGHT*(WORLD_TILE_HEIGHT-1)) creatures[i].y = TILE_HEIGHT*(WORLD_TILE_HEIGHT-1);

                    if(creatures[i].mode == CREATURE_MODE_PURSUE)
                    {
                        // update particles
                        creatures[i].particle_spawn_counter++;
                        if(creatures[i].particle_spawn_counter >= 30)
                        {
                            creatures[i].particle_spawn_counter = 0;
                            spawn_particle(rand() % TILE_WIDTH + creatures[i].x,creatures[i].y,1,2,CHAR_HEART,12);
                            spawn_particle(rand() % TILE_WIDTH + creatures[i].x,creatures[i].y,1,2,CHAR_HEART,12);
                            spawn_particle(rand() % TILE_WIDTH + creatures[i].x,creatures[i].y,1,2,CHAR_HEART,12);
                        }

                    }
                    
                }
            }
            else
            {
                creatures[i].action_counter++;

                if(creatures[i].action_counter >= creatures[i].action_counter_max)
                {
                    // choose an action to make
                    creatures[i].action_counter = 0;
                    creatures[i].state = CREATURE_STATE_ACTING;

                    switch(creatures[i].mode)
                    {
                        case CREATURE_MODE_WANDER:

                            creatures[i].action_counter_max = 60;
                            creatures[i].action_duration_counter_max = rand() % 60 + 1;

                            int d = rand() % 9;

                            switch (d)
                            {
                                case 0:
                                    creatures[i].x_vel = +0;
                                    creatures[i].y_vel = +0;
                                    break;
                                case 1: 
                                    creatures[i].dir = DIR_UP;
                                    creatures[i].x_vel = +0;
                                    creatures[i].y_vel = -1;
                                    break;
                                case 2: 
                                    creatures[i].dir = DIR_DOWN;
                                    creatures[i].x_vel = +0;
                                    creatures[i].y_vel = +1;
                                    break;
                                case 3: 
                                    creatures[i].dir = DIR_LEFT;
                                    creatures[i].x_vel = -1;
                                    creatures[i].y_vel = +0;
                                    break;
                                case 4: 
                                    creatures[i].dir = DIR_RIGHT;
                                    creatures[i].x_vel = +1;
                                    creatures[i].y_vel = +0;
                                    break;
                                case 5: 
                                    creatures[i].dir = DIR_UP;
                                    creatures[i].x_vel = -1;
                                    creatures[i].y_vel = -1;
                                    break;
                                case 6: 
                                    creatures[i].dir = DIR_UP;
                                    creatures[i].x_vel = +1;
                                    creatures[i].y_vel = -1;
                                    break;
                                case 7: 
                                    creatures[i].dir = DIR_DOWN;
                                    creatures[i].x_vel = -1;
                                    creatures[i].y_vel = +1;
                                    break;
                                case 8: 
                                    creatures[i].dir = DIR_DOWN;
                                    creatures[i].x_vel = +1;
                                    creatures[i].y_vel = +1;
                                    break;
                            }

                            break;

                        case CREATURE_MODE_PURSUE:
                            
                            creatures[i].action_counter_max  = 0;
                            creatures[i].action_duration_counter_max = 20; 

                            float diff_x = creatures[i].x - player.x;
                            float diff_y = creatures[i].y - player.y;

                            float angle  = atan(diff_y/diff_x);
                            double EIGHTH_PI = PI/8.0;

                            if(angle >= -4*EIGHTH_PI && angle < -3*EIGHTH_PI)
                            {
                                if(diff_y > 0)
                                {
                                    // UP 
                                    creatures[i].dir = DIR_UP;
                                    creatures[i].x_vel = +0;
                                    creatures[i].y_vel = -1;
                                }
                                else
                                {
                                    // DOWN
                                    creatures[i].dir = DIR_DOWN;
                                    creatures[i].x_vel = +0;
                                    creatures[i].y_vel = +1;
                                }
                            }
                            else if(angle >= -3*EIGHTH_PI && angle < -1*EIGHTH_PI)
                            {
                                if(diff_x > 0)
                                {
                                    // DOWN LEFT
                                    creatures[i].dir = DIR_DOWN;
                                    creatures[i].x_vel = -1;
                                    creatures[i].y_vel = +1;
                                }
                                else
                                {
                                    // UP RIGHT
                                    creatures[i].dir = DIR_UP;
                                    creatures[i].x_vel = +1;
                                    creatures[i].y_vel = -1;
                                }
                            }
                            else if(angle >= -1*EIGHTH_PI && angle < +1*EIGHTH_PI)
                            {
                                if(diff_x > 0)
                                {
                                    // LEFT
                                    creatures[i].dir = DIR_LEFT;
                                    creatures[i].x_vel = -1;
                                    creatures[i].y_vel = +0;
                                }
                                else
                                {
                                    // RIGHT
                                    creatures[i].dir = DIR_RIGHT;
                                    creatures[i].x_vel = +1;
                                    creatures[i].y_vel = +0;
                                }
                            }
                            else if(angle >= +1*EIGHTH_PI && angle < +3*EIGHTH_PI)
                            {
                                if(diff_x > 0)
                                {
                                    // UP LEFT
                                    creatures[i].dir = DIR_UP;
                                    creatures[i].x_vel = -1;
                                    creatures[i].y_vel = -1;
                                }
                                else
                                {
                                    // DOWN RIGHT
                                    creatures[i].dir = DIR_DOWN;
                                    creatures[i].x_vel = +1;
                                    creatures[i].y_vel = +1;
                                }
                            }
                            else if(angle <= +4*EIGHTH_PI && angle >= +3*EIGHTH_PI)
                            {
                                if(diff_y > 0)
                                {
                                    // UP 
                                    creatures[i].dir = DIR_UP;
                                    creatures[i].x_vel = +0;
                                    creatures[i].y_vel = -1;
                                }
                                else
                                {
                                    // DOWN
                                    creatures[i].dir = DIR_DOWN;
                                    creatures[i].x_vel = +0;
                                    creatures[i].y_vel = +1;
                                }
                            }
                        break;
                    }
                }
            }
            
            // handle creature animation
            if(creatures[i].x_vel != 0 || creatures[i].y_vel != 0)
            {
                creatures[i].anim.counter++;

                if(creatures[i].anim.counter >= 10/creatures[i].speed)
                {
                    // cycle_animation
                    creatures[i].anim.counter = 0;
                    creatures[i].anim.frame += 1;
                    if(creatures[i].anim.frame >= creatures[i].anim.num_frames)
                        creatures[i].anim.frame = 0;
                }
            }
            else
            {
                // clear animation frame
                creatures[i].anim.counter = 0;
                creatures[i].anim.frame = 0;
            }

            // handle creature mating
            if(creatures[i].reproductive && creatures[i].gender == MALE && creatures[i].age >= creatures[i].adult_age)
            {
                for(int j = 0; j < num_creatures; ++j)
                {
                    // is creature reproductive and an adult?
                    if(creatures[j].reproductive && creatures[j].age >= creatures[j].adult_age)
                    {
                        // are creatures the same species?
                        if(strcmp(creatures[j].species, creatures[i].species) == 0)
                        {
                            // are creatures opposite gender?
                            if(creatures[j].gender = !creatures[i].gender)
                            {
                                // is female currently not pregnant?
                                if(!creatures[j].pregnant)
                                {
                                    // look at distance
                                    double distance = get_distance(creatures[i].x,creatures[i].y,creatures[j].x, creatures[j].y);

                                    if(distance <= creatures[i].mating_radius)
                                    {
                                        // spawn flames
                                        for(int k = 0; k < 50; ++k)
                                            spawn_particle(creatures[j].x + (rand()%TILE_WIDTH), creatures[j].y + (rand()%(TILE_HEIGHT/2)), rand()%2 + 1,3,CHAR_FLAME,6); 

                                        creatures[j].pregnant = TRUE;
                                    }
                                }
                            }
                        } 
                    }
                }
            }
            
            // handle pregnancies
            if(creatures[i].pregnant)
            {
                // spawn heart (indicates pregnancy)
                spawn_particle(creatures[i].x + (rand()%TILE_WIDTH), creatures[i].y + (rand()%(TILE_HEIGHT/2)),1,3,CHAR_HEART,6); 

                ++creatures[i].gestation_counter;
                if(creatures[i].gestation_counter >= creatures[i].gestation_period)
                {
                    creatures[i].gestation_counter = 0;

                    // have a baby
                    spawn_creature(creatures[i].name,creatures[i].x,creatures[i].y);

                    // have a party
					int c[5] = { 6,9,11,13,14 };

                    for(int j = 0; j < 50; ++j)
                        spawn_particle(creatures[i].x + (rand()%TILE_WIDTH), creatures[i].y + (rand()%(TILE_HEIGHT/2)), rand()%2 + 1,3,0,c[rand()%5]); 

                    creatures[num_creatures - 1].age = 0; // set age to zero (newborn)

                    creatures[i].pregnant = FALSE;
                }
            }

            // handle aging
            ++creatures[i].age_counter;
            if(creatures[i].age_counter >= 60)
            {
                // creature is one second older
                creatures[i].age_counter = 0;

                ++creatures[i].age;
                if(creatures[i].age == creatures[i].adult_age)
                {
                    // throw a party
                    int c[5] = { 6,9,11,13,14};

                    for(int j = 0; j < 50; ++j)
                        spawn_particle(creatures[i].x + (rand()%TILE_WIDTH), creatures[i].y + (rand()%(TILE_HEIGHT/2)), rand()%2 + 1,3,0,c[rand()%5]); 
                }
            }
        }
        


    }
}

static void draw_creature(int i)
{
        int creature_x = creatures[i].x - camera.x;
        int creature_y = creatures[i].y - camera.y;

        int age_offset = 0;
        if(creatures[i].reproductive && creatures[i].age < creatures[i].adult_age)
            age_offset = 16;

        if (creatures[i].state == CREATURE_STATE_STUNNED)
        {
            // draw red tint
            draw_tile_tinted(creatures[i].x - camera.x, creatures[i].y - camera.y,creatures[i].tileset_name, creatures[i].tile_index + age_offset + creatures[i].dir + creatures[i].anim.frame_order[creatures[i].anim.frame],6);
        }
        else
        {
            draw_tile(creatures[i].x - camera.x, creatures[i].y - camera.y,creatures[i].tileset_name, creatures[i].tile_index + age_offset + creatures[i].dir + creatures[i].anim.frame_order[creatures[i].anim.frame],day_cycle_shade_amount);
        }

}

static void draw_creatures()
{
	for (int i = 0; i < num_creatures; ++i)
	{
        draw_creature(i);
	}

}
