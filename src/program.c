#define WIN32_LEAN_AND_MEAN
#define PI 3.14159265359 
#define NUM_RATS 10000
#define MAX_FLOATING_NUMBERS 1000

#include <windows.h>
#include <math.h>     // for abs,atan,cos,sin
#include <time.h>     // for time function
#include "common.h"
#include "cdraw.h"
#include "cutil.h"
#include "cfontsimple.h"
#include "ctimer.h"
#include "ctile.h"
#include "cworld.h"
#include "cweapon.h"

const char* CLASS_NAME = "Adventure";

WNDCLASSEX wc;
HDC dc;
HWND main_window;

typedef struct
{
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD          acolors[256];
} dibinfo_t;
dibinfo_t bmi = { 0 };

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
	DIR_UP    = 0,
	DIR_DOWN  = 3,
	DIR_LEFT  = 6,
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
	PLAYER_STATE_DEAD   = 8
} PlayerState;

typedef struct
{
    int counter;
    int max_count;
    int frame;
    int num_frames;
    int frame_order[16];
} Animation;

typedef struct
{
    int number;
    float x;
    float y;
    int float_duration_counter;
    int float_duration_counter_max;
    int color;
} FloatingNumber;

typedef struct
{
    float x;
    float y;
    Animation anim;
} Coin;

Coin coins[1000];
int num_coins = 0;

FloatingNumber floating_numbers[MAX_FLOATING_NUMBERS];
int num_floating_numbers = 0;

typedef struct
{
    char* name;
    int tile_index;
    int lvl;
    int xp;
    int hp;
    int max_hp;
    float x;
    float y;
    int x_vel;
    int y_vel;
    int gold;
    float speed;
    float attack_angle;
    int attack_frame_counter;
    BOOL attack_hit;
    Weapon weapon;
    Direction dir;
    AttackDirection attack_dir;
    Animation anim;
    PlayerState state;
} Player;
Player player;

typedef enum
{
    ENEMY_STATE_NONE,
    ENEMY_STATE_MOVE_UP,
    ENEMY_STATE_MOVE_DOWN,
    ENEMY_STATE_MOVE_LEFT,
    ENEMY_STATE_MOVE_RIGHT,
    ENEMY_STATE_MOVE_UP_LEFT,
    ENEMY_STATE_MOVE_UP_RIGHT,
    ENEMY_STATE_MOVE_DOWN_LEFT,
    ENEMY_STATE_MOVE_DOWN_RIGHT,
    ENEMY_STATE_ATTACK,
    ENEMY_STATE_HIT
} EnemyState;

typedef struct
{
    char* name;
    int tile_index;
    int xp;
    int hp;
    int max_hp;
    int x;
    int y;
    int x_vel;
    int y_vel;
    float speed;
    int action_counter;
    int action_duration_counter;
    int action_duration_counter_max;
    EnemyState state;
    Direction dir;
    Animation anim;
} Enemy;
Enemy enemies[NUM_RATS];
int num_enemies;

int rats_killed = 0; // @TEMP

int action_counter_max = 60;
int world_water_anim_counter = 0;
int world_water_anim_counter_max = 5;

KeyPress keypress = 0x0000;

BOOL is_running;

POINT camera  = {0};

int buffer_width;
int buffer_height;

int window_width;
int window_height;

const int BYTES_PER_PIXEL = 1;

double TARGET_FPS = 60.0f;

// prototypes
static LRESULT CALLBACK MainWndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);
static void setup_window(HINSTANCE hInstance);
static void update_scene();
static void draw_scene();
static void init_enemies();
static void init_player();
static void spawn_coin(int x, int y);
static void spawn_floating_number(float x, float y, int color);
static void remove_enemy(int index);
static void remove_floating_number(int index);
static void remove_coin(int index);
static BOOL set_working_directory();

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, s32 nshowcmd)
{
	set_working_directory();

	generate_palette_file("data\\16_color_palette.png");
	setup_window(hinstance);

	generate_indexed_tileset("data\\tileset0.png", bmi.acolors);
    generate_world_file("data\\world0.png");
    
	srand(time(NULL));

    init_font("data\\font.png");
	load_tileset("data\\tileset");

    init_weapons();
    init_world("data\\world");
    init_enemies();
    init_player();

    timer_init(TARGET_FPS); 
    is_running = TRUE;

	MSG msg;
	while (is_running)
	{
        // @MainLoop
		while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
        
        timer_tick();

		if (timer_ready())
		{
            update_scene();
            draw_scene();
        }
	}

	return EXIT_SUCCESS;
}

