#define BOARD_TILE_WIDTH  256
#define BOARD_TILE_HEIGHT 256
#define GRAVITY 0.2f;
#define AIR_RESISTANCE 0.01f;
#define GROUND_FRICTION 0.1f;
#define DAY_CYCLE_COUNTER_MAX 14400 
#define SECONDS_PER_DAY (DAY_CYCLE_COUNTER_MAX / TARGET_FPS)
#define DAY_CYCLE_DAYTIME 3600 
#define DAY_CYCLE_NIGHTTIME 10800

#define GRASS    0
#define MARSH    1
#define WATER    2
#define WATER2   3
#define SAND     4
#define MUD      5
#define MOUNTAIN 6
#define SNOW     7
#define LAVA     8
#define LAVA2    9
#define STONE    10 
#define WOOD     11

typedef struct
{
    float x;
    float y;
    float z;
    float width;
	float length;
    float height;
} BoundingBox;

typedef enum
{
    ITEM_TYPE_NONE,
    ITEM_TYPE_HEALTH,
    ITEM_TYPE_MANA,
    ITEM_TYPE_WEAPON,
    ITEM_TYPE_ARMOR,
    ITEM_TYPE_QUEST
} ItemType;

typedef enum
{
	ARMOR_TYPE_NONE,
	ARMOR_TYPE_HEAD,
	ARMOR_TYPE_BODY,
	ARMOR_TYPE_HANDS,
	ARMOR_TYPE_FEET
} ArmorType;

typedef enum
{
    WEAPON_TYPE_MELEE,
    WEAPON_TYPE_RANGED
} WeaponType;

typedef struct
{
    float attack_speed;
    int attack_range;
    int min_damage;
    int max_damage;
    WeaponType weapon_type;
} WeaponProperties;

typedef struct
{
    float defence;
    ArmorType armor_type;
    int y_offset;
} ArmorProperties;

typedef struct
{
    float x;
    float y;
    float z;
    float x_vel;
    float y_vel;
    float z_vel;
    float base_speed;
    float speed;
    float height;
    int environmental_hurt_counter;
    int environmental_hurt_max;
    int hp;
    int max_hp;
} PhysicalProperties;

typedef struct
{
    char* name;
    int board_index;
    char* board_name;
    char* description;
    float friction;
    char* tileset_name;
    int  tile_index;
    int  value;
    BOOL highlighted;
    BOOL consummable;
    ItemType type;
    WeaponProperties weapon_props;
    ArmorProperties  armor_props;
    PhysicalProperties phys;
} Item;

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
	DIR_UP = 0,
	DIR_DOWN = 3,
	DIR_LEFT = 6,
	DIR_RIGHT = 9
} Direction;

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
    PLAYER_STATE_MIDAIR = 8,
	PLAYER_STATE_NOTCHED  = 16,
	PLAYER_STATE_DEAD   = 32 
} PlayerState;

typedef struct
{
    char* name;
    char* board_name;
    char* tileset_name;
    int tile_index;
    int lvl;
    float xp;
    int mp;
    int max_mp;
    PhysicalProperties phys;
    int gold;
    int coin_throw_counter;
    int coin_throw_max;
    int bounce_counter;
    int bounce_counter_max;
    float attack_angle;
    int attack_frame_counter;
    BOOL throw_coins;
    BOOL pickup;
    BOOL take;
    BOOL jump;
    BOOL notch;
    BOOL shoot;
    int item_held_index;
    int notch_index;
    int hurt_counter;
    int hurt_counter_max;
    Item weapon;
    Item armor_head;
    Item armor_body;
    Item armor_hands;
    Item armor_feet;
    Direction dir;
    AttackDirection attack_dir;
    Animation anim;
    PlayerState state;
} Player;

Player player;

int foes_killed = 0; // @TEMP
long next_level = 100;

static int current_board_index = 0;

static void gain_level()
{
    player.lvl++;
    player.xp -= next_level;
    next_level *= 2.00f;

    spawn_floating_string(player.phys.x + TILE_WIDTH/2, player.phys.y,"+Lvl",8);
    for(int i = 0; i < 10; ++i)
        spawn_particle(rand() % TILE_WIDTH + player.phys.x,player.phys.y,2,5,'*',8,current_board_index);
}

