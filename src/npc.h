#define MAX_NPCS     1000
#define MAX_NPC_LIST 1000

typedef struct
{
    char* name;
    char* board_name;
    int board_index;
    char* tileset_name;
    int tile_index;
    int xp;
    PhysicalProperties phys;
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
    Item weapon;
    Item armor_head;
    Item armor_body;
    Item armor_hands;
    Item armor_feet;
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
            npc->phys.x = npc_list[i].phys.x;
            npc->phys.y = npc_list[i].phys.y;
            npc->phys.hp = npc_list[i].phys.hp;
            npc->phys.max_hp = npc_list[i].phys.max_hp;
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

	return FALSE;
}

static BOOL spawn_npc(const char* npc_name)
{
    NPC npc = {0};

    if(!get_npc_by_name(npc_name,&npc))
        return FALSE;

    npcs[num_npcs].phys.x = npc.phys.x; 
    npcs[num_npcs].phys.y = npc.phys.y;
    npcs[num_npcs].phys.x_vel = 0;
    npcs[num_npcs].phys.y_vel = 0;
    npcs[num_npcs].phys.base_speed = 0.5f;
    npcs[num_npcs].phys.speed = 0.5f;
    npcs[num_npcs].dir = DIR_DOWN;
    npcs[num_npcs].state = CREATURE_STATE_NEUTRAL;
    npcs[num_npcs].talking = FALSE;
    npcs[num_npcs].name = npc.name;
    npcs[num_npcs].board_name = "Astoria";
    npcs[num_npcs].board_index = get_board_index_by_name(npcs[num_npcs].board_name);
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
    npcs[num_npcs].phys.hp = npc.phys.hp; 
    npcs[num_npcs].phys.max_hp = npc.phys.max_hp; 
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
    get_item_by_name("Rat Helm",&npcs[num_npcs].armor_head);

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
            float diff_x = player.phys.x - npcs[i].phys.x;
            float diff_y = player.phys.y - npcs[i].phys.y;

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
                        break;
                    case 1: 
                        npcs[i].state = CREATURE_STATE_ACTING;
                        npcs[i].dir = DIR_UP;
                        npcs[i].phys.x_vel = +0;
                        npcs[i].phys.y_vel = -1;
                        break;
                    case 2: 
                        npcs[i].state = CREATURE_STATE_ACTING;
                        npcs[i].dir = DIR_DOWN;
                        npcs[i].phys.x_vel = +0;
                        npcs[i].phys.y_vel = +1;
                        break;
                    case 3: 
                        npcs[i].state = CREATURE_STATE_ACTING;
                        npcs[i].dir = DIR_LEFT;
                        npcs[i].phys.x_vel = -1;
                        npcs[i].phys.y_vel = +0;
                        break;
                    case 4: 
                        npcs[i].state = CREATURE_STATE_ACTING;
                        npcs[i].dir = DIR_RIGHT;
                        npcs[i].phys.x_vel = +1;
                        npcs[i].phys.y_vel = +0;
                        break;
                    case 5: 
                        npcs[i].state = CREATURE_STATE_ACTING;
                        npcs[i].dir = DIR_UP;
                        npcs[i].phys.x_vel = -1;
                        npcs[i].phys.y_vel = -1;
                        break;
                    case 6: 
                        npcs[i].state = CREATURE_STATE_ACTING;
                        npcs[i].dir = DIR_UP;
                        npcs[i].phys.x_vel = +1;
                        npcs[i].phys.y_vel = -1;
                        break;
                    case 7: 
                        npcs[i].state = CREATURE_STATE_ACTING;
                        npcs[i].dir = DIR_DOWN;
                        npcs[i].phys.x_vel = -1;
                        npcs[i].phys.y_vel = +1;
                        break;
                    case 8: 
                        npcs[i].state = CREATURE_STATE_ACTING;
                        npcs[i].dir = DIR_DOWN;
                        npcs[i].phys.x_vel = +1;
                        npcs[i].phys.y_vel = +1;
                        break;
                }
            }

            npcs[i].action_counter++;
        }

        switch (npcs[i].state)
        {
            case CREATURE_STATE_ACTING:

                npcs[i].phys.x += npcs[i].phys.x_vel*npcs[i].phys.speed;
                npcs[i].phys.y += npcs[i].phys.y_vel*npcs[i].phys.speed;
                        
                handle_terrain_collision(npcs[i].board_index, &npcs[i].phys);

                if(npcs[i].phys.x < 0) npcs[i].phys.x = 0;
                if(npcs[i].phys.y < 0) npcs[i].phys.y = 0;
                if(npcs[i].phys.x >TILE_WIDTH*(BOARD_TILE_WIDTH-1)) npcs[i].phys.x = TILE_WIDTH*(BOARD_TILE_WIDTH-1);
                if(npcs[i].phys.y >TILE_HEIGHT*(BOARD_TILE_HEIGHT-1)) npcs[i].phys.y = TILE_HEIGHT*(BOARD_TILE_HEIGHT-1);

                break;
        }
                
        // handle npc animation
        if(npcs[i].phys.x_vel != 0 || npcs[i].phys.y_vel != 0)
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
            npcs[i].phys.x_vel = +0;
            npcs[i].phys.y_vel = +0;
        }

        // check if player is near
        float distance = get_distance(player.phys.x,player.phys.y,npcs[i].phys.x,npcs[i].phys.y);

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
            npcs[i].phys.x_vel = +0;
            npcs[i].phys.y_vel = +0;
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
    if(npcs[i].board_index != current_board_index)
        return;

    int npc_x = npcs[i].phys.x - camera.x;
    int npc_y = npcs[i].phys.y - camera.y;

    draw_tile(npc_x, npc_y, npcs[i].tileset_name,npcs[i].tile_index + npcs[i].dir + npcs[i].anim.frame_order[npcs[i].anim.frame],day_cycle_shade_amount);

    if(npcs[i].armor_head.name != NULL)
        draw_tile(npcs[i].phys.x - camera.x, npcs[i].phys.y - camera.y + npcs[i].armor_head.armor_props.y_offset - 0.5*npcs[i].phys.z, npcs[i].armor_head.tileset_name, npcs[i].armor_head.tile_index + npcs[i].dir + npcs[i].anim.frame_order[npcs[i].anim.frame], day_cycle_shade_amount);
}
