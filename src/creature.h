#pragma once


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

static int get_creature_index_by_name(const char* name)
{
    for(int i = 0; i < num_creatures; ++i)
    {
		if (creatures[i].name == NULL)
			continue;

        if(strcmp(creatures[i].name, name) == 0)
        {
            return i;
        }
    }

    return -1;

}
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
            creature->phys.width = creature_list[i].phys.width;
            creature->phys.length = creature_list[i].phys.length;
            creature->phys.height = creature_list[i].phys.height;
            creature->gold_drop_max = creature_list[i].gold_drop_max;
            //creature->behavior = creature_list[i].behavior;

			//for (int cs = 0; cs < NUM_STATES; ++cs) {
			//	creature->behaviors[cs] = creature_list[i].behaviors[cs];
			//}
			creature->behaviors[CREATURE_STATE_NORMAL] = creature_list[i].behaviors[CREATURE_STATE_NORMAL];
			creature->behaviors[CREATURE_STATE_HIT] = creature_list[i].behaviors[CREATURE_STATE_HIT];
			creature->behaviors[CREATURE_STATE_ANTAGONIZED] = creature_list[i].behaviors[CREATURE_STATE_ANTAGONIZED];
			creature->behaviors[CREATURE_STATE_NEAR_PLAYER] = creature_list[i].behaviors[CREATURE_STATE_NEAR_PLAYER];
			creature->behaviors[CREATURE_STATE_LOW_HEALTH_NEAR] = creature_list[i].behaviors[CREATURE_STATE_LOW_HEALTH_NEAR];
			creature->behaviors[CREATURE_STATE_DEAD] = creature_list[i].behaviors[CREATURE_STATE_DEAD];


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
            creature->phys.x_offset = creature_list[i].phys.x_offset;
            creature->phys.y_offset = creature_list[i].phys.y_offset;
            creature->npc_props.is_npc = creature_list[i].npc_props.is_npc;
            creature->npc_props.is_vendor = creature_list[i].npc_props.is_vendor;
            creature->npc_props.num_dialogue = creature_list[i].npc_props.num_dialogue;
            creature->npc_props.dialogue[0] = creature_list[i].npc_props.dialogue[0];
            creature->npc_props.dialogue[1] = creature_list[i].npc_props.dialogue[1];
            creature->npc_props.dialogue[2] = creature_list[i].npc_props.dialogue[2];
            creature->npc_props.dialogue[3] = creature_list[i].npc_props.dialogue[3];
            creature->npc_props.dialogue[4] = creature_list[i].npc_props.dialogue[4];
            creature->npc_props.dialogue[5] = creature_list[i].npc_props.dialogue[5]; 
            creature->npc_props.dialogue[6] = creature_list[i].npc_props.dialogue[6];
            creature->npc_props.dialogue[7] = creature_list[i].npc_props.dialogue[7];
            creature->npc_props.dialogue[8] = creature_list[i].npc_props.dialogue[8];
            creature->npc_props.dialogue[9] = creature_list[i].npc_props.dialogue[9];

            return TRUE;
        }
    }

	return FALSE;
}


static BOOL spawn_creature(const char* creature_name,const char* board_name,float x, float y)
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
    creatures[num_creatures].phys.width = creature.phys.width;
    creatures[num_creatures].phys.length = creature.phys.length;
    creatures[num_creatures].phys.height = creature.phys.height;
    creatures[num_creatures].dir = DIR_DOWN;

    creatures[num_creatures].state = CREATURE_STATE_NORMAL;
	creatures[num_creatures].behaviors[CREATURE_STATE_NORMAL] = creature.behaviors[CREATURE_STATE_NORMAL];
	creatures[num_creatures].behaviors[CREATURE_STATE_HIT] = creature.behaviors[CREATURE_STATE_HIT];
	creatures[num_creatures].behaviors[CREATURE_STATE_ANTAGONIZED] = creature.behaviors[CREATURE_STATE_ANTAGONIZED];
	creatures[num_creatures].behaviors[CREATURE_STATE_NEAR_PLAYER] = creature.behaviors[CREATURE_STATE_NEAR_PLAYER];
	creatures[num_creatures].behaviors[CREATURE_STATE_LOW_HEALTH_NEAR] = creature.behaviors[CREATURE_STATE_LOW_HEALTH_NEAR];
	creatures[num_creatures].behaviors[CREATURE_STATE_DEAD] = creature.behaviors[CREATURE_STATE_DEAD];
	creatures[num_creatures].action_duration_counter = 0;
	creatures[num_creatures].action_duration_counter_max = 0;
	creatures[num_creatures].behavior_duration_counter = 0;
	creatures[num_creatures].behavior_duration_counter_max = 0;
	creatures[num_creatures].break_state = TRUE;
	creatures[num_creatures].hit = FALSE;

    creatures[num_creatures].name = creature.name;
    creatures[num_creatures].board_name = board_name;
    creatures[num_creatures].board_index = get_board_index_by_name(creatures[num_creatures].board_name);
    creatures[num_creatures].tileset_name = creature.tileset_name;
    creatures[num_creatures].tile_index = creature.tile_index;
    creatures[num_creatures].phys.hp = creature.phys.hp;
    creatures[num_creatures].phys.max_hp = creature.phys.max_hp;
    creatures[num_creatures].xp = creature.xp;
    creatures[num_creatures].gold_drop_max = creature.gold_drop_max;
    creatures[num_creatures].aggro_radius = 200;
    creatures[num_creatures].particle_spawn_counter = 0;
    creatures[num_creatures].phys.environmental_hurt_counter = 60;
    creatures[num_creatures].phys.environmental_hurt_max = 60;
    creatures[num_creatures].stun_counter = 0;
    creatures[num_creatures].stun_duration = 20;
    creatures[num_creatures].untargetable = creature.untargetable;
    creatures[num_creatures].gender = rand() % 2;
    creatures[num_creatures].action_counter_max = 60;
    //creatures[num_creatures].action_counter = rand() % creatures[num_creatures].action_counter_max;
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
	creatures[num_creatures].attacking = FALSE;
	creatures[num_creatures].stunned = FALSE;
	creatures[num_creatures].deaggress = FALSE;
	creatures[num_creatures].deaggression_duration = 10*TARGET_FPS;
	creatures[num_creatures].deaggression_counter = 0;
    creatures[num_creatures].attack_angle = 0.0f;
    creatures[num_creatures].litter_max = creature.litter_max;
    creatures[num_creatures].birth_recovery_time = creature.birth_recovery_time;
    creatures[num_creatures].birth_recovery_counter = 0;
    creatures[num_creatures].birth_recovery = FALSE;
    creatures[num_creatures].death_check_counter = rand() % DAY_CYCLE_COUNTER_MAX;
    creatures[num_creatures].phys.x_offset = creature.phys.x_offset;
    creatures[num_creatures].phys.y_offset = creature.phys.y_offset;
    creatures[num_creatures].npc_props.is_npc = creature.npc_props.is_npc;
    creatures[num_creatures].npc_props.is_vendor = creature.npc_props.is_vendor;
    creatures[num_creatures].npc_props.num_dialogue = creature.npc_props.num_dialogue;
    creatures[num_creatures].npc_props.dialogue[0] = creature.npc_props.dialogue[0];
    creatures[num_creatures].npc_props.dialogue[1] = creature.npc_props.dialogue[1];
    creatures[num_creatures].npc_props.dialogue[2] = creature.npc_props.dialogue[2];
    creatures[num_creatures].npc_props.dialogue[3] = creature.npc_props.dialogue[3];
    creatures[num_creatures].npc_props.dialogue[4] = creature.npc_props.dialogue[4];
    creatures[num_creatures].npc_props.dialogue[5] = creature.npc_props.dialogue[5]; 
    creatures[num_creatures].npc_props.dialogue[6] = creature.npc_props.dialogue[6];
    creatures[num_creatures].npc_props.dialogue[7] = creature.npc_props.dialogue[7];
    creatures[num_creatures].npc_props.dialogue[8] = creature.npc_props.dialogue[8];
    creatures[num_creatures].npc_props.dialogue[9] = creature.npc_props.dialogue[9];
    creatures[num_creatures].npc_props.talking = FALSE;
    creatures[num_creatures].npc_props.vendor_credit = 0;
    creatures[num_creatures].npc_props.talk_radius = 20;
    creatures[num_creatures].npc_props.distance_from_player = -1.0f;

    if(strcmp(creature_name,"Orc") == 0)
        get_item_by_name("Axe",&creatures[num_creatures].weapon);
    else if(strcmp(creature_name,"Bat") == 0)
        get_item_by_name("Poison Spit",&creatures[num_creatures].weapon);
    else
        get_item_by_name("Claw",&creatures[num_creatures].weapon);

	if (creatures[num_creatures].reproductive && creatures[num_creatures].gender == FEMALE && creatures[num_creatures].age >= creatures[num_creatures].adult_age)
    {
        int chance_of_preg = rand() % 10;

        if(chance_of_preg == 0)
        {
            creatures[num_creatures].pregnant = TRUE;
            ++num_pregs;
        }
    }

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
    if (num_creature_list == 0)
        return;

    num_creatures = 0;
    num_npcs = 0;

    for(int i = 0; i < num_zones; ++i)
    {
        int creatures_in_zone = zone_list[i].num_creatures;

        for(int j = 0; j < creatures_in_zone; ++j)
        {
            int creature_type_index = rand() % zone_list[i].num_creature_types;
            int board_index = get_board_index_by_name(zone_list[i].board_name);

            int creature_x;
            int creature_y;

            int test_collision_1, test_collision_2, test_collision_3, test_collision_4;
            BOOL collision = TRUE;
            
            // make sure not to spawn creature in a solid object
            while(collision)
            {
                creature_x = (rand() % (int)zone_list[i].w) + zone_list[i].x;
                creature_y = (rand() % (int)zone_list[i].h) + zone_list[i].y;

                test_collision_1 = board_list[board_index].collision[creature_x/TILE_WIDTH][creature_y/TILE_HEIGHT]; 
                test_collision_2 = board_list[board_index].collision[creature_x/TILE_WIDTH][creature_y/TILE_HEIGHT];
                test_collision_3 = board_list[board_index].collision[creature_x/TILE_WIDTH][creature_y/TILE_HEIGHT];
                test_collision_4 = board_list[board_index].collision[creature_x/TILE_WIDTH][creature_y/TILE_HEIGHT];
                
                collision = (test_collision_1 == 5 && test_collision_2 == 5 || test_collision_3 == 5 || test_collision_4 == 5);
            }

            spawn_creature(zone_list[i].creature_type_names[creature_type_index],zone_list[i].board_name,creature_x,creature_y);

            // set zone for creature
            get_zone_by_name(zone_list[i].name,&creatures[num_creatures-1].zone);

            if(creatures[num_creatures-1].npc_props.is_npc)
            {
                npc_creature_indices[num_npcs++] = num_creatures-1;
            }
        }
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
    int item_index = rand() % (2*num_item_list);

    if(item_index < num_item_list)
    {
        spawn_item(item_list[item_index].name,creatures[i].phys.x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),creatures[i].phys.y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)), creatures[i].board_index);
    }

    if(creatures[i].pregnant)
        --num_pregs;

    remove_creature(i);

    ++num_deaths;


}