typedef struct
{
    char name[100];
    int map_x_index;
    int map_y_index;
    unsigned int data[BOARD_TILE_HEIGHT][BOARD_TILE_WIDTH];
    unsigned int collision[BOARD_TILE_HEIGHT][BOARD_TILE_WIDTH];
} Board;

Board board_list[16];

int num_boards = 0;

static int world_water_anim_counter = 0;
static int world_water_anim_counter_max = 5;

static int day_cycle_counter = 0;
static int day_cycle_direction = 1;
static int day_cycle_shade_amount = 0;

const char* terrain_tileset_name = "terrain";

static int get_board_index_by_name(const char* name)
{
	if (!name)
		return;

    for(int i = 0; i < num_boards; ++i)
    {
        if(strcmp(board_list[i].name,name) == 0)
            return i;
    }

    return -1;
}

static int get_name_of_board_location(int x, int y,char* ret_name)
{
	if (x < 0 || y < 0)
		return -1;

    for(int i = 0; i < num_boards; ++i)
    {
        if(board_list[i].map_x_index == x && board_list[i].map_y_index == y)
        {
			ret_name = board_list[i].name;
            return i;
        }
    }

    return -1;

}

static void handle_terrain_collision(int board_index, PhysicalProperties* phys)
{
    // terrain collision
    //
    // 1            2
    //  x----------x
    //  |          |
    //  |          |
    //  x----------x
    // 3            4
    //

    // if entity isn't currently moving, we can return 
    if(phys->x_vel == 0.0f && phys->y_vel == 0.0f)
        return;

    int check_x1,check_y1, check_x2, check_y2, check_x3, check_y3, check_x4, check_y4;
    int collision_value_1,collision_value_2, collision_value_3, collision_value_4;

    // check collision
    check_x1 = (phys->x + 1*TILE_WIDTH/4) / TILE_WIDTH; check_y1 = (phys->y + TILE_HEIGHT/2) / TILE_HEIGHT;
    check_x2 = (phys->x + 3*TILE_WIDTH/4) / TILE_WIDTH; check_y2 = (phys->y + TILE_HEIGHT/2) / TILE_HEIGHT;
    check_x3 = (phys->x + 1*TILE_WIDTH/4) / TILE_WIDTH; check_y3 = (phys->y + TILE_HEIGHT/1) / TILE_HEIGHT;
    check_x4 = (phys->x + 3*TILE_WIDTH/4) / TILE_WIDTH; check_y4 = (phys->y + TILE_HEIGHT/1) / TILE_HEIGHT;

    collision_value_1 = board_list[board_index].collision[min(check_x1, BOARD_TILE_WIDTH - 1)][min(check_y1, BOARD_TILE_HEIGHT - 1)];
    collision_value_2 = board_list[board_index].collision[min(check_x2, BOARD_TILE_WIDTH - 1)][min(check_y2, BOARD_TILE_HEIGHT - 1)];
    collision_value_3 = board_list[board_index].collision[min(check_x3, BOARD_TILE_WIDTH - 1)][min(check_y3, BOARD_TILE_HEIGHT - 1)];
    collision_value_4 = board_list[board_index].collision[min(check_x4, BOARD_TILE_WIDTH - 1)][min(check_y4, BOARD_TILE_HEIGHT - 1)];

    if(collision_value_1 == 5 || collision_value_2 == 5 || collision_value_3 == 5 || collision_value_4 == 5)
    {
        // correct collision along one axis
        if((collision_value_1 == 5 && collision_value_3 == 5) || (collision_value_2 == 5 && collision_value_4 == 5))
            phys->x -= phys->x_vel*phys->speed; // correct along x-axis
        else if((collision_value_1 == 5 && collision_value_2 == 5) || (collision_value_3 == 5 && collision_value_4 == 5))
            phys->y -= phys->y_vel*phys->speed; // correct along y-axis
        else if(collision_value_1 == 5)
        {
            if(phys->y_vel != 0.0f) phys->x += 1.0f*phys->speed;
            if(phys->x_vel != 0.0f) phys->y += 1.0f*phys->speed;
        }
        else if(collision_value_2 == 5)
        {
            if(phys->y_vel != 0.0f) phys->x -= 1.0f*phys->speed;
            if(phys->x_vel != 0.0f) phys->y += 1.0f*phys->speed;
        }
        else if(collision_value_3 == 5)
        {
            if(phys->y_vel != 0.0f) phys->x += 1.0f*phys->speed;
            if(phys->x_vel != 0.0f) phys->y -= 1.0f*phys->speed;
        }
        else if(collision_value_4 == 5)
        {
            if(phys->y_vel != 0.0f) phys->x -= 1.0f*phys->speed;
            if(phys->x_vel != 0.0f) phys->y -= 1.0f*phys->speed;
        }
        
        // check collision again
        check_x1 = (phys->x + 1*TILE_WIDTH/4) / TILE_WIDTH; check_y1 = (phys->y + TILE_HEIGHT/2) / TILE_HEIGHT;
        check_x2 = (phys->x + 3*TILE_WIDTH/4) / TILE_WIDTH; check_y2 = (phys->y + TILE_HEIGHT/2) / TILE_HEIGHT;
        check_x3 = (phys->x + 1*TILE_WIDTH/4) / TILE_WIDTH; check_y3 = (phys->y + TILE_HEIGHT/1) / TILE_HEIGHT;
        check_x4 = (phys->x + 3*TILE_WIDTH/4) / TILE_WIDTH; check_y4 = (phys->y + TILE_HEIGHT/1) / TILE_HEIGHT;

        collision_value_1 = board_list[board_index].collision[min(check_x1, BOARD_TILE_WIDTH - 1)][min(check_y1, BOARD_TILE_HEIGHT - 1)];
        collision_value_2 = board_list[board_index].collision[min(check_x2, BOARD_TILE_WIDTH - 1)][min(check_y2, BOARD_TILE_HEIGHT - 1)];
        collision_value_3 = board_list[board_index].collision[min(check_x3, BOARD_TILE_WIDTH - 1)][min(check_y3, BOARD_TILE_HEIGHT - 1)];
        collision_value_4 = board_list[board_index].collision[min(check_x4, BOARD_TILE_WIDTH - 1)][min(check_y4, BOARD_TILE_HEIGHT - 1)];

        // correct collision along other axis (if needed)
        if((collision_value_1 == 5 && collision_value_3 == 5) || (collision_value_2 == 5 && collision_value_4 == 5))
            phys->x -= phys->x_vel*phys->speed; // correct along x-axis
        else if((collision_value_1 == 5 && collision_value_2 == 5) || (collision_value_3 == 5 && collision_value_4 == 5))
            phys->y -= phys->y_vel*phys->speed; // correct along y-axis
    }
    else if(collision_value_1 == 3 || collision_value_2 == 3 || collision_value_3 == 3 || collision_value_4 == 3)
    {
        if(phys->z == 0.0f)
        {
            // handle mud
            if(phys->x_vel != 0 || phys->y_vel != 0)
                spawn_particle(rand() % TILE_WIDTH + phys->x,phys->y+TILE_HEIGHT,2,1,0,4,board_index);

            phys->speed = phys->base_speed/2.0f;
        }
    }
    else if(collision_value_1 == 4 || collision_value_2 == 4 || collision_value_3 == 4 || collision_value_4 == 4)
    {
        // handle water
		if (phys->z == 0.0f)
        {
            if(phys->x_vel != 0 || phys->y_vel != 0)
            {
                spawn_particle(rand() % TILE_WIDTH + phys->x,phys->y+TILE_HEIGHT,2,2,0,8,board_index);
            }

            phys->speed = phys->base_speed/3.0f;
        }
    }
    else if(collision_value_1 == 6 || collision_value_2 == 6 || collision_value_3 == 6 || collision_value_4 == 6)
    {
        if(phys->z == 0.0f)
        {
            // handle lava
            if(phys->x_vel != 0 || phys->y_vel != 0)
                spawn_particle(rand() % TILE_WIDTH + phys->x,phys->y+TILE_HEIGHT,2,2,0,6,board_index);

            phys->environmental_hurt_counter++;
            if(phys->environmental_hurt_counter >= phys->environmental_hurt_max)
            {
                phys->environmental_hurt_counter = 0;
                phys->hp--;
                spawn_floating_number(phys->x+TILE_WIDTH/2,phys->y,1,6);
            }
            phys->speed = phys->base_speed/3.0f;
        }
    }
    else if(phys->z == 0.0f)
        phys->speed = phys->base_speed;
    
}

