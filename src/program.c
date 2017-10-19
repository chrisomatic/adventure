#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h> // for GET_X_LPARAM,GET_Y_LPARAM

#include <math.h>     // for abs,atan,cos,sin
#include <time.h>     // for time function
#include "common.h"
#include "cdraw.h"
#include "cutil.h"
#include "cfontsimple.h"
#include "ctimer.h"
#include "ctile.h"
#include "cworld.h"

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
	DIR_UP = 0,
	DIR_DOWN = 3,
	DIR_LEFT = 6,
	DIR_RIGHT = 9
} Direction;

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
    char* name;
    int tile_index;
    int lvl;
    int xp;
    int hp;
    int max_hp;
    int x;
    int y;
    int x_vel;
    int y_vel;
    Direction dir;
    Animation anim;
} Player;
Player player;

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
    int action_counter;
    Direction dir;
    Animation anim;
} Enemy;
Enemy enemies[100];
int num_enemies;

int action_counter_max = 60;

BOOL is_running;

POINT curr_pt = {0};
POINT camera  = {0};

int buffer_width;
int buffer_height;

int window_width;
int window_height;

int keypress = 0x0000;

const int BYTES_PER_PIXEL = 1;

double TARGET_FPS = 60.0f;

// prototypes
static LRESULT CALLBACK MainWndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);
static void setup_window(HINSTANCE hInstance);
static void update_scene();
static void draw_scene();
static void init_enemies();
static void init_player();
static BOOL set_working_directory();

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, s32 nshowcmd)
{
	set_working_directory();

	dev_generate_palette_file("data\\16_color_palette.png");
	setup_window(hinstance);

	generate_indexed_tileset("data\\tileset0.png", bmi.acolors);

	srand(time(NULL));

    init_font("data\\font.png");
	load_tileset("data\\tileset");

    init_world();
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

    // OS should free memory so don't worry about this stuff
	//free_font();
	//free(back_buffer);
	//ReleaseDC(main_window, dc);

	return EXIT_SUCCESS;
}

static void update_scene()
{
    // handle player
    // handle player movement
	if ((keypress & KEYPRESS_LEFT) == KEYPRESS_LEFT)
	{
		player.dir = DIR_LEFT;
		player.x_vel = -1.0f;
	}
	else if ((keypress & KEYPRESS_RIGHT) == KEYPRESS_RIGHT)
	{
		player.dir = DIR_RIGHT;
		player.x_vel = +1.0f;
	}
	else
		player.x_vel = 0.0f;

	if ((keypress & KEYPRESS_UP) == KEYPRESS_UP)
	{
		player.dir = DIR_UP;
		player.y_vel = -1.0f;
	}
	else if ((keypress & KEYPRESS_DOWN) == KEYPRESS_DOWN)
	{
		player.dir = DIR_DOWN;
		player.y_vel = +1.0f;
	}
	else
		player.y_vel = 0.0f;

    player.x += player.x_vel;
    player.y += player.y_vel;

    if(player.x < 0) player.x = 0;
    if(player.y < 0) player.y = 0;
    if(player.x > buffer_width - TILE_WIDTH) player.x = buffer_width - TILE_WIDTH;
    if(player.y > buffer_height - TILE_HEIGHT) player.x = buffer_height - TILE_HEIGHT;

    // handle player animation
    if(player.x_vel != 0 || player.y_vel != 0)
    {
        player.anim.counter++;

        if(player.anim.counter == player.anim.max_count)
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
        player.anim.counter = 0;
        player.anim.frame = 0;
    }

    // handle enemies
    for(int i = 0; i < num_enemies;++i)
    {
        enemies[i].action_counter++;

        if(enemies[i].action_counter >= action_counter_max)
        {
            enemies[i].action_counter = 0;
            
            // take an action
            int d = rand() % 4;
            switch (d)
            {
                case 0: 
                    enemies[i].dir = DIR_UP;
                    break;
                case 1: 
                    enemies[i].dir = DIR_DOWN;
                    break;
                case 2: 
                    enemies[i].dir = DIR_LEFT;
                    break;
                case 3: 
                    enemies[i].dir = DIR_RIGHT;
                    break;
            }
        }
    }

    // handle camera
	if (player.x != buffer_width / 2)
	{
		camera.x += (player.x - (buffer_width / 2));
		camera.x = max(camera.x, 0);

		if (camera.x > 0)
		{
			player.x = buffer_width / 2;
		}
	}

	if (player.y != buffer_height / 2)
	{
		camera.y += (player.y - (buffer_height / 2));
		camera.y = max(camera.y, 0);

		if (camera.y > 0)
		{
			player.y = buffer_height / 2;
		}
	}

}