static void action_pursue_flee(int creature_index, int x, int y ,BOOL flee)
{

    float diff_x = (creatures[creature_index].phys.x - x);
    float diff_y = (creatures[creature_index].phys.y - y);

    if(flee) {
        diff_x *= -1;
        diff_y *= -1;
    }

    float angle  = atan(diff_y/diff_x);
    double EIGHTH_PI = PI/8.0;

    if(angle >= -4*EIGHTH_PI && angle < -3*EIGHTH_PI)
    {
        if(diff_y > 0)
        {
            // UP 
            creatures[creature_index].dir = DIR_UP;
            creatures[creature_index].phys.x_vel = +0;
            creatures[creature_index].phys.y_vel = -1;
        }
        else
        {
            // DOWN
            creatures[creature_index].dir = DIR_DOWN;
            creatures[creature_index].phys.x_vel = +0;
            creatures[creature_index].phys.y_vel = +1;
        }
    }
    else if(angle >= -3*EIGHTH_PI && angle < -1*EIGHTH_PI)
    {
        if(diff_x > 0)
        {
            // DOWN LEFT
            creatures[creature_index].dir = DIR_DOWN;
            creatures[creature_index].phys.x_vel = -1;
            creatures[creature_index].phys.y_vel = +1;
        }
        else
        {
            // UP RIGHT
            creatures[creature_index].dir = DIR_UP;
            creatures[creature_index].phys.x_vel = +1;
            creatures[creature_index].phys.y_vel = -1;
        }
    }
    else if(angle >= -1*EIGHTH_PI && angle < +1*EIGHTH_PI)
    {
        if(diff_x > 0)
        {
            // LEFT
            creatures[creature_index].dir = DIR_LEFT;
            creatures[creature_index].phys.x_vel = -1;
            creatures[creature_index].phys.y_vel = +0;
        }
        else
        {
            // RIGHT
            creatures[creature_index].dir = DIR_RIGHT;
            creatures[creature_index].phys.x_vel = +1;
            creatures[creature_index].phys.y_vel = +0;
        }
    }
    else if(angle >= +1*EIGHTH_PI && angle < +3*EIGHTH_PI)
    {
        if(diff_x > 0)
        {
            // UP LEFT
            creatures[creature_index].dir = DIR_UP;
            creatures[creature_index].phys.x_vel = -1;
            creatures[creature_index].phys.y_vel = -1;
        }
        else
        {
            // DOWN RIGHT
            creatures[creature_index].dir = DIR_DOWN;
            creatures[creature_index].phys.x_vel = +1;
            creatures[creature_index].phys.y_vel = +1;
        }
    }
    else if(angle <= +4*EIGHTH_PI && angle >= +3*EIGHTH_PI)
    {
        if(diff_y > 0)
        {
            // UP 
            creatures[creature_index].dir = DIR_UP;
            creatures[creature_index].phys.x_vel = +0;
            creatures[creature_index].phys.y_vel = -1;
        }
        else
        {
            // DOWN
            creatures[creature_index].dir = DIR_DOWN;
            creatures[creature_index].phys.x_vel = +0;
            creatures[creature_index].phys.y_vel = +1;
        }
    }
}

