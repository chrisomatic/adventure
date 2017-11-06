#define MAX_NPCS     1000
#define MAX_NPC_LIST 1000

typedef struct
{
    char* name;
    char* tileset_name;
    int tile_index;
    int xp;
    int hp;
    int max_hp;
    float x;
    float y;
    int x_vel;
    int y_vel;
    float speed;
    float distance_from_player;
    int action_counter;
    int action_counter_max;
    int action_duration_counter;
    int action_duration_counter_max;
    int talk_radius;
    int num_dialogue;
    int selected_dialogue_num;
    char* dialogue[10];
    BOOL talking;
    CreatureState state;
    Direction dir;
    Animation anim;
} NPC;

NPC npcs[MAX_NPCS];
NPC npc_list[MAX_NPC_LIST];

int num_npcs = 0;
int num_npc_list = 0;

static BOOL get_npc_by_name(const char* name,NPC* npc)
{
    for(int i = 0; i < MAX_NPC_LIST; ++i)
    {
		if (npc_list[i].name == NULL)
			continue;

        if(strcmp(npc_list[i].name, name) == 0)
        {
            npc->name = npc_list[i].name;
            npc->x = npc_list[i].x;
            npc->y = npc_list[i].y;
            npc->hp = npc_list[i].hp;
            npc->max_hp = npc_list[i].max_hp;
            npc->xp = npc_list[i].xp;
			npc->num_dialogue = npc_list[i].num_dialogue;
			npc->dialogue[0] = npc_list[i].dialogue[0];
			npc->dialogue[1] = npc_list[i].dialogue[1];
			npc->dialogue[2] = npc_list[i].dialogue[2];
			npc->dialogue[3] = npc_list[i].dialogue[3];
			npc->dialogue[4] = npc_list[i].dialogue[4];
			npc->dialogue[5] = npc_list[i].dialogue[5];
			npc->dialogue[6] = npc_list[i].dialogue[6];
			npc->dialogue[7] = npc_list[i].dialogue[7];
			npc->dialogue[8] = npc_list[i].dialogue[8];
			npc->dialogue[9] = npc_list[i].dialogue[9];
			npc->tile_index = npc_list[i].tile_index;
			npc->tileset_name = npc_list[i].tileset_name;

            return TRUE;
        }
    }
}

