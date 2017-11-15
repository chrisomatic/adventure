#define MAX_CREATURES     100000
#define MAX_CREATURE_LIST 100
#define CREATURE_GRID_X_MAX 32
#define CREATURE_GRID_Y_MAX 32
#define MAX_CREATURE_GRID MAX_CREATURES/CREATURE_GRID_X_MAX

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
    char* board_name;
    int   board_index;
    char* species;
    char* tileset_name;
    int tile_index;
    int xp;
    PhysicalProperties phys;
    int stun_counter;
    int stun_duration;
    int gold_drop_max;
    int aggro_radius;
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
    BOOL birth_recovery;
    int gestation_period;
    int gestation_counter; 
    int mating_radius;
    int grouping_radius;
    int age;
    int max_age;
    int age_counter;
    int adult_age;
    int litter_max;
    int birth_recovery_time;
    int birth_recovery_counter;
    int death_check_counter;
    int grid_index_x;
    int grid_index_y;
} Creature;

typedef struct
{
    Creature* creatures[MAX_CREATURE_GRID];
    int num_creatures;
} CreatureSection;

Creature creatures[MAX_CREATURES];
Creature creature_list[MAX_CREATURE_LIST];

CreatureSection creature_grid[CREATURE_GRID_X_MAX][CREATURE_GRID_Y_MAX];

int num_creatures = 0;
int num_creature_list = 0;

// @TEMP:
int num_pregs = 0;
int num_births = 0;
int num_deaths = 0;
//