static void draw_scene()
{
    // Clear buffer to bg_color 
    memset(back_buffer,5, buffer_width*buffer_height*BYTES_PER_PIXEL);
    
    // draw world
    draw_world(camera);
    
     // draw player shadow
    int shadow_y = player.y + TILE_HEIGHT - 3;

    for(int i = 7; i > 0; --i)
    {
		for (int j = 1; j < 15; ++j)
        {
            shade_pixel8(player.x + j,shadow_y,i);
        }
        shadow_y++;
    }
    
	// draw enemies
	for (int i = 0; i < num_enemies; ++i)
	{
		draw_tile(enemies[i].x - camera.x, enemies[i].y - camera.y, enemies[i].tile_index + enemies[i].dir + enemies[i].anim.frame_order[enemies[i].anim.frame]);
	}

    // draw player
    draw_tile(player.x,player.y,player.tile_index + player.dir+player.anim.frame_order[player.anim.frame]);

    // draw UI
    draw_string(player.name,0,buffer_height - 7,1.0f,1);

    char* x_vel_str = to_string((int)player.hp);
    draw_string(x_vel_str,26,buffer_height - 7,1.0f, 9);
    free(x_vel_str);

    char* y_vel_str = to_string((int)player.max_hp);
    draw_string(y_vel_str,40,buffer_height - 7,1.0f, 9);
    free(y_vel_str);
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
    player.x   = (buffer_width-TILE_WIDTH)/2;
    player.y   = (buffer_height-TILE_HEIGHT)/2;
    player.x_vel = 0;
    player.y_vel = 0;
    player.dir = DIR_DOWN;
    player.anim.counter = 0;
    player.anim.max_count = 10;
    player.anim.frame = 0;
    player.anim.num_frames = 4;
    player.anim.frame_order[0] = 0;
    player.anim.frame_order[1] = 1;
    player.anim.frame_order[2] = 0;
    player.anim.frame_order[3] = 2;
}

static void init_enemies()
{

    for (int i = 0; i < 100; ++i)
    {
        Enemy e = {0};

        enemies[num_enemies].x = rand() % (TILE_WIDTH*(WORLD_TILE_WIDTH -1));
        enemies[num_enemies].y = rand() % (TILE_HEIGHT*(WORLD_TILE_HEIGHT -1));
        enemies[num_enemies].x_vel = 0;
        enemies[num_enemies].y_vel = 0;
        enemies[num_enemies].dir = DIR_DOWN;
        enemies[num_enemies].name = "Rat";
        enemies[num_enemies].tile_index = 16;
        enemies[num_enemies].hp = 5;
        enemies[num_enemies].max_hp = 5;
        enemies[num_enemies].xp = 1;
        enemies[num_enemies].action_counter = rand() % action_counter_max;
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

          color_index++;
          if(feof(fp))
             break;
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
    
        case WM_LBUTTONDOWN:
        {
            POINT pt;

            pt.x = GET_X_LPARAM(lparam);
            pt.y = GET_Y_LPARAM(lparam);

            break;
        } 
        
        case WM_MOUSEMOVE:
        {
            POINT pt;
            pt.x = GET_X_LPARAM(lparam);
            pt.y = GET_Y_LPARAM(lparam);

            curr_pt.x = pt.x;
            curr_pt.y = pt.y;

            break;
        } 
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

			break;
		}
        case WM_SETCURSOR:
        {
			/*
            static BOOL HideCursor = FALSE;
            if ((LOWORD(lparam) == HTCLIENT) && !HideCursor)
            {
                HideCursor = TRUE;
                ShowCursor(FALSE);
            }
			else if ((LOWORD(lparam) != HTCLIENT) && HideCursor)
            {
                HideCursor = FALSE;
                ShowCursor(TRUE);
            }
			*/
            break;
        } 
        
        default:
        {
            break;
        }
	}

	return DefWindowProc(hwnd, umsg, wparam, lparam);
}
