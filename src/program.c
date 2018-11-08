#define WIN32_LEAN_AND_MEAN
#define PI 3.14159265359f

#include <windows.h>
#include <mmsystem.h>
#include <math.h>     // for abs,atan,cos,sin
#include <strsafe.h>
#include <time.h>     // for time function
#include "common.h"
#include "cwin.h"
#include "cstr.h"
#include "cdraw.h"
#include "font.h"
#include "timer.h"
#include "animation.h"
#include "tile.h"
#include "zone.h"
#include "board.h"
#include "item.h"
#include "item_stand.h"
#include "coin.h"
#include "particle.h"
#include "weather.h"
#include "floatingnumber.h"
#include "projectile.h"
#include "creature.h"
#include "portal.h"
#include "player.h"
#include "entity.h"
#include "asset.h"
#include "hud.h"

const char* CLASS_NAME = "Adventure";

BOOL is_running;

int buffer_width;
int buffer_height;

int window_width;
int window_height;

int counter_for_seconds = 0;
int counter_for_minutes = 0;

BOOL shift_down = FALSE;

const int BYTES_PER_PIXEL = 1;

static LRESULT CALLBACK MainWndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);
static void setup_window(HINSTANCE hInstance);
static void update_scene();
static void draw_scene();
static BOOL set_working_directory();

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, s32 nshowcmd)
{
	set_working_directory();

	generate_palette_file("data\\palettes\\palette_main.png");
	setup_window(hinstance);

    generate_all_tilesets();
    //generate_all_boards();
    
    // seed PRNG
	srand(time(NULL));

    init_font("data\\font.png");

    load_all_tilesets();
    load_all_boards();
    load_board_map();
	load_all_assets();

    init_creatures();
    init_items();
    init_player();
	load_player_props();
    init_boards();
    init_item_stands();
    init_portals();

    // test
    //playMIDIFile(main_window,"data\\music\\village.mid");
    // 

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

		if (timer_ready(&current_fps))
		{
            update_scene();
            draw_scene();
        }
	}

    //fclose(fp_rats);

	return EXIT_SUCCESS;
}

static void update_scene()
{
    // update message
    message.active = FALSE;
    player.notch = FALSE;

    update_board(current_board_index);
    update_player();
    update_item_stands();
    update_items();
    update_coins();
    update_floating_numbers();
    update_creatures();
    update_weather();
    update_particles();
    update_projectiles();
	update_hud();

    // update camera
    camera.x = (player.phys.x - (buffer_width / 2));
    camera.x = max(camera.x, 0);
    camera.x = min(camera.x,TILE_WIDTH*BOARD_TILE_WIDTH-buffer_width);

    camera.y = (player.phys.y - (buffer_height / 2));
    camera.y = max(camera.y, 0);
    camera.y = min(camera.y,TILE_HEIGHT*BOARD_TILE_HEIGHT-buffer_height);

}

// @TEMP
static void draw_bounding_box(PhysicalProperties* p)
{
    draw_rect8(p->x - camera.x,p->y - camera.y,p->width,p->height,6,FALSE);
}

static void draw_scene()
{
    // Clear buffer to bg_color 
    memset(back_buffer,1, buffer_width*buffer_height*BYTES_PER_PIXEL);
    
    // draw board
    draw_board(current_board_index);

    sort_entities();
    draw_entities();

    if(display_board_title)
    {
        int l = length_of(board_list[current_board_index].name) * 6;
        if((player.state & PLAYER_STATE_DEAD) == PLAYER_STATE_DEAD)
            draw_string_with_shadow("The Abyss",(buffer_width - 54)/2,(buffer_height - 50)/2,1.0f,14);
        else
            draw_string_with_shadow(board_list[current_board_index].name,(buffer_width - l)/2,(buffer_height - 50)/2,1.0f,14);

        ++board_title_display_counter;
        if(board_title_display_counter >= 180)
        {
            board_title_display_counter = 0;
            display_board_title = FALSE;
        }
    }

    draw_hud();

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
	window_height = 751;

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
	DWORD window_style = WS_OVERLAPPEDWINDOW;//(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
	
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
    update_game_colors();

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
        case WM_SIZING:
        {
            RECT* screen_coords = lparam;

            int width = (screen_coords->right - screen_coords->left);
            int height = (screen_coords->bottom - screen_coords->top);

            if(width > 0 && height > 0)
            {
                window_width = width;
                window_height = height;
            }

        } break;
		case WM_SIZE:
		{
			if (wparam == SIZE_MAXIMIZED || wparam == SIZE_RESTORED)
			{
                int width = LOWORD(lparam);
                int height = HIWORD(lparam);

                if(width > 0 && height > 0)
                {
                    window_width = width;
                    window_height = height;
                }
			}
		} break;
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
                if(player.weapon.weapon_props.weapon_type == WEAPON_TYPE_MELEE)
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

            if(player.weapon.weapon_props.weapon_type == WEAPON_TYPE_RANGED)
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
            else if(wparam == 'E')
            {
                player.pickup = TRUE;
            }
            else if(wparam == 'R')
            {
                player.take = TRUE;
            }
            else if(wparam == 'Q')
            {
                if(display_inventory)
                    player.unequip = TRUE;
            }
            else if(wparam == VK_SPACE)
            {
                player.jump = TRUE;
            }
            else if(wparam == VK_SHIFT)
            {
                shift_down = TRUE;
                player.phys.base_speed = 2.0f;
            }
            else if(wparam == 'U')
            {
                display_stats = !display_stats;
            }
            else if(wparam == 'I')
            {
                display_inventory = !display_inventory;
                inventory_selection_index = 0;
            }
            else if(wparam == 'M')
            {
                display_map = !display_map;
            }
            else if(wparam == VK_TAB)
            {
                if(shift_down)
                {
                    --inventory_selection_index;
                    if(inventory_selection_index < 0)
                        inventory_selection_index = INVENTORY_SELECTION_MAX -1;
                }
                else
                {
                    ++inventory_selection_index;
                    if(inventory_selection_index == INVENTORY_SELECTION_MAX)
                        inventory_selection_index = 0;
                }
            }
            
            // @DEV
            if(wparam == '1')
            {
                get_item_by_name("Knife",&player.weapon);
            }
            else if(wparam == '2')
            {
                get_item_by_name("Sword",&player.weapon);
            }
            else if(wparam == '3')
            {
                get_item_by_name("Axe",&player.weapon);
            }
            else if(wparam == '4')
            {
                get_item_by_name("Ultimate",&player.weapon);
            }
            else if(wparam == '5')
            {
                get_item_by_name("Bow",&player.weapon);
            }
            else if(wparam == '6')
            {
                get_item_by_name("Staff",&player.weapon);
            }
            else if(wparam == '7')
            {
                get_item_by_name("Crossbow",&player.weapon);
            }
            else if(wparam == 'H')
            {
                // re-load assets
                load_all_assets();

                init_creatures();
                init_items();
            }
            else if(wparam == 'K')
            {
                is_raining = !is_raining;
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
            
            if(player.weapon.weapon_props.weapon_type == WEAPON_TYPE_RANGED)
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
                shift_down = FALSE;
                player.phys.base_speed = 1.0f;
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