static BOOL spawn_npc(const char* npc_name)
{
    NPC npc = {0};

    if(!get_npc_by_name(npc_name,&npc))
        return;

    npcs[num_npcs].x = npc.x; 
    npcs[num_npcs].y = npc.y;
    npcs[num_npcs].x_vel = 0;
    npcs[num_npcs].y_vel = 0;
    npcs[num_npcs].speed = 0.5f;
    npcs[num_npcs].dir = DIR_DOWN;
    npcs[num_npcs].state = CREATURE_STATE_NEUTRAL;
    npcs[num_npcs].talking = FALSE;
    npcs[num_npcs].name = npc.name;
	npcs[num_npcs].num_dialogue = npc.num_dialogue;
    npcs[num_npcs].selected_dialogue_num = 0;
	npcs[num_npcs].dialogue[0] = npc.dialogue[0];
	npcs[num_npcs].dialogue[1] = npc.dialogue[1];
	npcs[num_npcs].dialogue[2] = npc.dialogue[2];
	npcs[num_npcs].dialogue[3] = npc.dialogue[3];
	npcs[num_npcs].dialogue[4] = npc.dialogue[4];
	npcs[num_npcs].dialogue[5] = npc.dialogue[4];
	npcs[num_npcs].dialogue[6] = npc.dialogue[4];
	npcs[num_npcs].dialogue[7] = npc.dialogue[4];
	npcs[num_npcs].dialogue[8] = npc.dialogue[4];
	npcs[num_npcs].dialogue[9] = npc.dialogue[4];
    npcs[num_npcs].tile_index = npc.tile_index;
    npcs[num_npcs].tileset_name = npc.tileset_name;
    npcs[num_npcs].hp = npc.hp; 
    npcs[num_npcs].max_hp = npc.max_hp; 
    npcs[num_npcs].xp = npc.xp;
    npcs[num_npcs].talk_radius = 20;
    npcs[num_npcs].distance_from_player = -1.0f;
    npcs[num_npcs].action_counter_max = 180;
    npcs[num_npcs].action_counter = rand() % npcs[num_npcs].action_counter_max;
    npcs[num_npcs].action_duration_counter = 0;
    npcs[num_npcs].anim.counter = 0;
    npcs[num_npcs].anim.max_count = 10;
    npcs[num_npcs].anim.frame = 0;
    npcs[num_npcs].anim.num_frames = 16;
    npcs[num_npcs].anim.frame_order[0] = 0;
    npcs[num_npcs].anim.frame_order[1] = 1;
    npcs[num_npcs].anim.frame_order[2] = 1;
    npcs[num_npcs].anim.frame_order[3] = 1;
    npcs[num_npcs].anim.frame_order[4] = 1;
    npcs[num_npcs].anim.frame_order[5] = 1;
    npcs[num_npcs].anim.frame_order[6] = 1;
    npcs[num_npcs].anim.frame_order[7] = 1;
    npcs[num_npcs].anim.frame_order[8] = 1;
    npcs[num_npcs].anim.frame_order[9] = 1;
    npcs[num_npcs].anim.frame_order[10] = 1;
    npcs[num_npcs].anim.frame_order[11] = 1;
    npcs[num_npcs].anim.frame_order[12] = 1;
    npcs[num_npcs].anim.frame_order[13] = 1;
    npcs[num_npcs].anim.frame_order[14] = 1;
    npcs[num_npcs].anim.frame_order[15] = 1;
    ++num_npcs;

    if(num_npcs > MAX_NPCS -1)
    {
        num_npcs = MAX_NPCS - 1;
        return FALSE;
    }

    return TRUE;
    
}

static void init_npcs()
{
    num_npcs = 0;

    for(int i = 0; i < num_npc_list; ++i)
        spawn_npc(npc_list[i].name);
}

