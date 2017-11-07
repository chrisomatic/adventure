#define WORLD_TILE_WIDTH  256
#define WORLD_TILE_HEIGHT 256
#define GRAVITY 0.2f;
#define AIR_RESISTANCE 0.01f;
#define GROUND_FRICTION 0.1f;
#define DAY_CYCLE_COUNTER_MAX 14400 
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
    char* name;
    char* description;
    float x;
    float y;
    float z;
    float x_vel;
    float y_vel;
    float z_vel;
    float friction;
    char* tileset_name;
    int  tile_index;
    int  value;
    BOOL highlighted;
    BOOL consummable;
    ItemType type;
    WeaponProperties weapon_props;
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
    char* tileset_name;
    int tile_index;
    int lvl;
    float xp;
    int hp;
    int max_hp;
    int mp;
    int max_mp;
    float x;
    float y;
    float z;
    float x_vel;
    float y_vel;
    float z_vel;
    int gold;
    int coin_throw_counter;
    int coin_throw_max;
    int environmental_hurt_counter;
    int environmental_hurt_max;
    int bounce_counter;
    int bounce_counter_max;
    float base_speed;
    float speed;
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
    //Weapon weapon;
    Item weapon;
    Direction dir;
    AttackDirection attack_dir;
    Animation anim;
    PlayerState state;
} Player;

Player player;

int foes_killed = 0; // @TEMP
long next_level = 10;

static void gain_level()
{
    player.lvl++;
    player.xp -= next_level;
    next_level *= 2.00f;

    spawn_floating_string(player.x + TILE_WIDTH/2, player.y,"+Lvl",8);
    for(int i = 0; i < 10; ++i)
        spawn_particle(rand() % TILE_WIDTH + player.x,player.y,2,5,'*',8);
}

static int world[WORLD_TILE_HEIGHT][WORLD_TILE_WIDTH];
static int world_collision[WORLD_TILE_HEIGHT][WORLD_TILE_WIDTH];

static int world_water_anim_counter = 0;
static int world_water_anim_counter_max = 5;

static int day_cycle_counter = 0;
static int day_cycle_direction = 1;
static int day_cycle_shade_amount = 0;

const char* terrain_tileset_name = "terrain";

static void generate_world_file(const char* image_path)
{
    int w,h,n;
    unsigned char *imgdata = stbi_load(image_path,&w,&h,&n,0);

    if(imgdata == NULL)
        return;

    unsigned char *p = imgdata;

    FILE * fp_world;
    fp_world = fopen ("data\\world", "wb");

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
                
			if (fputc(val, fp_world) == EOF)
				return;

			p += n;
		}
    }
    
    fclose(fp_world);
    stbi_image_free(imgdata);
}

static void init_world(const char* path_to_world_file)
{
    FILE* fp_world = fopen(path_to_world_file,"rb");

	if (fp_world == NULL)
		return;

    int c;
	unsigned char uc;

	for (int j = 0; j < WORLD_TILE_HEIGHT; ++j)
	{
		for(int i = 0; i < WORLD_TILE_WIDTH; ++i)
		{
            c = fgetc(fp_world);
            if(c == EOF) return;

			uc = (unsigned char)c;

			world[j][i] = uc;

            switch(uc)
            {
                case GRASS: world_collision[i][j] = 1; break;
                case MARSH: world_collision[i][j] = 1; break;
                case WOOD:  world_collision[i][j] = 1; break;
                case SAND:  world_collision[i][j] = 2; break;
                case MUD:   world_collision[i][j] = 3; break;
                case WATER: world_collision[i][j] = 4; break;
                case LAVA:  world_collision[i][j] = 6; break;
                case MOUNTAIN: world_collision[i][j] = 5; break;
                case STONE: world_collision[i][j] = 5; break;
            }
        }
    }

    fclose(fp_world);
}

static void update_world()
{
    world_water_anim_counter++;

	if (world_water_anim_counter == world_water_anim_counter_max)
	{
		world_water_anim_counter = 0;
		for(int j = 0; j < WORLD_TILE_HEIGHT; ++j)
		{
			for(int i = 0; i < WORLD_TILE_WIDTH; ++i)
			{
				if(world[i][j] == WATER || world[i][j] == WATER2)
				{
					// animate water
                    if(world[i][j] == WATER) 
                        world[i][j] = WATER2;
                    else
                        world[i][j] = WATER;
                }
				if(world[i][j] == LAVA || world[i][j] == LAVA2)
                {
					// animate lava
                    if(world[i][j] == LAVA) 
                        world[i][j] = LAVA2;
                    else
                        world[i][j] = LAVA;
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
static void draw_world()
{
    int start_x = camera.x / TILE_WIDTH;
    int start_y = camera.y / TILE_HEIGHT;

    int end_x = start_x + (buffer_width/TILE_WIDTH) + 1;
    int end_y = start_y + (buffer_height/TILE_HEIGHT) + 1;

    if (end_x > WORLD_TILE_WIDTH)  end_x = WORLD_TILE_WIDTH;
    if (end_y > WORLD_TILE_HEIGHT) end_y = WORLD_TILE_HEIGHT;

    for(int j = start_y; j < end_y; ++j)
    {
        for(int i = start_x; i < end_x; ++i)
        {
            if(world[j][i] == LAVA || world[j][i] == LAVA2)
                draw_tile(i*TILE_WIDTH - camera.x,j*TILE_HEIGHT - camera.y,terrain_tileset_name, world[j][i],0); // don't shade lava tiles
            else
                draw_tile(i*TILE_WIDTH - camera.x,j*TILE_HEIGHT - camera.y,terrain_tileset_name, world[j][i],day_cycle_shade_amount);
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

