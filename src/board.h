#pragma once

#define BOARD_TILE_WIDTH  256
#define BOARD_TILE_HEIGHT 256
#define GRAVITY 0.2f;
#define AIR_RESISTANCE 0.01f;
#define GROUND_FRICTION 0.1f;
#define DAY_CYCLE_COUNTER_MAX 14400 
#define SECONDS_PER_DAY (DAY_CYCLE_COUNTER_MAX / TARGET_FPS)
#define DAY_CYCLE_DAYTIME 3600 
#define DAY_CYCLE_NIGHTTIME 10800
#define MAX_NPCS     1000
#define MAX_NPC_LIST 1000

#define GRASS        0
#define MARSH        1
#define WATER        2
#define WATER2       3
#define SAND         4
#define MUD          5
#define MOUNTAIN     6
#define SNOW         7
#define LAVA         8
#define LAVA2        9
#define STONE       10 
#define WOOD        11
#define WATER_DEEP  12
#define WATER_DEEP2 13
#define CAVE        14
#define DIRT        15
#define BRICK       16

#define ARROW 4
#define FIREBALL 6
#define POISON_SPIT 13

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
	int defence;
	ArmorType armor_type;
	int armor_y_offset;
} ArmorProperties;

typedef struct
{
	float x;
	float y;
	float z;
	int   x_offset;
	int   y_offset;
	float x_vel;
	float y_vel;
	float z_vel;
	float base_speed;
	float speed;
	int height;
	int width;
	int length;
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
	int  coin_value;
	float mount_float_angle;
	BOOL highlighted;
	BOOL consummable;
	BOOL mounted;
	int  vendor_index;
	ItemType type;
	WeaponProperties weapon_props;
	ArmorProperties  armor_props;
	PhysicalProperties phys;
} Item;

typedef enum
{
	KEYPRESS_NONE = 0,
	KEYPRESS_UP = 1,
	KEYPRESS_DOWN = 2,
	KEYPRESS_LEFT = 4,
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
	PLAYER_STATE_NONE = 0,
	PLAYER_STATE_MOVE = 1,
	PLAYER_STATE_ATTACK = 2,
	PLAYER_STATE_HURT = 4,
	PLAYER_STATE_MIDAIR = 8,
	PLAYER_STATE_NOTCHED = 16,
	PLAYER_STATE_DEAD = 32
} PlayerState;

typedef struct
{
	int strength;
	int dexterity;
	int vitality;
	int energy;
} Stats;

char tileset_map[16][100] = { 0 };
int tileset_num = 0;
int tileset_collisions[16][256] = { 1 };

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
	int available_stat_points;
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
	BOOL unequip;
	int item_held_index;
	int notch_index;
	int hurt_counter;
	int hurt_counter_max;
	Item weapon;
	Item armor_head;
	Item armor_body;
	Item armor_hands;
	Item armor_feet;
	Stats stats;
	Direction dir;
	AttackDirection attack_dir;
	Animation anim;
	PlayerState state;
} Player;

Player player;

int foes_killed = 0; // @TEMP
long next_level = 100;

BOOL display_board_title = FALSE;
int board_title_display_counter = 0;