float mortality_table[61] = {
    0.0033,
    0.0033,
    0.0033,
    0.0033,
    0.0033,
    0.0033,
    0.0033,
    0.0040,
    0.0040,
    0.0040,
    0.0040,
    0.0040,
    0.0040,
    0.0040,
    0.0044,
    0.0045,
    0.0046,
    0.0047,
    0.0048,
    0.0049,
    0.0040,
    0.0061,
    0.0062,
    0.0063,
    0.0064,
    0.0065,
    0.0066,
    0.0067,
    0.0108,
    0.0109,
    0.0100,
    0.0101,
    0.0102,
    0.0103,
    0.0104,
    0.0145,
    0.0146,
    0.0147,
    0.0148,
    0.0149,
    0.0140,
    0.0141,
    0.0232,
    0.0233,
    0.0234,
    0.0235,
    0.0236,
    0.0237,
    0.0238,
    0.0339,
    0.0330,
    0.0335,
    0.0335,
    0.0335,
    0.0335,
    0.0335,
    0.0828,
    0.0828,
    0.0828,
    0.0828,
    0.1000
};

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
			creature->phys.hp = creature_list[i].phys.hp;
			creature->phys.max_hp = creature_list[i].phys.max_hp;
			creature->xp = creature_list[i].xp;
            creature->phys.speed = creature_list[i].phys.speed;
            creature->gold_drop_max = creature_list[i].gold_drop_max;
            creature->behavior = creature_list[i].behavior;
            creature->untargetable = creature_list[i].untargetable;
            creature->tileset_name = creature_list[i].tileset_name;
            creature->tile_index = creature_list[i].tile_index;
            creature->reproductive = creature_list[i].reproductive;
            creature->species = creature_list[i].species;
            creature->mating_radius = creature_list[i].mating_radius;
            creature->birth_recovery_time = creature_list[i].birth_recovery_time;
            creature->adult_age = creature_list[i].adult_age;
            creature->gestation_period = creature_list[i].gestation_period;
            creature->litter_max = creature_list[i].litter_max;
            creature->max_age = creature_list[i].max_age;

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

    creatures[num_creatures].phys.x = x; 
    creatures[num_creatures].phys.y = y;
    creatures[num_creatures].phys.z = 0.0f;
    creatures[num_creatures].phys.x_vel = 0;
    creatures[num_creatures].phys.y_vel = 0;
    creatures[num_creatures].phys.height = 16.0f;
    creatures[num_creatures].phys.speed = creature.phys.speed;
    creatures[num_creatures].phys.base_speed = creature.phys.speed;
    creatures[num_creatures].dir = DIR_DOWN;
    creatures[num_creatures].state = CREATURE_STATE_NEUTRAL;
    creatures[num_creatures].mode  = CREATURE_MODE_WANDER;
    creatures[num_creatures].name = creature.name;
    creatures[num_creatures].board_name = "Astoria";
    creatures[num_creatures].board_index = get_board_index_by_name(creatures[num_creatures].board_name);
    creatures[num_creatures].tileset_name = creature.tileset_name;
    creatures[num_creatures].tile_index = creature.tile_index;
    creatures[num_creatures].phys.hp = creature.phys.hp;
    creatures[num_creatures].phys.max_hp = creature.phys.max_hp;
    creatures[num_creatures].xp = creature.xp;
    creatures[num_creatures].gold_drop_max = creature.gold_drop_max;
    creatures[num_creatures].aggro_radius = 50;
    creatures[num_creatures].particle_spawn_counter = 0;
    creatures[num_creatures].phys.environmental_hurt_counter = 60;
    creatures[num_creatures].phys.environmental_hurt_max = 60;
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
    creatures[num_creatures].gestation_period = creature.gestation_period;
    creatures[num_creatures].gestation_counter = 0;
    creatures[num_creatures].mating_radius = creature.mating_radius;
    creatures[num_creatures].grouping_radius = 50;
    creatures[num_creatures].adult_age = creature.adult_age;
    creatures[num_creatures].age = rand()%(creatures[num_creatures].adult_age*2); // half babies, half adults
    creatures[num_creatures].age_counter = 0;
    creatures[num_creatures].max_age = creature.max_age;
	creatures[num_creatures].reproductive = creature.reproductive;
	creatures[num_creatures].species = creature.species;
	creatures[num_creatures].pregnant = FALSE;
    creatures[num_creatures].litter_max = creature.litter_max;
    creatures[num_creatures].birth_recovery_time = creature.birth_recovery_time;
    creatures[num_creatures].birth_recovery_counter = 0;
    creatures[num_creatures].birth_recovery = FALSE;
    creatures[num_creatures].death_check_counter = rand() % DAY_CYCLE_COUNTER_MAX;

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

    for (int i = 0; i < 150; ++i)
    {
        int creature_x, creature_y;
        int test_collision_1, test_collision_2, test_collision_3, test_collision_4;
        BOOL collision = TRUE;

        int board_index = get_board_index_by_name("Astoria");

        if(board_index < 0)
            return;
        
        // make sure not to spawn creature in a solid object
        while(collision)
        {
            creature_x = rand() % (TILE_WIDTH*(BOARD_TILE_WIDTH -1));
            creature_y = rand() % (TILE_HEIGHT*(BOARD_TILE_HEIGHT -1));

            test_collision_1 = board_list[board_index].collision[creature_x/TILE_WIDTH][creature_y/TILE_HEIGHT]; 
            test_collision_2 = board_list[board_index].collision[(creature_x + TILE_WIDTH)/TILE_WIDTH][(creature_y + TILE_HEIGHT)/TILE_HEIGHT];
            test_collision_3 = board_list[board_index].collision[creature_x/TILE_WIDTH][(creature_y+TILE_HEIGHT)/TILE_HEIGHT]; 
            test_collision_4 = board_list[board_index].collision[(creature_x + TILE_WIDTH)/TILE_WIDTH][creature_y/TILE_HEIGHT];
            
            collision = (test_collision_1 == 5 && test_collision_2 == 5 || test_collision_3 == 5 || test_collision_4 == 5);
        }

		if (num_creature_list == 0)
			return;

		int creature_type = rand() % 2;//num_creature_list;
        //spawn_creature(creature_list[creature_type].name,creature_x,creature_y);

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
        spawn_particle(rand()%TILE_WIDTH + creatures[i].phys.x,rand()%TILE_HEIGHT+creatures[i].phys.y,rand() % 4 + 1,3,0,6,creatures[i].board_index);

    int coins_to_spawn = rand() % (creatures[i].gold_drop_max+1);

    int gold_coins = coins_to_spawn / 100; coins_to_spawn -= (gold_coins*100);
    int silver_coins = coins_to_spawn / 10; coins_to_spawn -= (silver_coins*10);
    int bronze_coins = coins_to_spawn / 1; coins_to_spawn -= bronze_coins;

    for(int c = 0; c < bronze_coins; ++c)
        spawn_coin(creatures[i].phys.x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),creatures[i].phys.y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)),2,0,0,3.0f, COIN_BRONZE, creatures[i].board_index);
    for(int c = 0; c < silver_coins; ++c)
        spawn_coin(creatures[i].phys.x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),creatures[i].phys.y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)),2,0,0,2.5f, COIN_SILVER, creatures[i].board_index);
    for(int c = 0; c < gold_coins; ++c)
        spawn_coin(creatures[i].phys.x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),creatures[i].phys.y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)),2,0,0,2.0f, COIN_GOLD, creatures[i].board_index);

    // drop item(s)
    int item_percent = rand() % 100 + 1;

    
    if(item_percent >= 1 && item_percent <= 20)
    {
        spawn_item("Meat",creatures[i].phys.x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),creatures[i].phys.y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)), creatures[i].board_index);
    }
    else if(item_percent >= 21 && item_percent <= 40)
    {
        spawn_item("Mana Vial",creatures[i].phys.x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),creatures[i].phys.y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)), creatures[i].board_index);
    }
    else if(item_percent >= 41 && item_percent <= 42)
    {
        spawn_item("Knife",creatures[i].phys.x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),creatures[i].phys.y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)), creatures[i].board_index);
    }
    else if(item_percent >= 43 && item_percent <= 44)
    {
        spawn_item("Sword",creatures[i].phys.x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),creatures[i].phys.y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)), creatures[i].board_index);
    }
    else if(item_percent >= 45 && item_percent <= 46)
    {
        spawn_item("Axe",creatures[i].phys.x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),creatures[i].phys.y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)), creatures[i].board_index);
    }
    else if(item_percent >= 47 && item_percent <= 48)
    {
        spawn_item("Bow",creatures[i].phys.x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),creatures[i].phys.y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)), creatures[i].board_index);
    }
    else if(item_percent >= 49 && item_percent <= 50)
    {
        spawn_item("Staff",creatures[i].phys.x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),creatures[i].phys.y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)), creatures[i].board_index);
    }
    else if(item_percent >= 51 && item_percent <= 59)
    {
        spawn_item("Helm",creatures[i].phys.x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),creatures[i].phys.y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)), creatures[i].board_index);
    }
    else if(item_percent >= 60 && item_percent <= 69)
    {
        spawn_item("Cloak",creatures[i].phys.x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),creatures[i].phys.y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)), creatures[i].board_index);
    }
    else if(item_percent >= 70 && item_percent <= 79)
    {
        spawn_item("Red Cloak",creatures[i].phys.x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),creatures[i].phys.y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)), creatures[i].board_index);
    }

    remove_creature(i);

    ++num_deaths;
}