static void update_scene()
{
    // update world
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
					// animate
                    if(world[i][j] == WATER) 
                        world[i][j] = WATER2;
                    else
                        world[i][j] = WATER;
                }
            }
        }
    }
   
    // update player movement
	if ((keypress & KEYPRESS_LEFT) == KEYPRESS_LEFT)
	{
        player.state |= PLAYER_STATE_MOVE;
		player.dir = DIR_LEFT;
		player.x_vel = -1.0f;
	}
	else if ((keypress & KEYPRESS_RIGHT) == KEYPRESS_RIGHT)
	{
        player.state |= PLAYER_STATE_MOVE;
		player.dir = DIR_RIGHT;
		player.x_vel = +1.0f;
	}
	else
		player.x_vel = 0.0f;

	if ((keypress & KEYPRESS_UP) == KEYPRESS_UP)
	{
        player.state |= PLAYER_STATE_MOVE;
		player.dir = DIR_UP;
		player.y_vel = -1.0f;
	}
	else if ((keypress & KEYPRESS_DOWN) == KEYPRESS_DOWN)
	{
        player.state |= PLAYER_STATE_MOVE;
		player.dir = DIR_DOWN;
		player.y_vel = +1.0f;
	}
	else
		player.y_vel = 0.0f;

    player.x += player.x_vel*player.speed;
    player.y += player.y_vel*player.speed;

    // handle player collision
    // terrain collision
    //
    // 1            2
    //  x----------x
    //  |          |
    //  |          |
    //  x----------x
    // 3            4
    //

    int player_check_x1 = (player.x + TILE_WIDTH/4) / TILE_WIDTH;
    int player_check_y1 = (player.y + TILE_HEIGHT/2) / TILE_HEIGHT;

    int player_check_x2 = (player.x + 3*TILE_WIDTH/4) / TILE_WIDTH;
    int player_check_y2 = (player.y + TILE_HEIGHT/2) / TILE_HEIGHT;

    int player_check_x3 = (player.x + TILE_WIDTH/4) / TILE_WIDTH;
    int player_check_y3 = (player.y + TILE_HEIGHT) / TILE_HEIGHT;

    int player_check_x4 = (player.x + 3*TILE_WIDTH/4) / TILE_WIDTH;
    int player_check_y4 = (player.y + TILE_HEIGHT) / TILE_HEIGHT;

    int collision_value_1 = world_collision[player_check_x1][player_check_y1];
    int collision_value_2 = world_collision[player_check_x2][player_check_y2];
    int collision_value_3 = world_collision[player_check_x3][player_check_y3];
    int collision_value_4 = world_collision[player_check_x4][player_check_y4];


    if(collision_value_1 == 5 || collision_value_2 == 5 || collision_value_3 == 5 || collision_value_4 == 5)
    {
        BOOL correct_x = FALSE;
        BOOL correct_y = FALSE;

        if((collision_value_1 == 5 && collision_value_3 == 5) || (collision_value_2 == 5 && collision_value_4 == 5))
        {
            //correct collision x
            player.x -= player.x_vel*player.speed;
            correct_x = TRUE;
        }
        if((collision_value_1 == 5 && collision_value_2 == 5) || (collision_value_3 == 5 && collision_value_4 == 5))
        {
            //correct collision y
            player.y -= player.y_vel*player.speed;
            correct_y = TRUE;
        }
        
        if(!correct_x && !correct_y)
        {
            if(collision_value_1 == 5 || collision_value_3 == 5)
				player.x += 1.0f*player.speed;
            
            if(collision_value_2 == 5 || collision_value_4 == 5)
				player.x -= 1.0f*player.speed;
        }
    }

    // keep player in world
    if(player.x < 0) player.x = 0;
    if(player.y < 0) player.y = 0;
    if(player.x >TILE_WIDTH*(WORLD_TILE_WIDTH - 1)) player.x = TILE_WIDTH*(WORLD_TILE_WIDTH - 1);
    if(player.y >TILE_HEIGHT*(WORLD_TILE_HEIGHT - 1)) player.y = TILE_HEIGHT*(WORLD_TILE_HEIGHT - 1);

    // update player movement animation
    if(player.x_vel != 0 || player.y_vel != 0)
    {
        player.anim.counter++;

        if(player.anim.counter >= 10/player.speed)
        {
            // cycle_animation
            player.anim.counter = 0;
            player.anim.frame += 1;
            if(player.anim.frame >= player.anim.num_frames)
                player.anim.frame = 0;
        }
    }
    else
    {
        // clear animation frame
        player.state ^= PLAYER_STATE_MOVE;
        player.anim.counter = 0;
        player.anim.frame = 0;
    }

    // update player attacking
    if((player.state & PLAYER_STATE_ATTACK) == PLAYER_STATE_ATTACK)
    {
        // switch player direction to attacking direction
        switch(player.attack_dir)
        {
            case PLAYER_ATTACK_LEFT: player.dir = DIR_LEFT; break;
            case PLAYER_ATTACK_RIGHT: player.dir = DIR_RIGHT; break;
            case PLAYER_ATTACK_UP: player.dir = DIR_UP; break;
            case PLAYER_ATTACK_DOWN: player.dir = DIR_DOWN; break; 
        }

        player.attack_angle += player.weapon.attack_speed*(PI/30.0f);
        player.attack_frame_counter++;

        // check for collision with enemies/objects
        //if(!player.attack_hit)
        //{
        float cosa = cos(player.attack_angle);
        float sina = sin(player.attack_angle);

        for(int i = num_enemies-1; i >= 0;--i)
        {
            int relative_enemy_position_x = enemies[i].x - camera.x;
            int relative_enemy_position_y = enemies[i].y - camera.y;

            // only care about enemies on screen...
            if(relative_enemy_position_x > 0 && relative_enemy_position_x < buffer_width)
            {
                if(relative_enemy_position_y > 0 && relative_enemy_position_y < buffer_height)
                {
                    // check along weapon line
                    for(int j = 0; j < player.weapon.attack_range; ++j)
                    {
                        // only care about hitting an enemy if it hasn't been hit yet.
                        if(enemies[i].state != ENEMY_STATE_HIT)
                        {
                            int start_weapon_x = player.x - camera.x + TILE_WIDTH/2;
                            int start_weapon_y = player.y - camera.y + TILE_HEIGHT/2;
                        
                            float delta_x = cosa*j;
                            float delta_y = -sina*j;

                            if(start_weapon_x+delta_x >= relative_enemy_position_x && start_weapon_x+delta_x <= relative_enemy_position_x+0.75*TILE_WIDTH)
                            {
                                if(start_weapon_y+delta_y >= relative_enemy_position_y && start_weapon_y+delta_y <= relative_enemy_position_y+0.75*TILE_HEIGHT)
                                {
                                    int damage = (rand() % (player.weapon.max_damage - player.weapon.min_damage + 1)) + player.weapon.min_damage;
                                    
                                    // add floating number
                                    spawn_floating_number(start_weapon_x+delta_x+camera.x,start_weapon_y+delta_y+camera.y,damage,6);

                                    // enemy hurt!
                                    enemies[i].hp -= damage;

                                    player.attack_hit = TRUE;
                                    
                                    // check if enemy died
                                    if (enemies[i].hp <= 0)
                                    {
                                        rats_killed++;
										int coins_spawned = rand() % 10;

										for(int c = 0; c < coins_spawned; ++c)
											spawn_coin(enemies[i].x + (rand()%(TILE_WIDTH/2) -(TILE_WIDTH/4)),enemies[i].y + (rand() % (TILE_HEIGHT/2) - (TILE_HEIGHT / 4)));

                                        remove_enemy(i);
                                    }
                                    else
                                    {
                                        enemies[i].state = ENEMY_STATE_HIT;
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        if(player.attack_frame_counter >= 15.0f/player.weapon.attack_speed)
        {
            // end attacking
            player.attack_frame_counter = 0;
            player.state ^= PLAYER_STATE_ATTACK;
            player.attack_hit = FALSE;
        }
    }

    // check if player got coins
    for(int i = num_coins -1; i>= 0; --i)
    {
        int coin_x = coins[i].x + TILE_WIDTH/2 -2;
        int coin_y = coins[i].y + TILE_HEIGHT/2 -2;

        // only consider coins on screen
        if(coin_x - camera.x >= 0 && coin_x - camera.x <= buffer_width)
        {
            if(coin_y - camera.y >= 0 && coin_y - camera.y <= buffer_height)
            {
                // check for player collision
                if(coin_x >= player.x && coin_x <= player.x + TILE_WIDTH)
                {
                    if(coin_y >= player.y + TILE_HEIGHT/2 && coin_y <= player.y + TILE_HEIGHT)
                    {
                        player.gold++;
                        spawn_floating_number(coins[i].x,coins[i].y,1,10);
						remove_coin(i);
                    }
                }
            }
        }
    }

    // update coins
    for(int i = num_coins -1; i >= 0; --i)
    {
        // update coin animation
        coins[i].anim.counter++;

        if(coins[i].anim.counter >= coins[i].anim.max_count)
        {
            // cycle_animation
            coins[i].anim.counter = 0;
            coins[i].anim.frame += 1;
            if(coins[i].anim.frame >= coins[i].anim.num_frames)
                coins[i].anim.frame = 0;
        }
    }
    
    // update floating numbers
    for(int i = num_floating_numbers -1; i >= 0; --i)
    {
        floating_numbers[i].y -= 0.25f;
        floating_numbers[i].float_duration_counter++;

        if(floating_numbers[i].float_duration_counter >= floating_numbers[i].float_duration_counter_max)
        {
            remove_floating_number(i);
        }
    }

    // update enemies
    for(int i = 0; i < num_enemies;++i)
    {
        if(enemies[i].state == ENEMY_STATE_NONE)
        {
            enemies[i].action_counter++;

            if(enemies[i].action_counter >= action_counter_max)
            {
                enemies[i].action_counter = 0;
                enemies[i].action_duration_counter_max = rand() % 60 + 1; // duration of action
                
                // take an action
                int d = rand() % 9;
                switch (d)
                {
                    case 0:
                        enemies[i].state = ENEMY_STATE_NONE;
                    case 1: 
                        enemies[i].state = ENEMY_STATE_MOVE_UP;
                        enemies[i].dir = DIR_UP;
                        enemies[i].x_vel = +0;
                        enemies[i].y_vel = -1;
                        break;
                    case 2: 
                        enemies[i].state = ENEMY_STATE_MOVE_DOWN;
                        enemies[i].dir = DIR_DOWN;
                        enemies[i].x_vel = +0;
                        enemies[i].y_vel = +1;
                        break;
                    case 3: 
                        enemies[i].state = ENEMY_STATE_MOVE_LEFT;
                        enemies[i].dir = DIR_LEFT;
                        enemies[i].x_vel = -1;
                        enemies[i].y_vel = +0;
                        break;
                    case 4: 
                        enemies[i].state = ENEMY_STATE_MOVE_RIGHT;
                        enemies[i].dir = DIR_RIGHT;
                        enemies[i].x_vel = +1;
                        enemies[i].y_vel = +0;
                        break;
                    case 5: 
                        enemies[i].state = ENEMY_STATE_MOVE_UP_LEFT;
                        enemies[i].dir = DIR_UP;
                        enemies[i].x_vel = -1;
                        enemies[i].y_vel = -1;
                        break;
                    case 6: 
                        enemies[i].state = ENEMY_STATE_MOVE_UP_RIGHT;
                        enemies[i].dir = DIR_UP;
                        enemies[i].x_vel = +1;
                        enemies[i].y_vel = -1;
                        break;
                    case 7: 
                        enemies[i].state = ENEMY_STATE_MOVE_DOWN_LEFT;
                        enemies[i].dir = DIR_DOWN;
                        enemies[i].x_vel = -1;
                        enemies[i].y_vel = +1;
                        break;
                    case 8: 
                        enemies[i].state = ENEMY_STATE_MOVE_DOWN_RIGHT;
                        enemies[i].dir = DIR_DOWN;
                        enemies[i].x_vel = +1;
                        enemies[i].y_vel = +1;
                        break;
                }
            }
        }
        else
        {
            switch (enemies[i].state)
            {
                case ENEMY_STATE_MOVE_UP:
                case ENEMY_STATE_MOVE_DOWN:
                case ENEMY_STATE_MOVE_LEFT:
                case ENEMY_STATE_MOVE_RIGHT:
                case ENEMY_STATE_MOVE_UP_LEFT:
                case ENEMY_STATE_MOVE_UP_RIGHT:
                case ENEMY_STATE_MOVE_DOWN_LEFT:
                case ENEMY_STATE_MOVE_DOWN_RIGHT:

                    enemies[i].x += enemies[i].x_vel;
                    enemies[i].y += enemies[i].y_vel;
                    
                    // handle enemies[i] collision
                    // terrain collision
                    //
                    // 1            2
                    //  x----------x
                    //  |          |
                    //  |          |
                    //  x----------x
                    // 3            4
                    //

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

                    if(enemies[i].x < 0) enemies[i].x = 0;
                    if(enemies[i].y < 0) enemies[i].y = 0;
                    if(enemies[i].x >TILE_WIDTH*(WORLD_TILE_WIDTH-1)) enemies[i].x = TILE_WIDTH*(WORLD_TILE_WIDTH-1);
                    if(enemies[i].y >TILE_HEIGHT*(WORLD_TILE_HEIGHT-1)) enemies[i].y = TILE_HEIGHT*(WORLD_TILE_HEIGHT-1);

                    break;
                case ENEMY_STATE_ATTACK:
                    // @TODO: handle enemy attack
                case ENEMY_STATE_HIT:
                    if((player.state & PLAYER_STATE_ATTACK) != PLAYER_STATE_ATTACK)
                    {
                        enemies[i].state = ENEMY_STATE_NONE;
                    }
                    break;
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


            enemies[i].action_duration_counter++;

            if(enemies[i].action_duration_counter >= enemies[i].action_duration_counter_max)
            {
                enemies[i].action_duration_counter = 0;
                enemies[i].state = ENEMY_STATE_NONE; // return to neutral state
                enemies[i].x_vel = +0;
                enemies[i].y_vel = +0;
            }
        }
    }


    // update camera
    camera.x = (player.x - (buffer_width / 2));
    camera.x = max(camera.x, 0);
    camera.x = min(camera.x,TILE_WIDTH*WORLD_TILE_WIDTH-buffer_width);

    camera.y = (player.y - (buffer_height / 2));
    camera.y = max(camera.y, 0);
    camera.y = min(camera.y,TILE_HEIGHT*WORLD_TILE_HEIGHT-buffer_height);

}

static void draw_scene()
{
    // Clear buffer to bg_color 
    memset(back_buffer,5, buffer_width*buffer_height*BYTES_PER_PIXEL);
    
    // draw world
    draw_world(camera);
    
    for(int i = 0; i < num_coins; ++i)
    {
		draw_tile(coins[i].x - camera.x, coins[i].y - camera.y, 80 + coins[i].anim.frame_order[coins[i].anim.frame]);
    }
    
	// draw enemies
	for (int i = 0; i < num_enemies; ++i)
	{
        int enemy_x = enemies[i].x - camera.x;
        int enemy_y = enemies[i].y - camera.y;

        if (enemy_x+TILE_WIDTH < 0 || enemy_x > buffer_width)
            continue;
        if (enemy_y+TILE_HEIGHT < 0 || enemy_y > buffer_height)
            continue;
        
        if (enemies[i].state == ENEMY_STATE_HIT)
        {
            draw_tile_tinted(enemies[i].x - camera.x, enemies[i].y - camera.y, enemies[i].tile_index + enemies[i].dir + enemies[i].anim.frame_order[enemies[i].anim.frame],6);
        }
        else
        {
            draw_tile(enemies[i].x - camera.x, enemies[i].y - camera.y, enemies[i].tile_index + enemies[i].dir + enemies[i].anim.frame_order[enemies[i].anim.frame]);
        }
	}
    
    // draw player shadow
    int shadow_y = player.y + TILE_HEIGHT - 3;

    for(int i = 7; i > 0; --i)
    {
		for (int j = 1; j < 15; ++j)
        {
            shade_pixel8(player.x - camera.x + j,shadow_y - camera.y,i);
        }
        shadow_y++;
    }

    // draw player
	if (player.dir != DIR_UP)
		draw_tile(player.x - camera.x,player.y - camera.y,player.tile_index + player.dir+player.anim.frame_order[player.anim.frame]);

    if((player.state & PLAYER_STATE_ATTACK) == PLAYER_STATE_ATTACK)
    {
        // draw weapon
        draw_tile_rotated(player.x - camera.x + cos(player.attack_angle)*2*TILE_WIDTH/3,player.y - camera.y - sin(player.attack_angle) * 2*TILE_HEIGHT/3,player.weapon.tile_index,player.attack_angle);
    }
    
	if (player.dir == DIR_UP)
		draw_tile(player.x - camera.x, player.y - camera.y, player.tile_index + player.dir + player.anim.frame_order[player.anim.frame]);

    // draw coins
    // draw floating numbers
    for(int i = 0; i < num_floating_numbers; ++i)
    {
		draw_number_string(floating_numbers[i].number, floating_numbers[i].x - camera.x, floating_numbers[i].y - camera.y, 1.0f, floating_numbers[i].color + 16*(floating_numbers[i].float_duration_counter/10));
    }
	
    // @DEBUG
    // move along weapon line and check for collision
    /*
    float cosa = cos(player.attack_angle);
    float sina = sin(player.attack_angle);

    int start_weapon_x = player.x - camera.x + TILE_WIDTH/2;
    int start_weapon_y = player.y - camera.y + TILE_HEIGHT/2;
    
    // check along weapon line
    for(int i = 0; i < player.weapon.attack_range; ++i)
    {
        float delta_x = cosa*i;
        float delta_y = -sina*i;

        draw_pixel8(start_weapon_x+delta_x,start_weapon_y+delta_y,5);
    }
    */
    
    // draw UI
    draw_string(player.name,0,buffer_height - 7,1.0f,1);
    draw_string(player.weapon.name,60,buffer_height -7,1.0f,3);
    draw_string("Gold:",buffer_width - 100, buffer_height -7,1.0f,3);
	draw_number_string(player.hp, 26, buffer_height - 7, 1.0f, 9);
	draw_number_string(player.max_hp, 40, buffer_height - 7, 1.0f, 9);
    draw_number_string(player.gold,buffer_width - 44,buffer_height -7,1.0f,14);

    draw_string("Dead Rats:",1,1,1.0f,0);
    draw_number_string(rats_killed,61,1,1.0f,0);
    draw_string("Dead Rats:",0,0,1.0f,7);
    draw_number_string(rats_killed,60,0,1.0f,8);
	//

    // Blit buffer to screen
    StretchDIBits(dc, 0, 0, window_width, window_height, 0, 0, buffer_width, buffer_height, back_buffer, (BITMAPINFO*)&bmi, DIB_RGB_COLORS, SRCCOPY);
}

static void init_player()
{
    player.name = "Hero";
    player.tile_index = 0;
    player.lvl = 1;
    player.xp  = 0;
    player.hp  = 10;
    player.max_hp = 10;
    player.gold = 0;
    player.x   = (WORLD_TILE_WIDTH*(TILE_WIDTH-1))/2;
    player.y   = (WORLD_TILE_HEIGHT*(TILE_HEIGHT-1))/2;
    player.x_vel = 0;
    player.y_vel = 0;
    player.speed = 1.0f;
    player.dir = DIR_DOWN;
    player.state = PLAYER_STATE_NONE;
    player.anim.counter = 0;
    player.anim.max_count = 10;
    player.anim.frame = 0;
    player.anim.num_frames = 4;
    player.anim.frame_order[0] = 0;
    player.anim.frame_order[1] = 1;
    player.anim.frame_order[2] = 0;
    player.anim.frame_order[3] = 2;
    player.attack_angle = 0.0f;
    player.attack_frame_counter = 0;
    player.attack_hit = FALSE;
    player.weapon.name = weapons[1].name;
    player.weapon.attack_speed = weapons[1].attack_speed;
    player.weapon.attack_range = weapons[1].attack_range;
    player.weapon.min_damage = weapons[1].min_damage;
    player.weapon.max_damage = weapons[1].max_damage;
    player.weapon.tile_index = weapons[1].tile_index;
}

static void init_enemies()
{

    for (int i = 0; i < NUM_RATS ; ++i)
    {
        Enemy e = {0};

        int test_x, test_y;
        int test_collision_1, test_collision_2, test_collision_3, test_collision_4;
        BOOL collision = TRUE;

        // make sure not to spawn enemy in water or on a wall
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

        enemies[num_enemies].x = test_x; 
        enemies[num_enemies].y = test_y;
        enemies[num_enemies].x_vel = 0;
        enemies[num_enemies].y_vel = 0;
        enemies[num_enemies].speed = 1.0f;
        enemies[num_enemies].dir = DIR_DOWN;
        enemies[num_enemies].state = ENEMY_STATE_NONE;
        enemies[num_enemies].name = "Rat";
        enemies[num_enemies].tile_index = 16;
        enemies[num_enemies].hp = 5;
        enemies[num_enemies].max_hp = 5;
        enemies[num_enemies].xp = 1;
        enemies[num_enemies].action_counter = rand() % action_counter_max;
        enemies[num_enemies].action_duration_counter = 0;
        enemies[num_enemies].anim.counter = 0;
        enemies[num_enemies].anim.max_count = 10;
        enemies[num_enemies].anim.frame = 0;
        enemies[num_enemies].anim.num_frames = 4;
        enemies[num_enemies].anim.frame_order[0] = 0;
        enemies[num_enemies].anim.frame_order[1] = 1;
        enemies[num_enemies].anim.frame_order[2] = 0;
        enemies[num_enemies].anim.frame_order[3] = 2;

        num_enemies++;
    }
}

static void remove_enemy(int index)
{
	num_enemies--;
	enemies[index] = enemies[num_enemies];
}

static void remove_floating_number(int index)
{
	num_floating_numbers--;
	floating_numbers[index] = floating_numbers[num_floating_numbers];
}
static void spawn_floating_number(float x, float y, int number,int color)
{
    floating_numbers[num_floating_numbers].number = number;
    floating_numbers[num_floating_numbers].x = x;
    floating_numbers[num_floating_numbers].y = y; 
    floating_numbers[num_floating_numbers].color = color;
    floating_numbers[num_floating_numbers].float_duration_counter = 0;
    floating_numbers[num_floating_numbers].float_duration_counter_max = 60;
    num_floating_numbers++;
}

static void spawn_coin(int x, int y)
{
    coins[num_coins].x = x;
    coins[num_coins].y = y;
    coins[num_coins].anim.counter = rand() % 5;
    coins[num_coins].anim.max_count = 5;
    coins[num_coins].anim.frame = 0;
    coins[num_coins].anim.num_frames = 4;
    coins[num_coins].anim.frame_order[0] = 0;
    coins[num_coins].anim.frame_order[1] = 1;
    coins[num_coins].anim.frame_order[2] = 2;
    coins[num_coins].anim.frame_order[3] = 3;

    num_coins++;
}

static void remove_coin(int index)
{
	num_coins--;
	coins[index] = coins[num_coins];
}

static BOOL set_working_directory()
{
	char cwd[256] = { 0 };
    char curr_path[256] = {0};
    GetModuleFileName(NULL, curr_path, 256);

	for (int i = 255; curr_path[i] != '\\' && i > 0; --i)
		curr_path[i] = 0;

    if(!curr_path)
        return FALSE;

    if(!SetCurrentDirectory(curr_path))
		return FALSE;

	return TRUE;
}

static void setup_window(HINSTANCE hInstance)
{
 	buffer_width  = 240; // 15 16px tiles
	buffer_height = 176; // 11 16px tiles

	window_width = 1024;
	window_height = 768;

	wc.lpfnWndProc = MainWndProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = CLASS_NAME;
	wc.style = CS_DBLCLKS;
	wc.cbSize = sizeof(WNDCLASSEX);

	//wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	//wc.hIconSm = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	wc.lpszMenuName = NULL;
	wc.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(0x0f, 0x0f, 0x0f)));
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;

	if (!RegisterClassExA(&wc))
		return;

	DWORD window_style_ex = 0;
	DWORD window_style = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
	
	RECT r = { 0 };

	r.right = window_width;
	r.bottom = window_height;

	// To make the drawable space truly BUFFER_WIDTH & BUFFER_HEIGHT
	AdjustWindowRectEx(&r, window_style, 0, window_style_ex);

	// Get screen dimensions to startup window at center-screen
	RECT w = { 0 };
	GetClientRect(GetDesktopWindow(), &w);

	main_window = CreateWindowEx(
		window_style_ex
		, CLASS_NAME
		, "Adventure"
		, window_style
		, (w.right / 2) - (window_width / 2)
		, (w.bottom / 2) - (window_height / 2)
		, r.right - r.left
		, r.bottom - r.top
		, NULL
		, NULL
		, hInstance
		, 0
		);

	ShowWindow(main_window, SW_SHOWDEFAULT);

	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = buffer_width;
	bmi.bmiHeader.biHeight = -buffer_height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 8;
	bmi.bmiHeader.biCompression = BI_RGB;

	back_buffer = malloc(buffer_width*buffer_height * BYTES_PER_PIXEL);

    // open palette file
	FILE *fp;
    fp = fopen("data\\palette","rb");

    if(fp != NULL)
    {
        int color_index = 0;
        while(1) {

          bmi.acolors[color_index].rgbRed   = fgetc(fp);
          bmi.acolors[color_index].rgbGreen = fgetc(fp);
          bmi.acolors[color_index].rgbBlue  = fgetc(fp);

		  if (palette_num_channels == 4) 
			  fgetc(fp);

          if(feof(fp))
             break;

		  color_index++;

        }

        int percentage_decrease = 5;

		while (color_index < 256)
		{
			if (color_index == 255)
			{
				// set transparent color...
				bmi.acolors[color_index].rgbRed = 255;
				bmi.acolors[color_index].rgbGreen = 0;
				bmi.acolors[color_index].rgbBlue = 255;
				break;
			}

			bmi.acolors[color_index].rgbRed = bmi.acolors[color_index % 16].rgbRed * (1.0f - (percentage_decrease/100.0f));
			bmi.acolors[color_index].rgbGreen = bmi.acolors[color_index % 16].rgbGreen * (1.0f - (percentage_decrease/100.0f));
			bmi.acolors[color_index].rgbBlue = bmi.acolors[color_index % 16].rgbBlue * (1.0f - (percentage_decrease/100.0f));

			color_index++;

			if (color_index % 16 == 0)
			{
				percentage_decrease += 7;
				percentage_decrease = min(100, percentage_decrease);
			}
		}
    }
    else
    {
        // randomly generate palette colors
        for (int i = 0; i < 256; ++i)
        {
            bmi.acolors[i].rgbRed   = rand() % 256;
            bmi.acolors[i].rgbGreen = rand() % 256; 
            bmi.acolors[i].rgbBlue  = rand() % 256;
        }
    }

    fclose(fp);
    
	dc = GetDC(main_window);
}


static LRESULT CALLBACK MainWndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
        case WM_CLOSE:
        {
            is_running = FALSE;
            PostQuitMessage(0);
            break;
        }
        case WM_DESTROY:
        {
            is_running = FALSE;
            PostQuitMessage(0);
            break;
        }
        case WM_KEYDOWN:
        {
            if(wparam == 'A')
                keypress |= KEYPRESS_LEFT;
            else if(wparam == 'D')
                keypress |= KEYPRESS_RIGHT;

            if(wparam == 'W')
                keypress |= KEYPRESS_UP;
            else if(wparam == 'S')
                keypress |= KEYPRESS_DOWN;

            if((player.state & PLAYER_STATE_ATTACK) != PLAYER_STATE_ATTACK)
            {
                // start an attack if you are already not attacking
                if(wparam == VK_LEFT)
                {
                    player.state |= PLAYER_STATE_ATTACK;
                    player.attack_dir = PLAYER_ATTACK_LEFT;
                    player.attack_angle = +3*PI/4;
                }
                else if(wparam == VK_RIGHT)
                {
                    player.state |= PLAYER_STATE_ATTACK;
                    player.attack_dir = PLAYER_ATTACK_RIGHT;
                    player.attack_angle = -1*PI/4;
                }
                else if(wparam == VK_UP)
                {
                    player.state |= PLAYER_STATE_ATTACK;
                    player.attack_dir = PLAYER_ATTACK_UP;
                    player.attack_angle = +1*PI/4;
                }
                else if(wparam == VK_DOWN)
                {
                    player.state |= PLAYER_STATE_ATTACK;
                    player.attack_dir = PLAYER_ATTACK_DOWN;
                    player.attack_angle = -3*PI/4;
                }
            }
            
            // @DEV
            if(wparam == 'K')
            {
                player.speed = 8;
                player.anim.max_count = 1;

            }
            if(wparam == '1')
            {
                player.weapon.name = weapons[0].name;
                player.weapon.attack_speed = weapons[0].attack_speed;
                player.weapon.attack_range = weapons[0].attack_range;
                player.weapon.min_damage = weapons[0].min_damage;
                player.weapon.max_damage = weapons[0].max_damage;
                player.weapon.tile_index = weapons[0].tile_index;
            }
            else if(wparam == '2')
            {
                player.weapon.name = weapons[1].name;
                player.weapon.attack_speed = weapons[1].attack_speed;
                player.weapon.attack_range = weapons[1].attack_range;
                player.weapon.min_damage = weapons[1].min_damage;
                player.weapon.max_damage = weapons[1].max_damage;
                player.weapon.tile_index = weapons[1].tile_index;

            }
            else if(wparam == '3')
            {
                player.weapon.name = weapons[2].name;
                player.weapon.attack_speed = weapons[2].attack_speed;
                player.weapon.attack_range = weapons[2].attack_range;
                player.weapon.min_damage = weapons[2].min_damage;
                player.weapon.max_damage = weapons[2].max_damage;
                player.weapon.tile_index = weapons[2].tile_index;
            }
            else if(wparam == '4')
            {
                player.weapon.name = weapons[3].name;
                player.weapon.attack_speed = weapons[3].attack_speed;
                player.weapon.attack_range = weapons[3].attack_range;
                player.weapon.min_damage = weapons[3].min_damage;
                player.weapon.max_damage = weapons[3].max_damage;
                player.weapon.tile_index = weapons[3].tile_index;
            }

			break;
        } 
		case WM_KEYUP:
		{
            if(wparam == 'A')
                keypress &= (~KEYPRESS_LEFT);
            else if(wparam == 'D')
                keypress &= (~KEYPRESS_RIGHT);

            if(wparam == 'W')
                keypress &= (~KEYPRESS_UP);
            else if(wparam == 'S')
                keypress &= (~KEYPRESS_DOWN);
            
            // @DEV
            if(wparam == 'K')
            {
                player.speed = 1;
                player.anim.max_count = 10;
            }

			break;
		}
        
        default:
        {
            break;
        }
	}

	return DefWindowProc(hwnd, umsg, wparam, lparam);
}