typedef enum
{
	CREATURE_STATE_NEUTRAL,
	CREATURE_STATE_ACTING
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

#define MAX_CREATURES       100000
#define MAX_CREATURE_GRID   1000
#define MAX_CREATURE_LIST   100
#define CREATURE_GRID_X_MAX 16 
#define CREATURE_GRID_Y_MAX 16

typedef struct
{
	float distance_from_player;
	int talk_radius;
	int num_dialogue;
	int selected_dialogue_num;
	int vendor_credit;
	char* dialogue[10];
	BOOL talking;
	BOOL is_vendor;
	BOOL is_npc;
} NPCProperties;

typedef struct
{
	char* name;
	char* board_name;
	int   board_index;
	char* species;
	char* tileset_name;
	int tile_index;
	int xp;
	Item weapon;
	Item armor_head;
	Item armor_body;
	Item armor_hands;
	Item armor_feet;
	PhysicalProperties phys;
	NPCProperties npc_props;
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
	BOOL is_npc;
	BOOL reproductive;
	BOOL pregnant;
	BOOL birth_recovery;
	BOOL attacking;
	BOOL stunned;
	BOOL deaggress;
	BOOL attack_recovery;
	float attack_angle;
	int gestation_period;
	int gestation_counter;
	int mating_radius;
	int grouping_radius;
	int age;
	int max_age;
	int age_counter;
	int adult_age;
	int litter_max;
	int deaggression_counter;
	int deaggression_duration;
	int birth_recovery_time;
	int birth_recovery_counter;
	int death_check_counter;
	int grid_index_x;
	int grid_index_y;
	Zone zone;
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
int npc_creature_indices[MAX_NPCS];
int num_npcs = 0;

static int current_board_index = 0;

typedef struct
{
	char name[100];
	int map_x_index;
	int map_y_index;
	unsigned int data[BOARD_TILE_HEIGHT][BOARD_TILE_WIDTH];
	unsigned int tileset_data[BOARD_TILE_HEIGHT][BOARD_TILE_WIDTH];
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

	for (int i = 0; i < num_boards; ++i)
	{
		if (strcmp(board_list[i].name, name) == 0)
			return i;
	}

	return -1;
}

static int get_name_of_board_location(int x, int y, char* ret_name)
{
	if (x < 0 || y < 0)
		return -1;

	for (int i = 0; i < num_boards; ++i)
	{
		if (board_list[i].map_x_index == x && board_list[i].map_y_index == y)
		{
			ret_name = board_list[i].name;
			return i;
		}
	}

	return -1;

}

static BOOL is_colliding_with_solid_terrain(int board_index, PhysicalProperties* phys)
{
	int check_x1, check_y1, check_x2, check_y2, check_x3, check_y3, check_x4, check_y4;
	int collision_value_1, collision_value_2, collision_value_3, collision_value_4;

	// check collision
	check_x1 = (phys->x + phys->x_offset) / TILE_WIDTH; check_y1 = (phys->y + phys->y_offset) / TILE_HEIGHT;
	check_x2 = (phys->x + phys->x_offset + phys->width) / TILE_WIDTH; check_y2 = (phys->y + phys->y_offset) / TILE_HEIGHT;
	check_x3 = (phys->x + phys->x_offset) / TILE_WIDTH; check_y3 = (phys->y + phys->y_offset + phys->length) / TILE_HEIGHT;
	check_x4 = (phys->x + phys->x_offset + phys->width) / TILE_WIDTH; check_y4 = (phys->y + phys->y_offset + phys->length) / TILE_HEIGHT;

	collision_value_1 = board_list[board_index].collision[min(check_x1, BOARD_TILE_WIDTH - 1)][min(check_y1, BOARD_TILE_HEIGHT - 1)];
	collision_value_2 = board_list[board_index].collision[min(check_x2, BOARD_TILE_WIDTH - 1)][min(check_y2, BOARD_TILE_HEIGHT - 1)];
	collision_value_3 = board_list[board_index].collision[min(check_x3, BOARD_TILE_WIDTH - 1)][min(check_y3, BOARD_TILE_HEIGHT - 1)];
	collision_value_4 = board_list[board_index].collision[min(check_x4, BOARD_TILE_WIDTH - 1)][min(check_y4, BOARD_TILE_HEIGHT - 1)];

	return (collision_value_1 == 5 || collision_value_2 == 5 || collision_value_3 == 5 || collision_value_4 == 5);
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
	if (phys->x_vel == 0.0f && phys->y_vel == 0.0f)
		return;

	int check_x1, check_y1, check_x2, check_y2, check_x3, check_y3, check_x4, check_y4;
	int collision_value_1, collision_value_2, collision_value_3, collision_value_4;

	for (int i = 0; i < 2; ++i)
	{
		// check collision
		check_x1 = (phys->x + phys->x_offset) / TILE_WIDTH; check_y1 = (phys->y + phys->y_offset) / TILE_HEIGHT;
		check_x2 = (phys->x + phys->x_offset + phys->width) / TILE_WIDTH; check_y2 = (phys->y + phys->y_offset) / TILE_HEIGHT;
		check_x3 = (phys->x + phys->x_offset) / TILE_WIDTH; check_y3 = (phys->y + phys->y_offset + phys->length) / TILE_HEIGHT;
		check_x4 = (phys->x + phys->x_offset + phys->width) / TILE_WIDTH; check_y4 = (phys->y + phys->y_offset + phys->length) / TILE_HEIGHT;

		collision_value_1 = board_list[board_index].collision[min(check_x1, BOARD_TILE_WIDTH - 1)][min(check_y1, BOARD_TILE_HEIGHT - 1)];
		collision_value_2 = board_list[board_index].collision[min(check_x2, BOARD_TILE_WIDTH - 1)][min(check_y2, BOARD_TILE_HEIGHT - 1)];
		collision_value_3 = board_list[board_index].collision[min(check_x3, BOARD_TILE_WIDTH - 1)][min(check_y3, BOARD_TILE_HEIGHT - 1)];
		collision_value_4 = board_list[board_index].collision[min(check_x4, BOARD_TILE_WIDTH - 1)][min(check_y4, BOARD_TILE_HEIGHT - 1)];

		if (collision_value_1 == 5 || collision_value_2 == 5 || collision_value_3 == 5 || collision_value_4 == 5)
		{
			if ((collision_value_1 == 5 && collision_value_3 == 5) || (collision_value_2 == 5 && collision_value_4 == 5))
				phys->x -= phys->x_vel*phys->speed; // correct along x-axis
			else if ((collision_value_1 == 5 && collision_value_2 == 5) || (collision_value_3 == 5 && collision_value_4 == 5))
				phys->y -= phys->y_vel*phys->speed; // correct along y-axis
			else if (collision_value_1 == 5)
			{
				if (phys->y_vel != 0.0f) phys->x += 1.0f*phys->speed;
				if (phys->x_vel != 0.0f) phys->y += 1.0f*phys->speed;
			}
			else if (collision_value_2 == 5)
			{
				if (phys->y_vel != 0.0f) phys->x -= 1.0f*phys->speed;
				if (phys->x_vel != 0.0f) phys->y += 1.0f*phys->speed;
			}
			else if (collision_value_3 == 5)
			{
				if (phys->y_vel != 0.0f) phys->x += 1.0f*phys->speed;
				if (phys->x_vel != 0.0f) phys->y -= 1.0f*phys->speed;
			}
			else if (collision_value_4 == 5)
			{
				if (phys->y_vel != 0.0f) phys->x -= 1.0f*phys->speed;
				if (phys->x_vel != 0.0f) phys->y -= 1.0f*phys->speed;
			}
		}
	}

	if (collision_value_1 == 3 || collision_value_2 == 3 || collision_value_3 == 3 || collision_value_4 == 3)
	{
		if (phys->z == 0.0f)
		{
			// handle mud
			if (phys->x_vel != 0 || phys->y_vel != 0)
				spawn_particle(rand() % TILE_WIDTH + phys->x, phys->y + TILE_HEIGHT, 2, 1, 0, 4, board_index);

			phys->speed = phys->base_speed / 2.0f;
		}
	}
	else if (collision_value_1 == 4 || collision_value_2 == 4 || collision_value_3 == 4 || collision_value_4 == 4)
	{
		// handle water
		if (phys->z == 0.0f)
		{
			if (phys->x_vel != 0 || phys->y_vel != 0)
			{
				spawn_particle(rand() % TILE_WIDTH + phys->x, phys->y + TILE_HEIGHT, 2, 2, 0, 8, board_index);
			}

			phys->speed = phys->base_speed / 3.0f;
		}
	}
	else if (collision_value_1 == 6 || collision_value_2 == 6 || collision_value_3 == 6 || collision_value_4 == 6)
	{
		if (phys->z == 0.0f)
		{
			// handle lava
			if (phys->x_vel != 0 || phys->y_vel != 0)
				spawn_particle(rand() % TILE_WIDTH + phys->x, phys->y + TILE_HEIGHT, 2, 2, 0, 6, board_index);

			phys->environmental_hurt_counter++;
			if (phys->environmental_hurt_counter >= phys->environmental_hurt_max)
			{
				phys->environmental_hurt_counter = 0;
				phys->hp--;
				spawn_floating_number(phys->x + TILE_WIDTH / 2, phys->y, 1, 6, board_index);
			}
			phys->speed = phys->base_speed / 3.0f;
		}
	}
	else if (phys->z == 0.0f)
		phys->speed = phys->base_speed;

}

static void init_boards()
{
	current_board_index = get_board_index_by_name(player.board_name);
	display_board_title = TRUE;
}

static void load_board_map()
{
	FILE* fp_map = fopen("data\\boards\\board_map", "r");

	if (fp_map == NULL)
		return;

	int c;
	int  board_name_counter = 0;
	char board_name[100] = { 0 };

	int x_index = 0;
	int y_index = 0;
	do
	{
		c = fgetc(fp_map);

		if (c == '0')
		{
			// ignore
		}
		else if (c == ',')
		{
			// make sure board_name is not nothing
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
			++x_index;
		}
		else if (c == '\n' || c == EOF)
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


//not used anymore
static void generate_indexed_board(const char* rgb_image_path, const char* indexed_path)
{
	int w, h, n;
	unsigned char *imgdata = stbi_load(rgb_image_path, &w, &h, &n, 0);

	if (imgdata == NULL)
		return;

	unsigned char *p = imgdata;

	FILE * fp_board;
	fp_board = fopen(indexed_path, "wb");

	unsigned char r, g, b;
	int val;

	for (int j = 0; j < h; ++j)
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
			else if (r == 0 && g == 50 && b == 150) // deep water 
				val = WATER_DEEP;
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
			else if (r == 0 && g == 0 && b == 0)   // cave
				val = CAVE;

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

	for (int i = 0; i < num_files; ++i)
	{
		char index_path[MAX_PATH] = { 0 };

		remove_file_extension(paths[i], index_path);
		generate_indexed_board(paths[i], index_path);
	}
}



static void load_board(const char* path_to_board_file, int board_index)
{
	FILE* fp_board = fopen(path_to_board_file, "rb");

	if (fp_board == NULL)
		return;

	// find :data section
	char s[1000] = { 0 };
	int key = 0;
	char value[100] = { 0 };


	int current_section = 0;

	for (;;) {

		if (fgets(s, 1000, fp_board) == NULL)
			break;

		if (str_contains(s, ":board_info")) {
			current_section = 1;
			continue;
		}
		else if (str_contains(s, ":tileset_info")) {
			current_section = 2;
			continue;
		}
		else if (str_contains(s, ":data")) {
			current_section = 3;
		}

		switch (current_section) {
		case 1: {
			// board info
		} break;
		case 2: {
			// tileset info
			int matches = sscanf(s, "%d=%s\n", &key, value);
			str_replace(value, 100, ".tileset.png", "", value);

			char path_to_collision[100];
			strncpy(path_to_collision, "data\\tilesets\\", 100);
			strcat(path_to_collision, value, 100);
			strcat(path_to_collision, ".tileset.collision");


			//some stuff I tried
			//parse the collision file to get the array of values
			if (matches == 2) {
				//tileset_collisions[key] = load_tile_collisions(path_to_collision);
				//int collisions[256] = load_tile_collisions(path_to_collision);
				//int i;
				//for (i = 0; i < 256; i++) {
				//	tileset_collisions[key][i] = collisions[i];
				//}

			}

			//load_tile_collisions(path_to_collision);






			if (matches == 2) {
				switch (key) {
				case 0:  strncpy(tileset_map[0], value, 100);  break;
				case 1:  strncpy(tileset_map[1], value, 100);  break;
				case 2:  strncpy(tileset_map[2], value, 100);  break;
				case 3:  strncpy(tileset_map[3], value, 100);  break;
				case 4:  strncpy(tileset_map[4], value, 100);  break;
				case 5:  strncpy(tileset_map[5], value, 100);  break;
				case 6:  strncpy(tileset_map[6], value, 100);  break;
				case 7:  strncpy(tileset_map[7], value, 100);  break;
				case 8:  strncpy(tileset_map[8], value, 100);  break;
				case 9:  strncpy(tileset_map[9], value, 100);  break;
				case 10: strncpy(tileset_map[10], value, 100); break;
				case 11: strncpy(tileset_map[11], value, 100); break;
				case 12: strncpy(tileset_map[12], value, 100); break;
				case 13: strncpy(tileset_map[13], value, 100); break;
				case 14: strncpy(tileset_map[14], value, 100); break;
				case 15: strncpy(tileset_map[15], value, 100); break;
				}
			}
		} break;
		case 3: {
			// data
			for (int j = 0; j < BOARD_TILE_HEIGHT; ++j)
			{
				for (int i = 0; i < BOARD_TILE_WIDTH; ++i)
				{
					int tileset_index = fgetc(fp_board);
					int tile_index = fgetc(fp_board);

					if (tile_index == EOF || tileset_index == EOF)
						break;

					board_list[board_index].data[j][i] = tile_index;
					board_list[board_index].tileset_data[j][i] = tileset_index;


					board_list[board_index].collision[i][j] = tileset_collisions[tileset_index][tile_index];

					if (FALSE) {
						if (strcmp(tileset_map[tileset_index], "terrain") == 0) {
							switch (tile_index)
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
							case WATER_DEEP: board_list[board_index].collision[i][j] = 5; break;
							case CAVE: board_list[board_index].collision[i][j] = 1; break;
							case DIRT: board_list[board_index].collision[i][j] = 1; break;
							case BRICK: board_list[board_index].collision[i][j] = 5; break;
							default: board_list[board_index].collision[i][j] = 1; break;
							}
						}

						else
							board_list[board_index].collision[i][j] = 1;
					}
				}
			}
		} break;
		}
	}


	board_list[board_index].map_x_index = -1;
	board_list[board_index].map_y_index = -1;

	fclose(fp_board);
}

// can't return arrays
static void load_tile_collisions(const char* path_to_collision_file)
{
	// default
	int collisions[256] = { 1 };

	FILE* fp_collisions = fopen(path_to_collision_file, "r");

	if (fp_collisions == NULL)
		return collisions;


	int c;
	char collisions_number[100] = { 0 };
	int i = 0;
	BOOL gteol = FALSE;

	do
	{
		c = fgetc(fp_collisions);

		if (c == '#')
		{
			// go to end of line or end of file
			gteol = TRUE;
			continue;

		}
		else if (c == ',' || c == '\n' || c == EOF)
		{
			if (c == '\n' & gteol)
			{
				gteol = FALSE;
				continue;
				memset(collisions_number, 0, 100);
			}

			//make sure collisions_number is not nothing
			if (collisions_number[0] != '\0')
			{
				int x = collisions_number;
				collisions[i] = x;
				++i;
				memset(collisions_number, 0, 100);
			}
		}




	} while (c != EOF);

	fclose(fp_collisions);
	return collisions;

}


static void load_all_boards()
{
	char paths[100][MAX_PATH] = { 0 };
	int num_files = get_files_in_directory_with_extension("data\\boards", ".board", paths);

	for (int i = 0; i < num_files; ++i)
	{
		char path_without_ext[MAX_PATH] = { 0 };
		char board_name[MAX_PATH] = { 0 };

		remove_file_extension(paths[i], path_without_ext);
		get_file_name(path_without_ext, board_name);

		C_strcpy(board_name, board_list[num_boards].name);
		load_board(paths[i], num_boards);

		num_boards++;
	}
}

static void update_board(int index)
{
	world_water_anim_counter++;

	if (world_water_anim_counter == world_water_anim_counter_max)
	{
		world_water_anim_counter = 0;

		for (int j = 0; j < BOARD_TILE_HEIGHT; ++j)
		{
			for (int i = 0; i < BOARD_TILE_WIDTH; ++i)
			{
				if (board_list[index].data[i][j] == WATER || board_list[index].data[i][j] == WATER2)
				{
					// animate water
					if (board_list[index].data[i][j] == WATER)
						board_list[index].data[i][j] = WATER2;
					else
						board_list[index].data[i][j] = WATER;
				}
				else if (board_list[index].data[i][j] == LAVA || board_list[index].data[i][j] == LAVA2)
				{
					// animate lava
					if (board_list[index].data[i][j] == LAVA)
						board_list[index].data[i][j] = LAVA2;
					else
						board_list[index].data[i][j] = LAVA;
				}
				else if (board_list[index].data[i][j] == WATER_DEEP || board_list[index].data[i][j] == WATER_DEEP2)
				{
					// animate water
					if (board_list[index].data[i][j] == WATER_DEEP)
						board_list[index].data[i][j] = WATER_DEEP2;
					else
						board_list[index].data[i][j] = WATER_DEEP;
				}
			}
		}
	}

	// update day cycle
	day_cycle_counter += day_cycle_direction;

	if (day_cycle_counter < DAY_CYCLE_DAYTIME)
	{
		// day time
		day_cycle_shade_amount = 0;
	}
	else if (day_cycle_counter > DAY_CYCLE_NIGHTTIME)
	{
		// night time
		day_cycle_shade_amount = 10;
	}
	else
	{
		// transition
		// 3600 - 10800 = 7200 frames
		// 0 - 7200
		day_cycle_shade_amount = 10.0f*((float)(day_cycle_counter - DAY_CYCLE_DAYTIME) / (DAY_CYCLE_NIGHTTIME - DAY_CYCLE_DAYTIME));
	}

	if (day_cycle_counter == DAY_CYCLE_COUNTER_MAX || day_cycle_counter == 0)
		day_cycle_direction *= -1;

}
static void draw_board(int index)
{
	int start_x = camera.x / TILE_WIDTH;
	int start_y = camera.y / TILE_HEIGHT;

	int end_x = start_x + (buffer_width / TILE_WIDTH) + 1;
	int end_y = start_y + (buffer_height / TILE_HEIGHT) + 1;

	if (end_x > BOARD_TILE_WIDTH)  end_x = BOARD_TILE_WIDTH;
	if (end_y > BOARD_TILE_HEIGHT) end_y = BOARD_TILE_HEIGHT;

	for (int j = start_y; j < end_y; ++j)
	{
		for (int i = start_x; i < end_x; ++i)
		{
			if (board_list[index].data[j][i] == LAVA || board_list[index].data[j][i] == LAVA2)
				draw_tile(i*TILE_WIDTH - camera.x, j*TILE_HEIGHT - camera.y, tileset_map[board_list[index].tileset_data[j][i]], board_list[index].data[j][i], 0); // don't shade lava tiles
			else
				draw_tile(i*TILE_WIDTH - camera.x, j*TILE_HEIGHT - camera.y, tileset_map[board_list[index].tileset_data[j][i]], board_list[index].data[j][i], day_cycle_shade_amount);
		}
	}
}