static void update_creatures()
{
    
    // init creature grid
    for(int i = 0; i < CREATURE_GRID_X_MAX; ++i)
    {
        for(int j = 0; j < CREATURE_GRID_Y_MAX; ++j)
        {
            creature_grid[i][j].num_creatures = 0;
        }
    }

    for(int i = num_creatures -1; i >= 0;--i)
    {
        // assign creatures to creature grids for optimizing creature-creature interactions
        int grid_x = floor(CREATURE_GRID_X_MAX*(creatures[i].phys.x/(BOARD_TILE_WIDTH*TILE_WIDTH)));
        int grid_y = floor(CREATURE_GRID_Y_MAX*(creatures[i].phys.y/(BOARD_TILE_HEIGHT*TILE_HEIGHT)));
        
        creature_grid[grid_x][grid_y].creatures[creature_grid[grid_x][grid_y].num_creatures++] = &creatures[i];
        
        creatures[i].grid_index_x = grid_x;
        creatures[i].grid_index_y = grid_y;
        //

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
                if(get_distance(player.phys.x + TILE_WIDTH/2,player.phys.y + TILE_HEIGHT/2,creatures[i].phys.x + TILE_WIDTH/2,creatures[i].phys.y+TILE_HEIGHT/2) <= creatures[i].aggro_radius)
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
                            creatures[i].phys.x_vel = +0;
                            creatures[i].phys.y_vel = +0;
                        }
                }
                
                // update creature position
                creatures[i].phys.x += creatures[i].phys.x_vel*creatures[i].phys.speed;
                creatures[i].phys.y += creatures[i].phys.y_vel*creatures[i].phys.speed;

                if(creatures[i].phys.x_vel != 0 || creatures[i].phys.y_vel != 0)
                {
                    handle_terrain_collision(creatures[i].board_index, &creatures[i].phys);

                    // keep creature in board boundaries
                    if(creatures[i].phys.x < 0) creatures[i].phys.x = 0;
                    if(creatures[i].phys.y < 0) creatures[i].phys.y = 0;
                    if(creatures[i].phys.x >TILE_WIDTH*(BOARD_TILE_WIDTH-1)) creatures[i].phys.x = TILE_WIDTH*(BOARD_TILE_WIDTH-1);
                    if(creatures[i].phys.y >TILE_HEIGHT*(BOARD_TILE_HEIGHT-1)) creatures[i].phys.y = TILE_HEIGHT*(BOARD_TILE_HEIGHT-1);

                    if(creatures[i].mode == CREATURE_MODE_PURSUE)
                    {
                        // update particles
                        creatures[i].particle_spawn_counter++;
                        if(creatures[i].particle_spawn_counter >= 30)
                        {
                            creatures[i].particle_spawn_counter = 0;
                            spawn_particle(rand() % TILE_WIDTH + creatures[i].phys.x,creatures[i].phys.y,1,2,CHAR_HEART,12,creatures[i].board_index);
                            spawn_particle(rand() % TILE_WIDTH + creatures[i].phys.x,creatures[i].phys.y,1,2,CHAR_HEART,12,creatures[i].board_index);
                            spawn_particle(rand() % TILE_WIDTH + creatures[i].phys.x,creatures[i].phys.y,1,2,CHAR_HEART,12,creatures[i].board_index);
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
                                    creatures[i].phys.x_vel = +0;
                                    creatures[i].phys.y_vel = +0;
                                    break;
                                case 1: 
                                    creatures[i].dir = DIR_UP;
                                    creatures[i].phys.x_vel = +0;
                                    creatures[i].phys.y_vel = -1;
                                    break;
                                case 2: 
                                    creatures[i].dir = DIR_DOWN;
                                    creatures[i].phys.x_vel = +0;
                                    creatures[i].phys.y_vel = +1;
                                    break;
                                case 3: 
                                    creatures[i].dir = DIR_LEFT;
                                    creatures[i].phys.x_vel = -1;
                                    creatures[i].phys.y_vel = +0;
                                    break;
                                case 4: 
                                    creatures[i].dir = DIR_RIGHT;
                                    creatures[i].phys.x_vel = +1;
                                    creatures[i].phys.y_vel = +0;
                                    break;
                                case 5: 
                                    creatures[i].dir = DIR_UP;
                                    creatures[i].phys.x_vel = -1;
                                    creatures[i].phys.y_vel = -1;
                                    break;
                                case 6: 
                                    creatures[i].dir = DIR_UP;
                                    creatures[i].phys.x_vel = +1;
                                    creatures[i].phys.y_vel = -1;
                                    break;
                                case 7: 
                                    creatures[i].dir = DIR_DOWN;
                                    creatures[i].phys.x_vel = -1;
                                    creatures[i].phys.y_vel = +1;
                                    break;
                                case 8: 
                                    creatures[i].dir = DIR_DOWN;
                                    creatures[i].phys.x_vel = +1;
                                    creatures[i].phys.y_vel = +1;
                                    break;
                            }

                            break;

                        case CREATURE_MODE_PURSUE:
                            
                            creatures[i].action_counter_max  = 0;
                            creatures[i].action_duration_counter_max = 20; 

                            float diff_x = creatures[i].phys.x - player.phys.x;
                            float diff_y = creatures[i].phys.y - player.phys.y;

                            float angle  = atan(diff_y/diff_x);
                            double EIGHTH_PI = PI/8.0;

                            if(angle >= -4*EIGHTH_PI && angle < -3*EIGHTH_PI)
                            {
                                if(diff_y > 0)
                                {
                                    // UP 
                                    creatures[i].dir = DIR_UP;
                                    creatures[i].phys.x_vel = +0;
                                    creatures[i].phys.y_vel = -1;
                                }
                                else
                                {
                                    // DOWN
                                    creatures[i].dir = DIR_DOWN;
                                    creatures[i].phys.x_vel = +0;
                                    creatures[i].phys.y_vel = +1;
                                }
                            }
                            else if(angle >= -3*EIGHTH_PI && angle < -1*EIGHTH_PI)
                            {
                                if(diff_x > 0)
                                {
                                    // DOWN LEFT
                                    creatures[i].dir = DIR_DOWN;
                                    creatures[i].phys.x_vel = -1;
                                    creatures[i].phys.y_vel = +1;
                                }
                                else
                                {
                                    // UP RIGHT
                                    creatures[i].dir = DIR_UP;
                                    creatures[i].phys.x_vel = +1;
                                    creatures[i].phys.y_vel = -1;
                                }
                            }
                            else if(angle >= -1*EIGHTH_PI && angle < +1*EIGHTH_PI)
                            {
                                if(diff_x > 0)
                                {
                                    // LEFT
                                    creatures[i].dir = DIR_LEFT;
                                    creatures[i].phys.x_vel = -1;
                                    creatures[i].phys.y_vel = +0;
                                }
                                else
                                {
                                    // RIGHT
                                    creatures[i].dir = DIR_RIGHT;
                                    creatures[i].phys.x_vel = +1;
                                    creatures[i].phys.y_vel = +0;
                                }
                            }
                            else if(angle >= +1*EIGHTH_PI && angle < +3*EIGHTH_PI)
                            {
                                if(diff_x > 0)
                                {
                                    // UP LEFT
                                    creatures[i].dir = DIR_UP;
                                    creatures[i].phys.x_vel = -1;
                                    creatures[i].phys.y_vel = -1;
                                }
                                else
                                {
                                    // DOWN RIGHT
                                    creatures[i].dir = DIR_DOWN;
                                    creatures[i].phys.x_vel = +1;
                                    creatures[i].phys.y_vel = +1;
                                }
                            }
                            else if(angle <= +4*EIGHTH_PI && angle >= +3*EIGHTH_PI)
                            {
                                if(diff_y > 0)
                                {
                                    // UP 
                                    creatures[i].dir = DIR_UP;
                                    creatures[i].phys.x_vel = +0;
                                    creatures[i].phys.y_vel = -1;
                                }
                                else
                                {
                                    // DOWN
                                    creatures[i].dir = DIR_DOWN;
                                    creatures[i].phys.x_vel = +0;
                                    creatures[i].phys.y_vel = +1;
                                }
                            }
                        break;
                    }

                }
            }
            
            // handle creature animation
            if(creatures[i].phys.x_vel != 0 || creatures[i].phys.y_vel != 0)
            {
                creatures[i].anim.counter++;

                if(creatures[i].anim.counter >= 10/creatures[i].phys.speed)
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
                        spawn_particle(creatures[i].phys.x + (rand()%TILE_WIDTH), creatures[i].phys.y + (rand()%(TILE_HEIGHT/2)), rand()%2 + 1,3,0,c[rand()%5],creatures[i].board_index);
                }
                
                // @TEMP: Indicate male reproductive creatures
                if(creatures[i].reproductive)
                {
                    if(creatures[i].gender == MALE)
                    {
                        spawn_particle(creatures[i].phys.x + (rand()%TILE_WIDTH), creatures[i].phys.y + (rand()%(TILE_HEIGHT/2)),1,2,CHAR_MALE_SYMBOL,8,creatures[i].board_index);
                    }
                }

                // handle creature mating
                if(creatures[i].reproductive && creatures[i].gender == MALE && creatures[i].age >= creatures[i].adult_age)
                {
                    for(int grid_offset_x = -1; grid_offset_x <= +1; ++grid_offset_x)
                    {
                        int x_bound = creatures[i].grid_index_x + grid_offset_x;
                        if(x_bound < 0 || x_bound >= CREATURE_GRID_X_MAX) continue;

                        for(int grid_offset_y = -1; grid_offset_y <= +1; ++grid_offset_y)
                        {
                            int y_bound = creatures[i].grid_index_y + grid_offset_y;
                            if(y_bound < 0 || y_bound >= CREATURE_GRID_X_MAX) continue;

                            for(int j = 0; j < creature_grid[x_bound][y_bound].num_creatures; ++j)
                            {
                                Creature* creature_j = creature_grid[x_bound][y_bound].creatures[j];
                                
                                // are creatures opposite gender?
                                if(creature_j->gender == !creatures[i].gender)
                                {
                                    // is creature reproductive and an adult and not currently pregnant?
                                    if(creature_j->reproductive && creature_j->age >= creature_j->adult_age && !creature_j->pregnant)
                                    {
                                        // is creature not recoverying from a pregnancy?
                                        if(!creature_j->birth_recovery)
                                        {
                                            // are creatures the same species?
                                            if(strcmp(creature_j->species, creatures[i].species) == 0)
                                            {
                                                // look at x distance
                                                if(abs(creatures[i].phys.x - creature_j->phys.x) <= creatures[i].mating_radius)
                                                {
                                                    // look at y distance
                                                    if(abs(creatures[i].phys.y - creature_j->phys.y) <= creatures[i].mating_radius)
                                                    {
                                                        // get direct distance
                                                        double distance = get_distance(creatures[i].phys.x,creatures[i].phys.y,creature_j->phys.x, creature_j->phys.y);

                                                        if(distance <= creatures[i].mating_radius)
                                                        {
                                                            // spawn flames
                                                            for(int k = 0; k < 10; ++k)
                                                                spawn_particle(creature_j->phys.x + (rand()%TILE_WIDTH), creature_j->phys.y + (rand()%(TILE_HEIGHT/2)),1,3,CHAR_FLAME,9, creature_j->board_index);

                                                            creature_j->pregnant = TRUE;
                                                            creature_j->phys.base_speed /= 2.0f; // reduce speed of pregnant creature
                                                            ++num_pregs;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    } 
                                }
                            }
                        }
                    }
                    /*
                    for(int j = 0; j < num_creatures; ++j)
                    {
                        // are creatures opposite gender?
                        if(creatures[j].gender == !creatures[i].gender)
                        {
                            // is creature reproductive and an adult and not currently pregnant?
                            if(creatures[j].reproductive && creatures[j].age >= creatures[j].adult_age && !creatures[j].pregnant)
                            {
                                // is creature not recoverying from a pregnancy?
                                if(!creatures[j].birth_recovery)
                                {
                                    // are creatures the same species?
                                    if(strcmp(creatures[j].species, creatures[i].species) == 0)
                                    {
                                        // look at x distance
                                        if(abs(creatures[i].phys.x - creatures[j].phys.x) <= creatures[i].mating_radius)
                                        {
                                            // look at y distance
                                            if(abs(creatures[i].phys.y - creatures[j].phys.y) <= creatures[i].mating_radius)
                                            {
                                                // get direct distance
                                                double distance = get_distance(creatures[i].phys.x,creatures[i].phys.y,creatures[j].phys.x, creatures[j].phys.y);

                                                if(distance <= creatures[i].mating_radius)
                                                {
                                                    // spawn flames
                                                    for(int k = 0; k < 10; ++k)
                                                        spawn_particle(creatures[j].phys.x + (rand()%TILE_WIDTH), creatures[j].phys.y + (rand()%(TILE_HEIGHT/2)),1,3,CHAR_FLAME,9, creatures[i].board_index);

                                                    creatures[j].pregnant = TRUE;
                                                    creatures[j].phys.base_speed /= 2.0f; // reduce speed of pregnant creature
                                                    ++num_pregs;
                                                }
                                            }
                                        }
                                    }
                                }
                            } 
                        }
                    }
                    */
                }

                // handle pregnancies
                if(creatures[i].pregnant)
                {
                    spawn_particle(creatures[i].phys.x + (rand()%TILE_WIDTH), creatures[i].phys.y + (rand()%(TILE_HEIGHT/2)),1,3,CHAR_HEART,6, creatures[i].board_index); 

                    ++creatures[i].gestation_counter;
                    if(creatures[i].gestation_counter >= creatures[i].gestation_period)
                    {
                        creatures[i].gestation_counter = 0;

                        // have a baby
                        int num_babies = (rand() % creatures[i].litter_max) + 1;

                        for(int j = 0; j < num_babies; ++j)
                        {
                            spawn_creature(creatures[i].name,creatures[i].phys.x,creatures[i].phys.y);
                            creatures[num_creatures - 1].age = 0; // set age to zero (newborn)
                            ++num_births;
                        }

                        // have a party
                        int c[5] = { 6,9,11,13,14 };

                        for(int j = 0; j < 50; ++j)
                            spawn_particle(creatures[i].phys.x + (rand()%TILE_WIDTH), creatures[i].phys.y + (rand()%(TILE_HEIGHT/2)), rand()%2 + 1,3,0,c[rand()%5], creatures[i].board_index);

                        creatures[i].pregnant = FALSE;
                        creatures[i].phys.base_speed *= 2.0f; // return creature back to normal speed
                        creatures[i].birth_recovery = TRUE;
                        --num_pregs;
                    }
                }
                else if(creatures[i].birth_recovery)
                {
                    ++creatures[i].birth_recovery_counter;
                    if(creatures[i].birth_recovery_counter >= creatures[i].birth_recovery_time)
                    {
                        creatures[i].birth_recovery_counter = 0;
                        creatures[i].birth_recovery = FALSE;
                    }
                }
                
                // handle death
                ++creatures[i].death_check_counter;
                if(creatures[i].death_check_counter >= SECONDS_PER_DAY)
                {
                    // one day since last death check.
                    creatures[i].death_check_counter = 0;

                    int death_num = rand() % 10000;
                    int life_in_days = min(60,floor(creatures[i].age / SECONDS_PER_DAY));

                    // min(max((rats/200)^2,1),maxq_mult) * base_rate
                    float num_rat_rate = num_creatures/200.0f;
                    float chance_of_death = min(max(num_rat_rate*num_rat_rate,1),25) * mortality_table[life_in_days];

                    if(death_num >= (10000 - (10000.0f*chance_of_death)))
                    {
                        // creature dies.
                        creature_death(i);
                    }
                }
            }
        }
    }
}

static void draw_creature(int i)
{
    if(creatures[i].board_index != current_board_index)
        return;

    int creature_x = creatures[i].phys.x - camera.x;
    int creature_y = creatures[i].phys.y - camera.y;

    int age_offset = 0;
    if(creatures[i].reproductive && creatures[i].age < creatures[i].adult_age)
        age_offset = 16;

    if (creatures[i].state == CREATURE_STATE_STUNNED)
    {
        // draw red tint
        draw_tile_tinted(creatures[i].phys.x - camera.x, creatures[i].phys.y - camera.y,creatures[i].tileset_name, creatures[i].tile_index + age_offset + creatures[i].dir + creatures[i].anim.frame_order[creatures[i].anim.frame],6);
    }
    else
    {
        draw_tile(creatures[i].phys.x - camera.x, creatures[i].phys.y - camera.y,creatures[i].tileset_name, creatures[i].tile_index + age_offset + creatures[i].dir + creatures[i].anim.frame_order[creatures[i].anim.frame],day_cycle_shade_amount);
    }

}