static void update_npcs()
{
    // update npcs
    for(int i = 0; i < num_npcs;++i)
    {
        if(npcs[i].talking)
        {
            // update direction to face player
            float diff_x = player.x - npcs[i].x;
            float diff_y = player.y - npcs[i].y;

            if(diff_x <0)
            {
                if(diff_y < 0)
                {
                    if(abs(diff_x) > abs(diff_y))
                        npcs[i].dir = DIR_LEFT;
                    else
                        npcs[i].dir = DIR_UP;
                }
                else
                {
                    if(abs(diff_x) > abs(diff_y))
                        npcs[i].dir = DIR_LEFT;
                    else
                        npcs[i].dir = DIR_DOWN;
                }
            }
            else
            {
                if(diff_y < 0)
                {
                    if(abs(diff_x) > abs(diff_y))
                        npcs[i].dir = DIR_RIGHT;
                    else
                        npcs[i].dir = DIR_UP;
                }
                else
                {
                    if(abs(diff_x) > abs(diff_y))
                        npcs[i].dir = DIR_RIGHT;
                    else
                        npcs[i].dir = DIR_DOWN;
                }
            }
        }
        else
        {
            if(npcs[i].action_counter >= npcs[i].action_counter_max)
            {
                npcs[i].action_counter = 0;
                npcs[i].action_duration_counter_max = rand() % 180 + 60; // duration of action
                        
                // take an action
                int d = rand() % 9;
                switch (d)
                {
                    case 0:
                        npcs[i].state = CREATURE_STATE_NEUTRAL;
                    case 1: 
                        npcs[i].state = CREATURE_STATE_ACTING;
                        npcs[i].dir = DIR_UP;
                        npcs[i].x_vel = +0;
                        npcs[i].y_vel = -1;
                        break;
                    case 2: 
                        npcs[i].state = CREATURE_STATE_ACTING;
                        npcs[i].dir = DIR_DOWN;
                        npcs[i].x_vel = +0;
                        npcs[i].y_vel = +1;
                        break;
                    case 3: 
                        npcs[i].state = CREATURE_STATE_ACTING;
                        npcs[i].dir = DIR_LEFT;
                        npcs[i].x_vel = -1;
                        npcs[i].y_vel = +0;
                        break;
                    case 4: 
                        npcs[i].state = CREATURE_STATE_ACTING;
                        npcs[i].dir = DIR_RIGHT;
                        npcs[i].x_vel = +1;
                        npcs[i].y_vel = +0;
                        break;
                    case 5: 
                        npcs[i].state = CREATURE_STATE_ACTING;
                        npcs[i].dir = DIR_UP;
                        npcs[i].x_vel = -1;
                        npcs[i].y_vel = -1;
                        break;
                    case 6: 
                        npcs[i].state = CREATURE_STATE_ACTING;
                        npcs[i].dir = DIR_UP;
                        npcs[i].x_vel = +1;
                        npcs[i].y_vel = -1;
                        break;
                    case 7: 
                        npcs[i].state = CREATURE_STATE_ACTING;
                        npcs[i].dir = DIR_DOWN;
                        npcs[i].x_vel = -1;
                        npcs[i].y_vel = +1;
                        break;
                    case 8: 
                        npcs[i].state = CREATURE_STATE_ACTING;
                        npcs[i].dir = DIR_DOWN;
                        npcs[i].x_vel = +1;
                        npcs[i].y_vel = +1;
                        break;
                }
            }

            npcs[i].action_counter++;
        }

        switch (npcs[i].state)
        {
            case CREATURE_STATE_ACTING:

                npcs[i].x += npcs[i].x_vel*npcs[i].speed;
                npcs[i].y += npcs[i].y_vel*npcs[i].speed;
                        
                // handle npcs[i] collision
                // terrain collision
                //
                // 1            2
                //  x----------x
                //  |          |
                //  |          |
                //  x----------x
                // 3            4
                //

                int npc_check_x1 = (npcs[i].x + TILE_WIDTH/4) / TILE_WIDTH;
                int npc_check_y1 = (npcs[i].y + TILE_HEIGHT/2) / TILE_HEIGHT;

                int npc_check_x2 = (npcs[i].x + 3*TILE_WIDTH/4) / TILE_WIDTH;
                int npc_check_y2 = (npcs[i].y + TILE_HEIGHT/2) / TILE_HEIGHT;

                int npc_check_x3 = (npcs[i].x + TILE_WIDTH/4) / TILE_WIDTH;
                int npc_check_y3 = (npcs[i].y + TILE_HEIGHT) / TILE_HEIGHT;

                int npc_check_x4 = (npcs[i].x + 3*TILE_WIDTH/4) / TILE_WIDTH;
                int npc_check_y4 = (npcs[i].y + TILE_HEIGHT) / TILE_HEIGHT;

                int collision_value_1 = world_collision[npc_check_x1][npc_check_y1];
                int collision_value_2 = world_collision[npc_check_x2][npc_check_y2];
                int collision_value_3 = world_collision[npc_check_x3][npc_check_y3];
                int collision_value_4 = world_collision[npc_check_x4][npc_check_y4];

                if(collision_value_1 == 5 || collision_value_2 == 5 || collision_value_3 == 5 || collision_value_4 == 5)
                {
                    BOOL correct_x = FALSE;
                    BOOL correct_y = FALSE;

                    if((collision_value_1 == 5 && collision_value_3 == 5) || (collision_value_2 == 5 && collision_value_4 == 5))
                    {
                        //correct collision x
                        npcs[i].x -= npcs[i].x_vel*npcs[i].speed;
                        correct_x = TRUE;
                    }
                    if((collision_value_1 == 5 && collision_value_2 == 5) || (collision_value_3 == 5 && collision_value_4 == 5))
                    {
                        //correct collision y
                        npcs[i].y -= npcs[i].y_vel*npcs[i].speed;
                        correct_y = TRUE;
                    }
                            
                    if(!correct_x && !correct_y)
                    {
                        if(collision_value_1 == 5 || collision_value_3 == 5)
                            npcs[i].x += 1.0f*npcs[i].speed;
                                
                        if(collision_value_2 == 5 || collision_value_4 == 5)
                            npcs[i].x -= 1.0f*npcs[i].speed;
                    }
                }

                if(npcs[i].x < 0) npcs[i].x = 0;
                if(npcs[i].y < 0) npcs[i].y = 0;
                if(npcs[i].x >TILE_WIDTH*(WORLD_TILE_WIDTH-1)) npcs[i].x = TILE_WIDTH*(WORLD_TILE_WIDTH-1);
                if(npcs[i].y >TILE_HEIGHT*(WORLD_TILE_HEIGHT-1)) npcs[i].y = TILE_HEIGHT*(WORLD_TILE_HEIGHT-1);

                break;
        }
                
        // handle npc animation
        if(npcs[i].x_vel != 0 || npcs[i].y_vel != 0)
        {
            npcs[i].anim.counter++;

            if(npcs[i].anim.counter >= npcs[i].anim.max_count)
            {
                // cycle_animation
                npcs[i].anim.counter = 0;
                npcs[i].anim.frame += 1;
                if(npcs[i].anim.frame >= npcs[i].anim.num_frames)
                    npcs[i].anim.frame = 0;
            }
        }
        else
        {
            // clear animation frame
            npcs[i].anim.counter = 0;
            npcs[i].anim.frame = 0;
        }


        npcs[i].action_duration_counter++;

        if(npcs[i].action_duration_counter >= npcs[i].action_duration_counter_max)
        {
            npcs[i].action_duration_counter = 0;
            npcs[i].state = CREATURE_STATE_NEUTRAL; // return to neutral state
            npcs[i].x_vel = +0;
            npcs[i].y_vel = +0;
        }

        // check if player is near
        float distance = get_distance(player.x,player.y,npcs[i].x,npcs[i].y);

        if(distance <= npcs[i].talk_radius)
        {
            BOOL prev_talking = npcs[i].talking;

            if(prev_talking == FALSE)
            {
                // randomly select new dialogue
                npcs[i].selected_dialogue_num = rand() % npcs[i].num_dialogue;
            }

            npcs[i].talking = TRUE;
            npcs[i].distance_from_player = distance;
            npcs[i].state = CREATURE_STATE_NEUTRAL; // return to neutral state
            npcs[i].x_vel = +0;
            npcs[i].y_vel = +0;
        }
        else
        {
            npcs[i].talking = FALSE;
            npcs[i].distance_from_player = -1.0f;
        }
    }

    // update message box with closest npc
    float min_distance = 1000.0f;
    int min_index = -1;

    for(int i = 0; i < num_npcs;++i)
    {
        if(!npcs[i].talking)
            continue;

        if(npcs[i].distance_from_player < min_distance)
        {
            min_distance = npcs[i].distance_from_player;
            min_index = i;
        }
    }

    if(min_index > -1)
    {
        message.name = npcs[min_index].name;
        message.message = npcs[min_index].dialogue[npcs[min_index].selected_dialogue_num];
        message.color = 10;
        message.active = TRUE;
    }
}

static void draw_npc(int i)
{
        int npc_x = npcs[i].x - camera.x;
        int npc_y = npcs[i].y - camera.y;

        draw_tile(npcs[i].x - camera.x, npcs[i].y - camera.y, npcs[i].tileset_name,npcs[i].tile_index + npcs[i].dir + npcs[i].anim.frame_order[npcs[i].anim.frame],day_cycle_shade_amount);
}

static void draw_npcs()
{
	for (int i = 0; i < num_npcs; ++i)
	{
        draw_npc(i);
	}
}