static void init_board()
{
    current_board_index = get_board_index_by_name(player.board_name);
}

static void load_board_map()
{
    FILE* fp_map = fopen("data\\boards\\board_map","r");

	if (fp_map == NULL)
		return;

    int c;
    int  board_name_counter = 0;
    char board_name[100] = {0};

    int x_index = 0;
    int y_index = 0;
    do
    {
        c = fgetc(fp_map);

        if(c == '0')
        {
            // ignore
        }
        else if(c == ',')
        {
            // make sure board_name is not nothing
            if(board_name[0] != '\0')
            {
                int board_index = get_board_index_by_name(board_name);
				if (board_index >= 0)
				{
					board_list[board_index].map_x_index = x_index;
					board_list[board_index].map_y_index = y_index;
				}
				memset(board_name, 0, 100);
				board_name_counter = 0;
            }
			++x_index;
        }
        else if(c == '\n' || c == EOF)
        {
			if (board_name[0] != '\0')
			{
				int board_index = get_board_index_by_name(board_name);
				if (board_index >= 0)
				{
					board_list[board_index].map_x_index = x_index;
					board_list[board_index].map_y_index = y_index;
				}

				memset(board_name, 0, 100);
				board_name_counter = 0;
			}

			++y_index;
            x_index = 0;
        }
        else
            board_name[board_name_counter++] = c;

        

    } while (c != EOF);

    fclose(fp_map);
}

