#define WIN32_LEAN_AND_MEAN
#define PI 3.14159265359 

#include <windows.h>
#include <math.h>     // for abs,atan,cos,sin
#include <strsafe.h>
#include <time.h>     // for time function
#include "common.h"
#include "cstr.h"
#include "cdraw.h"
#include "cfontsimple.h"
#include "ctimer.h"
#include "canimation.h"
#include "ctile.h"
#include "cweapon.h"
#include "cworld.h"
#include "ccoin.h"
#include "citem.h"
#include "cparticles.h"
#include "cprojectile.h"
#include "cfloatingnumber.h"
#include "cenemy.h"
#include "cplayer.h"
#include "cnpc.h"
#include "centity.h"
#include "casset.h"

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

BOOL is_running;

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

	load_all_assets();

    init_world("data\\world");
    init_enemies();
    init_player();
    init_items();
    init_npcs();

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
    // update message
    message.active = FALSE;
    player.notch = FALSE;

    // update world
    update_world();

    // update player
    update_player();
   
    // update items
    update_items();
    
    // update coins
    update_coins();

    // update floating numbers
    update_floating_numbers();

    // update enemies
    update_enemies();
    
    // update npcs
    update_npcs();

    // update particles
    update_particles();
    
    // update particles
    update_projectiles();

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
    draw_world(camera,day_cycle_shade_amount);

    sort_entities();
    draw_entities();

    if(message.active)
        draw_message();
    
    // draw HUD 
    // health
    int ui_x = 0;

    for(int i = 0; i < player.max_hp / 2.0f;++i)
    {
        draw_char_with_shadow(CHAR_HEART,ui_x + 6*i,buffer_height -7,3);
    }

    float num_hearts = player.hp / 2.0f;
    while(num_hearts >= 1)
    {
        draw_char_with_shadow(CHAR_HEART,ui_x, buffer_height -7, 6);
        ui_x += 6;
        num_hearts--;
    }
    if(num_hearts > 0)
        draw_char(CHAR_HEART_HALF,ui_x, buffer_height -7, 6);

    ui_x += 40;
    
    // gold
    draw_string_with_shadow("Gold:",ui_x, buffer_height -7,1.0f,7); ui_x += 30;
    draw_number_string_with_shadow(player.gold,ui_x,buffer_height -7,1.0f,14); ui_x += 30;

    // lvl
    draw_string_with_shadow("Lvl:",ui_x, buffer_height -7,1.0f,7); ui_x += 24;
    draw_number_string_with_shadow(player.lvl,ui_x,buffer_height -7,1.0f,14); ui_x += 18;
    
    // xp
    float xp_percentage = player.xp / next_level;

    draw_string_with_shadow("XP:",ui_x, buffer_height -7,1.0f,7); ui_x += 18;
    draw_rect8(ui_x,buffer_height-6,50,4,1,TRUE);
    draw_rect8(ui_x,buffer_height-6,50*xp_percentage,4,8,TRUE);
    
    // weapon
    draw_string_with_shadow(player.weapon.name,buffer_width - 50,0,1.0f,7);

    draw_string_with_shadow("Dead Foes:",0,0,1.0f,7);
    draw_number_string_with_shadow(foes_killed,60,0,1.0f,8);

    // Blit buffer to screen
    StretchDIBits(dc, 0, 0, window_width, window_height, 0, 0, buffer_width, buffer_height, back_buffer, (BITMAPINFO*)&bmi, DIB_RGB_COLORS, SRCCOPY);
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

            // start an attack if you are already not attacking
            if((player.state & PLAYER_STATE_ATTACK) != PLAYER_STATE_ATTACK)
            {
                if(player.weapon.type == WEAPON_TYPE_MELEE)
                {
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
            }

            if(player.weapon.type == WEAPON_TYPE_RANGED)
            {
                if(wparam == VK_LEFT)
                    keypress_attack |= KEYPRESS_LEFT;
                else if(wparam == VK_RIGHT)
                    keypress_attack |= KEYPRESS_RIGHT;
                else if(wparam == VK_UP)
                    keypress_attack |= KEYPRESS_UP;
                else if(wparam == VK_DOWN)
                    keypress_attack |= KEYPRESS_DOWN;
                
                if(wparam == VK_LEFT)
                {
                    player.state |= PLAYER_STATE_NOTCHED;
                    player.attack_dir = PLAYER_ATTACK_LEFT;
                    player.attack_angle = PI;
                    player.notch = TRUE;
                }
                else if(wparam == VK_RIGHT)
                {
                    player.state |= PLAYER_STATE_NOTCHED;
                    player.attack_dir = PLAYER_ATTACK_RIGHT;
                    player.attack_angle = 0;
                    player.notch = TRUE;
                }
                else if(wparam == VK_UP)
                {
                    player.state |= PLAYER_STATE_NOTCHED;
                    player.attack_dir = PLAYER_ATTACK_UP;
                    player.attack_angle = +PI/2;
                    player.notch = TRUE;
                }
                else if(wparam == VK_DOWN)
                {
                    player.state |= PLAYER_STATE_NOTCHED;
                    player.attack_dir = PLAYER_ATTACK_DOWN;
                    player.attack_angle = -PI/2;
                    player.notch = TRUE;
                }
            }
            
            if(wparam == 'C')
            {
                player.throw_coins = TRUE;
                player.coin_throw_counter = player.coin_throw_max; // start instant initial throw
            }

            if(wparam == 'E')
            {
                player.pickup = TRUE;
            }

            if(wparam == 'R')
            {
                player.take = TRUE;
            }

            if(wparam == VK_SPACE)
            {
                player.jump = TRUE;
            }

            if(wparam == VK_SHIFT)
            {
                player.base_speed = 3.0f;
            }

            // @DEV
            if(wparam == '1')
            {
                get_weapon_by_name("Knife",&player.weapon);
            }
            else if(wparam == '2')
            {
                get_weapon_by_name("Sword",&player.weapon);
            }
            else if(wparam == '3')
            {
                get_weapon_by_name("Axe",&player.weapon);
            }
            else if(wparam == '4')
            {
                get_weapon_by_name("Ultimate",&player.weapon);
            }
            else if(wparam == '5')
            {
                get_weapon_by_name("Bow",&player.weapon);
            }
            else if(wparam == '6')
            {
                get_weapon_by_name("Staff",&player.weapon);
            }

            if(wparam == 'H')
            {
                // re-load assets
                load_all_assets();

                init_enemies();
                init_items();
                init_npcs();
                init_weapons();
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
            
            if(wparam == 'C')
            {
                player.throw_coins = FALSE;
            }
            
            if(player.weapon.type == WEAPON_TYPE_RANGED)
            {
                if(wparam == VK_LEFT)
                    keypress_attack ^= KEYPRESS_LEFT;
                else if(wparam == VK_RIGHT)
                    keypress_attack ^= KEYPRESS_RIGHT;
                else if(wparam == VK_UP)
                    keypress_attack ^= KEYPRESS_UP;
                else if(wparam == VK_DOWN)
                    keypress_attack ^= KEYPRESS_DOWN;
            }

            if((player.state & PLAYER_STATE_NOTCHED) == PLAYER_STATE_NOTCHED)
                if(!player.notch)
                    if(keypress_attack == 0x0000)
                        player.shoot = TRUE;

            
            // @DEV
            if(wparam == VK_SHIFT)
            {
                player.base_speed = 1.0f;
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