static void action_attack_player(int creature_index)
{
    if (creatures[creature_index].weapon.weapon_props.weapon_type == WEAPON_TYPE_MELEE)
    {
        creatures[creature_index].attack_angle += creatures[creature_index].weapon.weapon_props.attack_speed*(PI / 30.0f);

        // check for collision with player
        float cosa = cos(creatures[creature_index].attack_angle);
        float sina = sin(creatures[creature_index].attack_angle);

        for (int j = 0; j < creatures[creature_index].weapon.weapon_props.attack_range; ++j)
        {
            if ((player.state & PLAYER_STATE_DEAD) == PLAYER_STATE_DEAD)
                return;

            int start_weapon_x = creatures[creature_index].phys.x + TILE_WIDTH / 2;
            int start_weapon_y = creatures[creature_index].phys.y + TILE_HEIGHT / 2;

            float delta_x = +cosa*j;
            float delta_y = -sina*j;

            if (start_weapon_x + delta_x >= player.phys.x && start_weapon_x + delta_x <= player.phys.x + 0.75*TILE_WIDTH)
            {
                if (start_weapon_y + delta_y >= player.phys.y && start_weapon_y + delta_y <= player.phys.y + 0.75*TILE_HEIGHT)
                {
                    int damage = (rand() % (creatures[creature_index].weapon.weapon_props.max_damage - creatures[creature_index].weapon.weapon_props.min_damage + 1)) + creatures[creature_index].weapon.weapon_props.min_damage;

                    // add floating number
                    spawn_floating_number(start_weapon_x + delta_x, start_weapon_y + delta_y, damage, 6, creatures[creature_index].board_index);

                    // player hurt!
                    player.phys.hp -= damage;

                    // check if creature died
                    if (creatures[creature_index].phys.hp <= 0)
                        player_die();
                    else
                        player.state |= PLAYER_STATE_HURT;

                    return;
                }
            }
        }
    }
    else if (creatures[creature_index].weapon.weapon_props.weapon_type == WEAPON_TYPE_RANGED)
    {
        if ((player.state & PLAYER_STATE_DEAD) == PLAYER_STATE_DEAD)
            return;

        float x_diff = creatures[creature_index].phys.x - player.phys.x;
        float y_diff = creatures[creature_index].phys.y - player.phys.y;

        float angle = atan(y_diff / x_diff);


        float x_vel = cos(angle) * 2;
        float y_vel = -sin(angle) * 2;

        if (x_diff > 0)
        {
            x_vel *= -1.0f;
            y_vel *= -1.0f;
        }

        int projectile_index;

        switch (creatures[creature_index].weapon.tile_index)
        {
        case BOW:
        case CROSSBOW:
            projectile_index = ARROW;
            break;
        case STAFF:
            projectile_index = FIREBALL;
            break;
        case POISON_SPIT:
            projectile_index = POISON_SPIT;
            break;
        }

        spawn_projectile(creatures[creature_index].phys.x, creatures[creature_index].phys.y, 5.0f, x_vel, -y_vel, 2.0f, projectile_index, creatures[creature_index].attack_angle, creatures[creature_index].weapon.weapon_props.min_damage, creatures[creature_index].weapon.weapon_props.max_damage, FALSE);
        projectiles[num_projectiles - 1].shot = TRUE;
    }
}
static void action_wander(int creature_index)
{

    int d = rand() % 9;

    switch (d)
    {
    case 0:
        creatures[creature_index].phys.x_vel = +0;
        creatures[creature_index].phys.y_vel = +0;
        break;
    case 1:
        creatures[creature_index].dir = DIR_UP;
        creatures[creature_index].phys.x_vel = +0;
        creatures[creature_index].phys.y_vel = -1;
        break;
    case 2:
        creatures[creature_index].dir = DIR_DOWN;
        creatures[creature_index].phys.x_vel = +0;
        creatures[creature_index].phys.y_vel = +1;
        break;
    case 3:
        creatures[creature_index].dir = DIR_LEFT;
        creatures[creature_index].phys.x_vel = -1;
        creatures[creature_index].phys.y_vel = +0;
        break;
    case 4:
        creatures[creature_index].dir = DIR_RIGHT;
        creatures[creature_index].phys.x_vel = +1;
        creatures[creature_index].phys.y_vel = +0;
        break;
    case 5:
        creatures[creature_index].dir = DIR_UP;
        creatures[creature_index].phys.x_vel = -1;
        creatures[creature_index].phys.y_vel = -1;
        break;
    case 6:
        creatures[creature_index].dir = DIR_UP;
        creatures[creature_index].phys.x_vel = +1;
        creatures[creature_index].phys.y_vel = -1;
        break;
    case 7:
        creatures[creature_index].dir = DIR_DOWN;
        creatures[creature_index].phys.x_vel = -1;
        creatures[creature_index].phys.y_vel = +1;
        break;
    case 8:
        creatures[creature_index].dir = DIR_DOWN;
        creatures[creature_index].phys.x_vel = +1;
        creatures[creature_index].phys.y_vel = +1;
        break;
    }

}