static void generate_indexed_board(const char* rgb_image_path,const char* indexed_path)
{
    int w,h,n;
    unsigned char *imgdata = stbi_load(rgb_image_path,&w,&h,&n,0);

    if(imgdata == NULL)
        return;

    unsigned char *p = imgdata;

    FILE * fp_board;
    fp_board = fopen (indexed_path, "wb");

	unsigned char r, g, b;
	int val;

    for(int j = 0; j < h; ++j)
    {
		for (int i = 0; i < w; ++i)
		{
			r = *(p + 0);
			g = *(p + 1);
			b = *(p + 2);

			val = 0xFF;

			if (r == 0 && g == 175 && b == 0) // grass
				val = GRASS;
			else if (r == 0 && g == 100 && b == 0) // marsh
				val = MARSH;
			else if (r == 0 && g == 150 && b == 175) // water 
				val = WATER;
			else if (r == 255 && g == 200 && b == 0) // sand
				val = SAND;
			else if (r == 100 && g == 75 && b == 50) // mud
				val = MUD;
            else if (r == 100 && g == 100 && b == 100) // mountain
                val = MOUNTAIN;
            else if (r == 255 && g == 255 && b == 255) // snow
				val = SNOW;
            else if (r == 255 && g == 0 && b == 0)    // lava
				val = LAVA;
            else if (r == 175 && g == 175 && b == 175)   // stone 
				val = STONE;
            else if (r == 200 && g == 150 && b == 64)   // wood 
				val = WOOD;
                
			if (fputc(val, fp_board) == EOF)
				return;

			p += n;
		}
    }
    
    fclose(fp_board);
    stbi_image_free(imgdata);
}

static void generate_all_boards()
{
	char paths[100][MAX_PATH] = { 0 };
	int num_files = get_files_in_directory_with_extension("data\\boards", ".board.png", paths);

    for(int i = 0; i < num_files; ++i)
    {
		char index_path[MAX_PATH] = {0};

        remove_file_extension(paths[i], index_path);
        generate_indexed_board(paths[i],index_path);
    }
}

static void load_board(const char* path_to_board_file, int board_index)
{
    FILE* fp_board = fopen(path_to_board_file,"rb");

	if (fp_board == NULL)
		return;

    int c;
	unsigned char uc;

	for (int j = 0; j < BOARD_TILE_HEIGHT; ++j)
	{
		for(int i = 0; i < BOARD_TILE_WIDTH; ++i)
		{
            c = fgetc(fp_board);
            if(c == EOF) return;

			uc = (unsigned char)c;

            board_list[board_index].data[j][i] = uc;

            switch(uc)
            {
                case GRASS: board_list[board_index].collision[i][j] = 1; break;
                case MARSH: board_list[board_index].collision[i][j] = 1; break;
                case WOOD:  board_list[board_index].collision[i][j] = 1; break;
                case SAND:  board_list[board_index].collision[i][j] = 2; break;
                case MUD:   board_list[board_index].collision[i][j] = 3; break;
                case WATER: board_list[board_index].collision[i][j] = 4; break;
                case LAVA:  board_list[board_index].collision[i][j] = 6; break;
                case MOUNTAIN: board_list[board_index].collision[i][j] = 5; break;
                case STONE: board_list[board_index].collision[i][j] = 5; break;
            }
        }
    }

    fclose(fp_board);
}

static void load_all_boards()
{
	char paths[100][MAX_PATH] = { 0 };
	int num_files = get_files_in_directory_with_extension("data\\boards", ".board", paths);

    for(int i = 0; i < num_files; ++i)
    {
        char path_without_ext[MAX_PATH] = {0};
        char board_name[MAX_PATH] = {0};

        remove_file_extension(paths[i], path_without_ext);
        get_file_name(path_without_ext,board_name);

        C_strcpy(board_name, board_list[num_boards].name);
        load_board(paths[i],num_boards);

        num_boards++;
    }
}

static void update_board(int index)
{
    world_water_anim_counter++;

	if (world_water_anim_counter == world_water_anim_counter_max)
	{
		world_water_anim_counter = 0;

		for(int j = 0; j < BOARD_TILE_HEIGHT; ++j)
		{
			for(int i = 0; i < BOARD_TILE_WIDTH; ++i)
			{
				if(board_list[index].data[i][j] == WATER || board_list[index].data[i][j] == WATER2)
				{
					// animate water
                    if(board_list[index].data[i][j] == WATER) 
                        board_list[index].data[i][j] = WATER2;
                    else
                        board_list[index].data[i][j] = WATER;
                }
				if(board_list[index].data[i][j] == LAVA || board_list[index].data[i][j] == LAVA2)
                {
					// animate lava
                    if(board_list[index].data[i][j] == LAVA) 
                        board_list[index].data[i][j] = LAVA2;
                    else
                        board_list[index].data[i][j] = LAVA;
                }    
            }
        }
    }
    
    // update day cycle
    day_cycle_counter += day_cycle_direction;

    if(day_cycle_counter < DAY_CYCLE_DAYTIME)
    {
        // day time
        day_cycle_shade_amount = 0;
    }
    else if(day_cycle_counter > DAY_CYCLE_NIGHTTIME)
    {
        // night time
        day_cycle_shade_amount = 10;
    }
    else
    {
        // transition
        // 3600 - 10800 = 7200 frames
        // 0 - 7200
        day_cycle_shade_amount = 10.0f*((float)(day_cycle_counter - DAY_CYCLE_DAYTIME)/(DAY_CYCLE_NIGHTTIME - DAY_CYCLE_DAYTIME));
    }

    if(day_cycle_counter == DAY_CYCLE_COUNTER_MAX || day_cycle_counter == 0)
        day_cycle_direction *= -1;

}
static void draw_board(int index)
{
    int start_x = camera.x / TILE_WIDTH;
    int start_y = camera.y / TILE_HEIGHT;

    int end_x = start_x + (buffer_width/TILE_WIDTH) + 1;
    int end_y = start_y + (buffer_height/TILE_HEIGHT) + 1;

    if (end_x > BOARD_TILE_WIDTH)  end_x = BOARD_TILE_WIDTH;
    if (end_y > BOARD_TILE_HEIGHT) end_y = BOARD_TILE_HEIGHT;

    for(int j = start_y; j < end_y; ++j)
    {
        for(int i = start_x; i < end_x; ++i)
        {
            if(board_list[index].data[j][i] == LAVA || board_list[index].data[j][i] == LAVA2)
                draw_tile(i*TILE_WIDTH - camera.x,j*TILE_HEIGHT - camera.y,terrain_tileset_name, board_list[index].data[j][i],0); // don't shade lava tiles
            else
                draw_tile(i*TILE_WIDTH - camera.x,j*TILE_HEIGHT - camera.y,terrain_tileset_name, board_list[index].data[j][i],day_cycle_shade_amount);
        }
    }
}

static void draw_message()
{
    // 240, 176
    for(int j = buffer_height - 28; j < buffer_height -7; ++j)
    {
        for(int i = 40; i < buffer_width - 40; ++i)
        {
            shade_pixel8(i,j,10);
        }
    } 

    draw_string(message.name,42, 149,1.0f,8);
    draw_string(message.message,50,156,1.0f,message.color);
}