static void update_creatures2()
{
	// init creature grid
	for (int i = 0; i < CREATURE_GRID_X_MAX; ++i)
	{
		for (int j = 0; j < CREATURE_GRID_Y_MAX; ++j)
		{
			creature_grid[i][j].num_creatures = 0;
		}
	}

	float min_distance = 1000.0f;
	int min_index = -1;

	for (int i = num_creatures - 1; i >= 0; --i)
	{
		// assign creatures to creature grids for optimizing creature-creature interactions
		int grid_x = floor(CREATURE_GRID_X_MAX*(creatures[i].phys.x / (BOARD_TILE_WIDTH*TILE_WIDTH)));
		int grid_y = floor(CREATURE_GRID_Y_MAX*(creatures[i].phys.y / (BOARD_TILE_HEIGHT*TILE_HEIGHT)));

		creature_grid[grid_x][grid_y].creatures[creature_grid[grid_x][grid_y].num_creatures++] = &creatures[i];

		creatures[i].grid_index_x = grid_x;
		creatures[i].grid_index_y = grid_y;

		creatures[i].action_duration_counter++;
		creatures[i].behavior_duration_counter++;

		BOOL in_range_of_player = creatures[i].board_index == current_board_index && (player.state & PLAYER_STATE_DEAD) != PLAYER_STATE_DEAD && get_distance(player.phys.x + TILE_WIDTH / 2, player.phys.y + TILE_HEIGHT / 2, creatures[i].phys.x + TILE_WIDTH / 2, creatures[i].phys.y + TILE_HEIGHT / 2) <= creatures[i].aggro_radius;

		BOOL low_health = FALSE;
		if (creatures[i].phys.hp <= 1)
			low_health = TRUE;

		BOOL action_complete = creatures[i].action_duration_counter >= creatures[i].action_duration_counter_max;
		BOOL behavior_complete = creatures[i].behavior_duration_counter >= creatures[i].behavior_duration_counter_max;
		BOOL action_max_complete = creatures[i].action_duration_counter >= creatures[i].action_counter_max;

		if (creatures[i].stunned)
		{
			creatures[i].stun_counter++;
			if (creatures[i].stun_counter >= creatures[i].stun_duration)
			{
				creatures[i].stun_counter = 0;
				creatures[i].stunned = FALSE;
			}
		}

		if (behavior_complete) {
			action_complete = TRUE;
			action_max_complete = TRUE;

	
			if (creatures[i].break_state) {
				if (creatures[i].state == CREATURE_STATE_HIT) {
					creatures[i].state = CREATURE_STATE_ANTAGONIZED;
				}
				else if (creatures[i].state == CREATURE_STATE_ANTAGONIZED) {
					creatures[i].state = CREATURE_STATE_NORMAL;
				}
				else {
					creatures[i].state = CREATURE_STATE_NORMAL;
				}

				creatures[i].break_state = FALSE;
			}

			//determine the creature state
			//you want to set break_state = TRUE if you are checking based off current state
			if (creatures[i].state == CREATURE_STATE_DEAD) {
			}
			else if (creatures[i].state == CREATURE_STATE_HIT) {
				creatures[i].break_state = TRUE;
				//when a creature is hit, they should get stunned and then they will be switched to the antagonized state
				//we might want to create a stunned behavior
			}
			else if (creatures[i].state == CREATURE_STATE_ANTAGONIZED) {
				creatures[i].break_state = TRUE;
				//once the creature is in the antagonization state it will act on its associate behavior, and then it will break the antagonization state once it's done with the behavior
			}
			else if (in_range_of_player && low_health) {
				creatures[i].state = CREATURE_STATE_LOW_HEALTH_NEAR;
			}
			else if (in_range_of_player) {
				creatures[i].state = CREATURE_STATE_NEAR_PLAYER;
			}
			else {
				creatures[i].state = CREATURE_STATE_NORMAL;
			}


			//reset the behavior counter
			creatures[i].behavior_duration_counter = 0;

		}

		if (action_complete && !action_max_complete) {
			creatures[i].phys.x_vel = +0;
			creatures[i].phys.y_vel = +0;

		}


		if (action_max_complete) {

			CreatureBehavior b = creatures[i].behaviors[creatures[i].state];

			switch(b)
			{
			case CREATURE_BEHAVIOR_NOTHING:
				creatures[i].behavior_duration_counter_max = 0;
				creatures[i].action_duration_counter = 0;
				creatures[i].action_duration_counter_max = 0;
				creatures[i].action_counter_max = 0;
				break;
			case CREATURE_BEHAVIOR_WANDER:
				creatures[i].behavior_duration_counter_max = TARGET_FPS;
				creatures[i].action_duration_counter = 0;
				creatures[i].action_duration_counter_max = rand() % 30 + 30 + 1;
				creatures[i].action_counter_max = min(TARGET_FPS, creatures[i].action_duration_counter_max + rand() % 30);
				action_wander(i);
				break;
			case CREATURE_BEHAVIOR_FLEE:
				creatures[i].behavior_duration_counter_max = TARGET_FPS * 15; //set the duration of the behavior (seconds) -- flee for 15 seconds
				creatures[i].action_duration_counter = 0; //reset their action counter
				creatures[i].action_duration_counter_max = 20; //set how long their action is
				creatures[i].action_counter_max = 20; //set how long it will be until their next action

				if (in_range_of_player) {
					action_pursue_flee(i, player.phys.x, player.phys.y, TRUE);
				}
				else {
					action_wander(i);
				}
				break;

			case CREATURE_BEHAVIOR_ATTACK:
				creatures[i].behavior_duration_counter_max = TARGET_FPS * 15; //set the duration of the behavior (seconds) -- attack for 15 seconds
				creatures[i].action_duration_counter = 0; //reset their action counter
				creatures[i].action_duration_counter_max = 20; //set how long their action is
				creatures[i].action_counter_max = 20; //set how long it will be until their next action

				if (in_range_of_player) {
					action_pursue_flee(i, player.phys.x, player.phys.y, FALSE);
                    if (low_health)
                        creatures[i].break_state = TRUE;
					else if (creatures[i].behavior_duration_counter >= creatures[i].behavior_duration_counter_max - (TARGET_FPS + 1))
						creatures[i].behavior_duration_counter = creatures[i].behavior_duration_counter - TARGET_FPS;
                    action_attack_player(i);
				}
				else {
					action_wander(i);
				}
				break;

			case CREATURE_BEHAVIOR_FOLLOW:
				creatures[i].behavior_duration_counter_max = 20; //TARGET_FPS * 1; //set the duration of the behavior (seconds)
				creatures[i].action_duration_counter = 0; //reset their action counter
				creatures[i].action_duration_counter_max = 20; //TARGET_FPS; //set how long their action is
				creatures[i].action_counter_max = 20; //TARGET_FPS; //set how long it will be until their next action

				if (in_range_of_player) {
					action_pursue_flee(i, player.phys.x, player.phys.y, FALSE);
				}
				else {
					action_wander(i);
				}
				break;

			case CREATURE_BEHAVIOR_PATROL:
				break;
			case CREATURE_BEHAVIOR_RPATROL:
				break;
			case CREATURE_BEHAVIOR_CONVERSE:
				break;

			}
		}

		creatures[i].phys.x += creatures[i].phys.x_vel*creatures[i].phys.speed;
		creatures[i].phys.y += creatures[i].phys.y_vel*creatures[i].phys.speed;

		if (creatures[i].phys.x_vel != 0 || creatures[i].phys.y_vel != 0)
		{
			handle_terrain_collision(creatures[i].board_index, &creatures[i].phys);

			if (creatures[i].stunned)
				creatures[i].phys.speed /= 2.0f;

			// keep creature in zone boundaries
			if (creatures[i].phys.x < creatures[i].zone.x)
			{
				creatures[i].phys.x = creatures[i].zone.x;
				creatures[i].phys.x_vel *= -1; // flip creature in other direction
				if (creatures[i].dir == DIR_LEFT) creatures[i].dir = DIR_RIGHT;
				if (creatures[i].dir == DIR_RIGHT) creatures[i].dir = DIR_LEFT;
			}
			if (creatures[i].phys.y < creatures[i].zone.y)
			{
				creatures[i].phys.y = creatures[i].zone.y;
				creatures[i].phys.y_vel *= -1; // flip creature in other direction
				if (creatures[i].dir == DIR_UP) creatures[i].dir = DIR_DOWN;
				if (creatures[i].dir == DIR_DOWN) creatures[i].dir = DIR_UP;
			}
			if (creatures[i].phys.x > creatures[i].zone.x + creatures[i].zone.w)
			{
				creatures[i].phys.x = creatures[i].zone.x + creatures[i].zone.w;
				creatures[i].phys.x_vel *= -1; // flip creature in other direction
				if (creatures[i].dir == DIR_LEFT) creatures[i].dir = DIR_RIGHT;
				if (creatures[i].dir == DIR_RIGHT) creatures[i].dir = DIR_LEFT;
			}
			if (creatures[i].phys.y > creatures[i].zone.y + creatures[i].zone.h)
			{
				creatures[i].phys.y = creatures[i].zone.y + creatures[i].zone.h;
				creatures[i].phys.y_vel *= -1; // flip creature in other direction
				if (creatures[i].dir == DIR_UP) creatures[i].dir = DIR_DOWN;
				if (creatures[i].dir == DIR_DOWN) creatures[i].dir = DIR_UP;
			}

			// keep creature in board boundaries
			if (creatures[i].phys.x < 0) creatures[i].phys.x = 0;
			if (creatures[i].phys.y < 0) creatures[i].phys.y = 0;
			if (creatures[i].phys.x >TILE_WIDTH*(BOARD_TILE_WIDTH - 1)) creatures[i].phys.x = TILE_WIDTH*(BOARD_TILE_WIDTH - 1);
			if (creatures[i].phys.y >TILE_HEIGHT*(BOARD_TILE_HEIGHT - 1)) creatures[i].phys.y = TILE_HEIGHT*(BOARD_TILE_HEIGHT - 1);

			if (creatures[i].behaviors[creatures[i].state] == CREATURE_BEHAVIOR_ATTACK)
			{
				// update particles
				creatures[i].particle_spawn_counter++;
				if (creatures[i].particle_spawn_counter >= 30)
				{
					creatures[i].particle_spawn_counter = 0;

					int num_puncts = rand() % 5 + 3;
					int punctuation;
					for (int j = 0; j < num_puncts; ++j)
					{
						punctuation = rand() % 6 + 33;
						spawn_particle(rand() % TILE_WIDTH + creatures[i].phys.x, creatures[i].phys.y, 1, 1, punctuation, 9, creatures[i].board_index);
					}
				}
			}
			else if (creatures[i].behaviors[creatures[i].state] == CREATURE_BEHAVIOR_FLEE)
			{
				// update particles
				creatures[i].particle_spawn_counter++;
				if (creatures[i].particle_spawn_counter >= 30)
				{
					creatures[i].particle_spawn_counter = 0;

					int num_puncts = rand() % 5 + 3;
					int punctuation;
					for (int j = 0; j < num_puncts; ++j)
					{
						punctuation = 33;
						spawn_particle(rand() % TILE_WIDTH + creatures[i].phys.x, creatures[i].phys.y, 1, 1, punctuation, 9, creatures[i].board_index);
					}
				}
			}
		}


		// handle creature animation
		if (creatures[i].phys.x_vel != 0 || creatures[i].phys.y_vel != 0)
		{
			creatures[i].anim.counter++;

			if (creatures[i].anim.counter >= 10 / creatures[i].phys.speed)
			{
				// cycle_animation
				creatures[i].anim.counter = 0;
				creatures[i].anim.frame += 1;
				if (creatures[i].anim.frame >= creatures[i].anim.num_frames)
					creatures[i].anim.frame = 0;
			}
		}
		else
		{
			// clear animation frame
			creatures[i].anim.counter = 0;
			creatures[i].anim.frame = 0;
		}

		// handle npc stuff
		if (creatures[i].npc_props.is_npc)
		{
			// check if player is near
			float distance = get_distance(player.phys.x, player.phys.y, creatures[i].phys.x, creatures[i].phys.y);

			if (distance <= creatures[i].npc_props.talk_radius)
			{
				BOOL prev_talking = creatures[i].npc_props.talking;

				if (prev_talking == FALSE)
				{
					// randomly select new dialogue
					creatures[i].npc_props.selected_dialogue_num = rand() % creatures[i].npc_props.num_dialogue;
				}

				creatures[i].npc_props.talking = TRUE;
				creatures[i].npc_props.distance_from_player = distance;
				creatures[i].state = CREATURE_STATE_NEUTRAL; // return to neutral state
				creatures[i].phys.x_vel = +0;
				creatures[i].phys.y_vel = +0;
			}
			else
			{
				creatures[i].npc_props.talking = FALSE;
				creatures[i].npc_props.distance_from_player = -1.0f;
			}

			// update min index
			if (creatures[i].npc_props.talking)
			{
				if (creatures[i].npc_props.distance_from_player < min_distance)
				{
					min_distance = creatures[i].npc_props.distance_from_player;
					min_index = i;
				}
			}
		}

		// handle aging
		++creatures[i].age_counter;
		if (creatures[i].age_counter >= 60)
		{
			// creature is one second older
			creatures[i].age_counter = 0;

			++creatures[i].age;
			if (creatures[i].age == creatures[i].adult_age)
			{
				// throw a party
				int c[5] = { 6,9,11,13,14 };

				for (int j = 0; j < 50; ++j)
					spawn_particle(creatures[i].phys.x + (rand() % TILE_WIDTH), creatures[i].phys.y + (rand() % (TILE_HEIGHT / 2)), rand() % 2 + 1, 3, 0, c[rand() % 5], creatures[i].board_index);
			}

			if (creatures[i].reproductive)
			{
				// @TEMP: Indicate male reproductive creatures
				if (creatures[i].gender == MALE)
				{
					spawn_particle(creatures[i].phys.x + (rand() % TILE_WIDTH), creatures[i].phys.y + (rand() % (TILE_HEIGHT / 2)), 1, 2, CHAR_MALE_SYMBOL, 8, creatures[i].board_index);
				}

				// handle death
				++creatures[i].death_check_counter;
				if (creatures[i].death_check_counter >= SECONDS_PER_DAY)
				{
					// one day since last death check.
					creatures[i].death_check_counter = 0;

					int death_num = rand() % 10000;
					int life_in_days = min(60, floor(creatures[i].age / SECONDS_PER_DAY));

					// min(max((rats/200)^2,1),maxq_mult) * base_rate
					float num_rat_rate = num_creatures / 200.0f;
					float chance_of_death = min(max(num_rat_rate*num_rat_rate, 1), 25) * mortality_table[life_in_days];

					if (death_num >= (10000 - (10000.0f*chance_of_death)))
					{
						// creature dies.
						creature_death(i);
					}
				}
			}

			// handle creature mating
			if (creatures[i].reproductive && creatures[i].gender == MALE && creatures[i].age >= creatures[i].adult_age)
			{
				for (int grid_offset_x = -1; grid_offset_x <= +1; ++grid_offset_x)
				{
					int x_bound = creatures[i].grid_index_x + grid_offset_x;
					if (x_bound < 0 || x_bound >= CREATURE_GRID_X_MAX) continue;

					for (int grid_offset_y = -1; grid_offset_y <= +1; ++grid_offset_y)
					{
						int y_bound = creatures[i].grid_index_y + grid_offset_y;
						if (y_bound < 0 || y_bound >= CREATURE_GRID_X_MAX) continue;

						for (int j = 0; j < creature_grid[x_bound][y_bound].num_creatures; ++j)
						{
							Creature* creature_j = creature_grid[x_bound][y_bound].creatures[j];

							// are creatures opposite gender?
							if (creature_j->gender == !creatures[i].gender)
							{
								// is creature reproductive and an adult and not currently pregnant?
								if (creature_j->reproductive && creature_j->age >= creature_j->adult_age && !creature_j->pregnant)
								{
									// is creature not recoverying from a pregnancy?
									if (!creature_j->birth_recovery)
									{
										// are creatures in same zone?
										if (strcmp(creature_j->zone.name, creatures[i].zone.name) == 0)
										{
											// are creatures the same species?
											if (strcmp(creature_j->species, creatures[i].species) == 0)
											{
												// look at x distance
												if (abs(creatures[i].phys.x - creature_j->phys.x) <= creatures[i].mating_radius)
												{
													// look at y distance
													if (abs(creatures[i].phys.y - creature_j->phys.y) <= creatures[i].mating_radius)
													{
														// get direct distance
														double distance = get_distance(creatures[i].phys.x, creatures[i].phys.y, creature_j->phys.x, creature_j->phys.y);

														if (distance <= creatures[i].mating_radius)
														{
															// spawn flames
															for (int k = 0; k < 10; ++k)
																spawn_particle(creature_j->phys.x + (rand() % TILE_WIDTH), creature_j->phys.y + (rand() % (TILE_HEIGHT / 2)), 1, 3, CHAR_FLAME, 9, creature_j->board_index);

															// calculate chance of pregnancy
															int chance_of_preg = rand() % 100;

															if (chance_of_preg < 100) // 100% chance of pregnancy
															{
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
					}
				}
			}

			// handle pregnancies
			if (creatures[i].pregnant)
			{
				spawn_particle(creatures[i].phys.x + (rand() % TILE_WIDTH), creatures[i].phys.y + (rand() % (TILE_HEIGHT / 2)), 1, 3, CHAR_HEART, 6, creatures[i].board_index);

				++creatures[i].gestation_counter;
				if (creatures[i].gestation_counter >= creatures[i].gestation_period)
				{
					creatures[i].gestation_counter = 0;

					// have a baby
					int num_babies = (rand() % creatures[i].litter_max) + 1;

					for (int j = 0; j < num_babies; ++j)
					{
						spawn_creature(creatures[i].name, creatures[i].board_name, creatures[i].phys.x, creatures[i].phys.y);
						creatures[num_creatures - 1].age = 0; // set age to zero (newborn)
						get_zone_by_name(creatures[i].zone.name, &creatures[num_creatures - 1].zone); // set zone
						++num_births;
					}

					// have a party
					int c[5] = { 6,9,11,13,14 };

					for (int j = 0; j < 50; ++j)
						spawn_particle(creatures[i].phys.x + (rand() % TILE_WIDTH), creatures[i].phys.y + (rand() % (TILE_HEIGHT / 2)), rand() % 2 + 1, 3, 0, c[rand() % 5], creatures[i].board_index);

					creatures[i].pregnant = FALSE;
					creatures[i].phys.base_speed *= 2.0f; // return creature back to normal speed
					creatures[i].birth_recovery = TRUE;
					--num_pregs;
				}
			}
			else if (creatures[i].birth_recovery)
			{
				++creatures[i].birth_recovery_counter;
				if (creatures[i].birth_recovery_counter >= creatures[i].birth_recovery_time)
				{
					creatures[i].birth_recovery_counter = 0;
					creatures[i].birth_recovery = FALSE;
				}
			}

		}

        // update npc message
        if (min_index > -1)
        {
            message.name = creatures[min_index].name;
            message.message = creatures[min_index].npc_props.dialogue[creatures[min_index].npc_props.selected_dialogue_num];

            if (creatures[min_index].npc_props.is_vendor)
                message.value = creatures[min_index].npc_props.vendor_credit;
            else
                message.value = -1;

            message.color = 10;
            message.active = TRUE;
        }

	} // end of loop through creatures
}

/*
static void update_creatures()
{
	// init creature grid
	for (int i = 0; i < CREATURE_GRID_X_MAX; ++i)
	{
		for (int j = 0; j < CREATURE_GRID_Y_MAX; ++j)
		{
			creature_grid[i][j].num_creatures = 0;
		}
	}

	float min_distance = 1000.0f;
	int min_index = -1;

	for (int i = num_creatures - 1; i >= 0; --i)
	{
		// assign creatures to creature grids for optimizing creature-creature interactions
		int grid_x = floor(CREATURE_GRID_X_MAX*(creatures[i].phys.x / (BOARD_TILE_WIDTH*TILE_WIDTH)));
		int grid_y = floor(CREATURE_GRID_Y_MAX*(creatures[i].phys.y / (BOARD_TILE_HEIGHT*TILE_HEIGHT)));

		creature_grid[grid_x][grid_y].creatures[creature_grid[grid_x][grid_y].num_creatures++] = &creatures[i];

		creatures[i].grid_index_x = grid_x;
		creatures[i].grid_index_y = grid_y;
		//

		if (creatures[i].stunned)
		{
			creatures[i].stun_counter++;
			if (creatures[i].stun_counter >= creatures[i].stun_duration)
			{
				creatures[i].stun_counter = 0;
				creatures[i].stunned = FALSE;
			}
		}

		if (creatures[i].behavior == CREATURE_BEHAVIOR_AGGRESSIVE)
		{
			// check distance from player
			if (creatures[i].board_index == current_board_index && (player.state & PLAYER_STATE_DEAD) != PLAYER_STATE_DEAD && get_distance(player.phys.x + TILE_WIDTH / 2, player.phys.y + TILE_HEIGHT / 2, creatures[i].phys.x + TILE_WIDTH / 2, creatures[i].phys.y + TILE_HEIGHT / 2) <= creatures[i].aggro_radius)
			{
				if (creatures[i].mode == CREATURE_MODE_WANDER)
				{
					creatures[i].state = CREATURE_STATE_NEUTRAL;
					creatures[i].action_counter = creatures[i].action_counter_max;
				}

				creatures[i].mode = CREATURE_MODE_PURSUE;
			}
			else
			{
				creatures[i].mode = CREATURE_MODE_WANDER;

				if (creatures[i].deaggress)
				{
					++creatures[i].deaggression_counter;
					if (creatures[i].deaggression_counter == creatures[i].deaggression_duration)
					{
						creatures[i].deaggression_counter = 0;
						creatures[i].deaggress = FALSE;
						creatures[i].behavior = CREATURE_BEHAVIOR_PASSIVE;
					}
				}
			}
		}
		else if (creatures[i].behavior == CREATURE_BEHAVIOR_QUAKER)
		{
			// check distance from player
			if (creatures[i].mode == CREATURE_MODE_FLEE && creatures[i].board_index == current_board_index && (player.state & PLAYER_STATE_DEAD) != PLAYER_STATE_DEAD && get_distance(player.phys.x + TILE_WIDTH / 2, player.phys.y + TILE_HEIGHT / 2, creatures[i].phys.x + TILE_WIDTH / 2, creatures[i].phys.y + TILE_HEIGHT / 2) <= creatures[i].aggro_radius)
			{
				//uncomment this to see a funny bug
				//creatures[i].state = CREATURE_STATE_NEUTRAL;

			}
			else
			{
				creatures[i].mode = CREATURE_MODE_WANDER;

				if (creatures[i].deaggress)
				{
					++creatures[i].deaggression_counter;
					if (creatures[i].deaggression_counter == creatures[i].deaggression_duration)
					{
						creatures[i].deaggression_counter = 0;
						creatures[i].deaggress = FALSE;
						creatures[i].phys.speed = creatures[i].phys.speed / 1.5;
					}
				}
			}
		}
		else
		{
			creatures[i].mode = CREATURE_MODE_WANDER;
		}

		// if creature is performing an action, let it play out
		if (creatures[i].state == CREATURE_STATE_ACTING)
		{
			creatures[i].action_duration_counter++;

			if (creatures[i].action_duration_counter >= creatures[i].action_duration_counter_max)
			{
				creatures[i].action_duration_counter = 0;
				creatures[i].state = CREATURE_STATE_NEUTRAL; // return to neutral state

				if (creatures[i].mode != CREATURE_MODE_PURSUE && creatures[i].mode != CREATURE_MODE_FLEE)
				{
					creatures[i].phys.x_vel = +0;
					creatures[i].phys.y_vel = +0;
				}

				if (creatures[i].attacking)
				{
					creatures[i].attacking = FALSE;
					creatures[i].attack_recovery = FALSE;
				}
			}

			// update creature position
			creatures[i].phys.x += creatures[i].phys.x_vel*creatures[i].phys.speed;
			creatures[i].phys.y += creatures[i].phys.y_vel*creatures[i].phys.speed;

			if (creatures[i].phys.x_vel != 0 || creatures[i].phys.y_vel != 0)
			{
				handle_terrain_collision(creatures[i].board_index, &creatures[i].phys);

				if (creatures[i].stunned)
					creatures[i].phys.speed /= 2.0f;

				// keep creature in zone boundaries
				if (creatures[i].phys.x < creatures[i].zone.x)
				{
					creatures[i].phys.x = creatures[i].zone.x;
					creatures[i].phys.x_vel *= -1; // flip creature in other direction
					if (creatures[i].dir == DIR_LEFT) creatures[i].dir = DIR_RIGHT;
					if (creatures[i].dir == DIR_RIGHT) creatures[i].dir = DIR_LEFT;
				}
				if (creatures[i].phys.y < creatures[i].zone.y)
				{
					creatures[i].phys.y = creatures[i].zone.y;
					creatures[i].phys.y_vel *= -1; // flip creature in other direction
					if (creatures[i].dir == DIR_UP) creatures[i].dir = DIR_DOWN;
					if (creatures[i].dir == DIR_DOWN) creatures[i].dir = DIR_UP;
				}
				if (creatures[i].phys.x > creatures[i].zone.x + creatures[i].zone.w)
				{
					creatures[i].phys.x = creatures[i].zone.x + creatures[i].zone.w;
					creatures[i].phys.x_vel *= -1; // flip creature in other direction
					if (creatures[i].dir == DIR_LEFT) creatures[i].dir = DIR_RIGHT;
					if (creatures[i].dir == DIR_RIGHT) creatures[i].dir = DIR_LEFT;
				}
				if (creatures[i].phys.y > creatures[i].zone.y + creatures[i].zone.h)
				{
					creatures[i].phys.y = creatures[i].zone.y + creatures[i].zone.h;
					creatures[i].phys.y_vel *= -1; // flip creature in other direction
					if (creatures[i].dir == DIR_UP) creatures[i].dir = DIR_DOWN;
					if (creatures[i].dir == DIR_DOWN) creatures[i].dir = DIR_UP;
				}

				// keep creature in board boundaries
				if (creatures[i].phys.x < 0) creatures[i].phys.x = 0;
				if (creatures[i].phys.y < 0) creatures[i].phys.y = 0;
				if (creatures[i].phys.x >TILE_WIDTH*(BOARD_TILE_WIDTH - 1)) creatures[i].phys.x = TILE_WIDTH*(BOARD_TILE_WIDTH - 1);
				if (creatures[i].phys.y >TILE_HEIGHT*(BOARD_TILE_HEIGHT - 1)) creatures[i].phys.y = TILE_HEIGHT*(BOARD_TILE_HEIGHT - 1);

				if (creatures[i].mode == CREATURE_MODE_PURSUE)
				{
					// update particles
					creatures[i].particle_spawn_counter++;
					if (creatures[i].particle_spawn_counter >= 30)
					{
						creatures[i].particle_spawn_counter = 0;

						int num_puncts = rand() % 5 + 3;
						int punctuation;
						for (int j = 0; j < num_puncts; ++j)
						{
							punctuation = rand() % 6 + 33;
							spawn_particle(rand() % TILE_WIDTH + creatures[i].phys.x, creatures[i].phys.y, 1, 1, punctuation, 9, creatures[i].board_index);
						}
					}
				}
				else if (creatures[i].mode == CREATURE_MODE_FLEE)
				{
					// update particles
					creatures[i].particle_spawn_counter++;
					if (creatures[i].particle_spawn_counter >= 30)
					{
						creatures[i].particle_spawn_counter = 0;

						int num_puncts = rand() % 5 + 3;
						int punctuation;
						for (int j = 0; j < num_puncts; ++j)
						{
							punctuation = 33;
							spawn_particle(rand() % TILE_WIDTH + creatures[i].phys.x, creatures[i].phys.y, 1, 1, punctuation, 9, creatures[i].board_index);
						}
					}
				}
			}

			if (creatures[i].attacking)
			{
				if (creatures[i].weapon.weapon_props.weapon_type == WEAPON_TYPE_MELEE)
				{
					creatures[i].attack_angle += creatures[i].weapon.weapon_props.attack_speed*(PI / 30.0f);

					// check for collision with player
					float cosa = cos(creatures[i].attack_angle);
					float sina = sin(creatures[i].attack_angle);

					for (int j = 0; j < creatures[i].weapon.weapon_props.attack_range; ++j)
					{
						if ((player.state & PLAYER_STATE_DEAD) == PLAYER_STATE_DEAD)
							break;

						if (creatures[i].attack_recovery)
							break;

						int start_weapon_x = creatures[i].phys.x + TILE_WIDTH / 2;
						int start_weapon_y = creatures[i].phys.y + TILE_HEIGHT / 2;

						float delta_x = +cosa*j;
						float delta_y = -sina*j;

						if (start_weapon_x + delta_x >= player.phys.x && start_weapon_x + delta_x <= player.phys.x + 0.75*TILE_WIDTH)
						{
							if (start_weapon_y + delta_y >= player.phys.y && start_weapon_y + delta_y <= player.phys.y + 0.75*TILE_HEIGHT)
							{
								int damage = (rand() % (creatures[i].weapon.weapon_props.max_damage - creatures[i].weapon.weapon_props.min_damage + 1)) + creatures[i].weapon.weapon_props.min_damage;

								// add floating number
								spawn_floating_number(start_weapon_x + delta_x, start_weapon_y + delta_y, damage, 6, creatures[i].board_index);

								// player hurt!
								player.phys.hp -= damage;

								creatures[i].attack_recovery = TRUE;

								// check if creature died
								if (creatures[i].phys.hp <= 0)
									player_die();
								else
									player.state |= PLAYER_STATE_HURT;

								break;
							}
						}
					}

				}
				else if (creatures[i].weapon.weapon_props.weapon_type == WEAPON_TYPE_RANGED)
				{
					if ((player.state & PLAYER_STATE_DEAD) == PLAYER_STATE_DEAD)
						break;

					if (creatures[i].attack_recovery)
						break;

					float x_diff = creatures[i].phys.x - player.phys.x;
					float y_diff = creatures[i].phys.y - player.phys.y;

					float angle = atan(y_diff / x_diff);


					float x_vel = cos(angle) * 2;
					float y_vel = -sin(angle) * 2;

					if (x_diff > 0)
					{
						x_vel *= -1.0f;
						y_vel *= -1.0f;
					}

					int projectile_index;

					switch (creatures[i].weapon.tile_index)
					{
					case BOW:
					case CROSSBOW:
						projectile_index = ARROW;
						break;
					case STAFF:
						projectile_index = FIREBALL;
						break;
					case POISON_SPIT:
						projectile_index = POISON_SPIT;
						break;
					}

					spawn_projectile(creatures[i].phys.x, creatures[i].phys.y, 5.0f, x_vel, -y_vel, 2.0f, projectile_index, creatures[i].attack_angle, creatures[i].weapon.weapon_props.min_damage, creatures[i].weapon.weapon_props.max_damage, FALSE);
					projectiles[num_projectiles - 1].shot = TRUE;

					creatures[i].attack_recovery = TRUE;
				}
			}
		}

		else if (creatures[i].npc_props.is_npc && creatures[i].npc_props.talking)
		{
			// update direction to face player
			float diff_x = player.phys.x - creatures[i].phys.x;
			float diff_y = player.phys.y - creatures[i].phys.y;

			if (diff_x <0)
			{
				if (diff_y < 0)
				{
					if (abs(diff_x) > abs(diff_y))
						creatures[i].dir = DIR_LEFT;
					else
						creatures[i].dir = DIR_UP;
				}
				else
				{
					if (abs(diff_x) > abs(diff_y))
						creatures[i].dir = DIR_LEFT;
					else
						creatures[i].dir = DIR_DOWN;
				}
			}
			else
			{
				if (diff_y < 0)
				{
					if (abs(diff_x) > abs(diff_y))
						creatures[i].dir = DIR_RIGHT;
					else
						creatures[i].dir = DIR_UP;
				}
				else
				{
					if (abs(diff_x) > abs(diff_y))
						creatures[i].dir = DIR_RIGHT;
					else
						creatures[i].dir = DIR_DOWN;
				}
			}
		}
		else
		{
			creatures[i].action_counter++;

			if (creatures[i].action_counter >= creatures[i].action_counter_max)
			{
				// choose an action to make
				creatures[i].action_counter = 0;
				creatures[i].state = CREATURE_STATE_ACTING;

				switch (creatures[i].mode)
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

				case CREATURE_MODE_FLEE:

					//creatures[i].action_counter_max = 60;
					//creatures[i].action_duration_counter_max = rand() % 15 + 15 + 1;

					creatures[i].action_counter_max = 0;
					creatures[i].action_duration_counter_max = 10;

					//float unit = 1.5;
					int dd = (rand() % 8) + 1;
					creatures[i].phys.speed = creatures[i].phys.speed * 1.5;

					// might use
					//float diff_x = (creatures[i].phys.x - player.phys.x);
					//float diff_y = (creatures[i].phys.y - player.phys.y);

					switch (dd)
					{
						// should never hit this case
					case 0:
						creatures[i].phys.x_vel = +1;
						creatures[i].phys.y_vel = +1;
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

					creatures[i].action_counter_max = 0;
					creatures[i].action_duration_counter_max = 10;

					action_pursue_flee(i, player.phys.x, player.phys.y, FALSE);

					float diff_x = (creatures[i].phys.x - player.phys.x);
					float diff_y = (creatures[i].phys.y - player.phys.y);

					if (abs(diff_x) < creatures[i].weapon.weapon_props.attack_range && abs(diff_y) < creatures[i].weapon.weapon_props.attack_range)
					{
						creatures[i].action_counter_max = 15.0f / creatures[i].weapon.weapon_props.attack_speed;
						creatures[i].action_duration_counter_max = 15.0f / creatures[i].weapon.weapon_props.attack_speed;
						creatures[i].attacking = TRUE;
						switch (creatures[i].dir)
						{
						case DIR_LEFT:
							creatures[i].attack_angle = +3 * PI / 4;
							break;
						case DIR_RIGHT:
							creatures[i].attack_angle = -1 * PI / 4;
							break;
						case DIR_UP:
							creatures[i].attack_angle = +1 * PI / 4;
							break;
						case DIR_DOWN:
							creatures[i].attack_angle = -3 * PI / 4;
							break;
						}
						creatures[i].phys.x_vel = 0.0f;
						creatures[i].phys.y_vel = 0.0f;
					}
					break;
				}

			}
		}

		// handle creature animation
		if (creatures[i].phys.x_vel != 0 || creatures[i].phys.y_vel != 0)
		{
			creatures[i].anim.counter++;

			if (creatures[i].anim.counter >= 10 / creatures[i].phys.speed)
			{
				// cycle_animation
				creatures[i].anim.counter = 0;
				creatures[i].anim.frame += 1;
				if (creatures[i].anim.frame >= creatures[i].anim.num_frames)
					creatures[i].anim.frame = 0;
			}
		}
		else
		{
			// clear animation frame
			creatures[i].anim.counter = 0;
			creatures[i].anim.frame = 0;
		}

		// handle npc stuff
		if (creatures[i].npc_props.is_npc)
		{
			// check if player is near
			float distance = get_distance(player.phys.x, player.phys.y, creatures[i].phys.x, creatures[i].phys.y);

			if (distance <= creatures[i].npc_props.talk_radius)
			{
				BOOL prev_talking = creatures[i].npc_props.talking;

				if (prev_talking == FALSE)
				{
					// randomly select new dialogue
					creatures[i].npc_props.selected_dialogue_num = rand() % creatures[i].npc_props.num_dialogue;
				}

				creatures[i].npc_props.talking = TRUE;
				creatures[i].npc_props.distance_from_player = distance;
				creatures[i].state = CREATURE_STATE_NEUTRAL; // return to neutral state
				creatures[i].phys.x_vel = +0;
				creatures[i].phys.y_vel = +0;
			}
			else
			{
				creatures[i].npc_props.talking = FALSE;
				creatures[i].npc_props.distance_from_player = -1.0f;
			}

			// update min index
			if (creatures[i].npc_props.talking)
			{
				if (creatures[i].npc_props.distance_from_player < min_distance)
				{
					min_distance = creatures[i].npc_props.distance_from_player;
					min_index = i;
				}
			}
		}

		// handle aging
		++creatures[i].age_counter;
		if (creatures[i].age_counter >= 60)
		{
			// creature is one second older
			creatures[i].age_counter = 0;

			++creatures[i].age;
			if (creatures[i].age == creatures[i].adult_age)
			{
				// throw a party
				int c[5] = { 6,9,11,13,14 };

				for (int j = 0; j < 50; ++j)
					spawn_particle(creatures[i].phys.x + (rand() % TILE_WIDTH), creatures[i].phys.y + (rand() % (TILE_HEIGHT / 2)), rand() % 2 + 1, 3, 0, c[rand() % 5], creatures[i].board_index);
			}

			if (creatures[i].reproductive)
			{
				// @TEMP: Indicate male reproductive creatures
				if (creatures[i].gender == MALE)
				{
					spawn_particle(creatures[i].phys.x + (rand() % TILE_WIDTH), creatures[i].phys.y + (rand() % (TILE_HEIGHT / 2)), 1, 2, CHAR_MALE_SYMBOL, 8, creatures[i].board_index);
				}

				// handle death
				++creatures[i].death_check_counter;
				if (creatures[i].death_check_counter >= SECONDS_PER_DAY)
				{
					// one day since last death check.
					creatures[i].death_check_counter = 0;

					int death_num = rand() % 10000;
					int life_in_days = min(60, floor(creatures[i].age / SECONDS_PER_DAY));

					// min(max((rats/200)^2,1),maxq_mult) * base_rate
					float num_rat_rate = num_creatures / 200.0f;
					float chance_of_death = min(max(num_rat_rate*num_rat_rate, 1), 25) * mortality_table[life_in_days];

					if (death_num >= (10000 - (10000.0f*chance_of_death)))
					{
						// creature dies.
						creature_death(i);
					}
				}
			}

			// handle creature mating
			if (creatures[i].reproductive && creatures[i].gender == MALE && creatures[i].age >= creatures[i].adult_age)
			{
				for (int grid_offset_x = -1; grid_offset_x <= +1; ++grid_offset_x)
				{
					int x_bound = creatures[i].grid_index_x + grid_offset_x;
					if (x_bound < 0 || x_bound >= CREATURE_GRID_X_MAX) continue;

					for (int grid_offset_y = -1; grid_offset_y <= +1; ++grid_offset_y)
					{
						int y_bound = creatures[i].grid_index_y + grid_offset_y;
						if (y_bound < 0 || y_bound >= CREATURE_GRID_X_MAX) continue;

						for (int j = 0; j < creature_grid[x_bound][y_bound].num_creatures; ++j)
						{
							Creature* creature_j = creature_grid[x_bound][y_bound].creatures[j];

							// are creatures opposite gender?
							if (creature_j->gender == !creatures[i].gender)
							{
								// is creature reproductive and an adult and not currently pregnant?
								if (creature_j->reproductive && creature_j->age >= creature_j->adult_age && !creature_j->pregnant)
								{
									// is creature not recoverying from a pregnancy?
									if (!creature_j->birth_recovery)
									{
										// are creatures in same zone?
										if (strcmp(creature_j->zone.name, creatures[i].zone.name) == 0)
										{
											// are creatures the same species?
											if (strcmp(creature_j->species, creatures[i].species) == 0)
											{
												// look at x distance
												if (abs(creatures[i].phys.x - creature_j->phys.x) <= creatures[i].mating_radius)
												{
													// look at y distance
													if (abs(creatures[i].phys.y - creature_j->phys.y) <= creatures[i].mating_radius)
													{
														// get direct distance
														double distance = get_distance(creatures[i].phys.x, creatures[i].phys.y, creature_j->phys.x, creature_j->phys.y);

														if (distance <= creatures[i].mating_radius)
														{
															// spawn flames
															for (int k = 0; k < 10; ++k)
																spawn_particle(creature_j->phys.x + (rand() % TILE_WIDTH), creature_j->phys.y + (rand() % (TILE_HEIGHT / 2)), 1, 3, CHAR_FLAME, 9, creature_j->board_index);

															// calculate chance of pregnancy
															int chance_of_preg = rand() % 100;

															if (chance_of_preg < 100) // 100% chance of pregnancy
															{
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
					}
				}
			}

			// handle pregnancies
			if (creatures[i].pregnant)
			{
				spawn_particle(creatures[i].phys.x + (rand() % TILE_WIDTH), creatures[i].phys.y + (rand() % (TILE_HEIGHT / 2)), 1, 3, CHAR_HEART, 6, creatures[i].board_index);

				++creatures[i].gestation_counter;
				if (creatures[i].gestation_counter >= creatures[i].gestation_period)
				{
					creatures[i].gestation_counter = 0;

					// have a baby
					int num_babies = (rand() % creatures[i].litter_max) + 1;

					for (int j = 0; j < num_babies; ++j)
					{
						spawn_creature(creatures[i].name, creatures[i].board_name, creatures[i].phys.x, creatures[i].phys.y);
						creatures[num_creatures - 1].age = 0; // set age to zero (newborn)
						get_zone_by_name(creatures[i].zone.name, &creatures[num_creatures - 1].zone); // set zone
						++num_births;
					}

					// have a party
					int c[5] = { 6,9,11,13,14 };

					for (int j = 0; j < 50; ++j)
						spawn_particle(creatures[i].phys.x + (rand() % TILE_WIDTH), creatures[i].phys.y + (rand() % (TILE_HEIGHT / 2)), rand() % 2 + 1, 3, 0, c[rand() % 5], creatures[i].board_index);

					creatures[i].pregnant = FALSE;
					creatures[i].phys.base_speed *= 2.0f; // return creature back to normal speed
					creatures[i].birth_recovery = TRUE;
					--num_pregs;
				}
			}
			else if (creatures[i].birth_recovery)
			{
				++creatures[i].birth_recovery_counter;
				if (creatures[i].birth_recovery_counter >= creatures[i].birth_recovery_time)
				{
					creatures[i].birth_recovery_counter = 0;
					creatures[i].birth_recovery = FALSE;
				}
			}

		}
	}

	// update npc message
	if (min_index > -1)
	{
		message.name = creatures[min_index].name;
		message.message = creatures[min_index].npc_props.dialogue[creatures[min_index].npc_props.selected_dialogue_num];

		if (creatures[min_index].npc_props.is_vendor)
			message.value = creatures[min_index].npc_props.vendor_credit;
		else
			message.value = -1;

		message.color = 10;
		message.active = TRUE;
	}

}
*/
static void draw_creature_only(int i)
{
    int creature_x = creatures[i].phys.x - camera.x;
    int creature_y = creatures[i].phys.y - camera.y;

    int age_offset = 0;
    if(creatures[i].reproductive && creatures[i].age < creatures[i].adult_age)
        age_offset = 16;

    if (creatures[i].stunned)
    {
        // draw red tint
        draw_tile_tinted(creatures[i].phys.x - camera.x, creatures[i].phys.y - camera.y,creatures[i].tileset_name, creatures[i].tile_index + age_offset + creatures[i].dir + creatures[i].anim.frame_order[creatures[i].anim.frame],6);
    }
    else
    {
        draw_tile(creatures[i].phys.x - camera.x, creatures[i].phys.y - camera.y,creatures[i].tileset_name, creatures[i].tile_index + age_offset + creatures[i].dir + creatures[i].anim.frame_order[creatures[i].anim.frame],day_cycle_shade_amount);
    }
}
static void draw_creature_weapon(int i)
{
    draw_tile_rotated_shadow(creatures[i].phys.x - camera.x + cos(creatures[i].attack_angle)*2*TILE_WIDTH/3,creatures[i].phys.y - camera.y - sin(creatures[i].attack_angle) * 2*TILE_HEIGHT/3,creatures[i].weapon.tileset_name,creatures[i].weapon.tile_index,creatures[i].attack_angle,max(10-day_cycle_shade_amount,0));
    draw_tile_rotated(creatures[i].phys.x - camera.x + cos(creatures[i].attack_angle)*2*TILE_WIDTH/3,creatures[i].phys.y - camera.y - sin(creatures[i].attack_angle) * 2*TILE_HEIGHT/3 - 0.5*creatures[i].phys.z,creatures[i].weapon.tileset_name,creatures[i].weapon.tile_index,creatures[i].attack_angle, day_cycle_shade_amount);
}

static void draw_creature(int i)
{
    if(creatures[i].board_index != current_board_index)
        return;

    if(creatures[i].dir == DIR_UP)
    {
        if(creatures[i].attacking)
            draw_creature_weapon(i);

        draw_creature_only(i);
    }
    else
    {
        draw_creature_only(i);

        if(creatures[i].attacking)
            draw_creature_weapon(i);
    }
}
