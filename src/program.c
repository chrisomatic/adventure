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
	PLAYER_DIR_UP = 0,
	PLAYER_DIR_DOWN = 3,
	PLAYER_DIR_LEFT = 6,
	PLAYER_DIR_RIGHT = 9
} PlayerDirection;

PlayerDirection player_direction = PLAYER_DIR_DOWN;

BOOL is_running;

POINT curr_pt = {0};
POINT camera  = {0};

int buffer_width;
int buffer_height;

int window_width;
int window_height;

int keypress = 0x0000;

int x_test = 0;
int y_test = 0;

float x_vel = 0;
float y_vel = 0;

int player_animation_counter = 0;
int player_animation_max_count = 10;
int player_animation_frame   = 0;
int player_animation_num_frames = 4;
int player_animation_frame_order[4] = { 0,1,0,2 };

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

	dev_generate_palette_file("data\\16_color_palette.png");
	setup_window(hinstance);

	generate_indexed_tileset("data\\tileset0.png", bmi.acolors);

    init_font("data\\font.png");
	load_tileset("data\\tileset");
    init_world();

	srand(time(NULL));

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
		player_direction = PLAYER_DIR_LEFT;
		x_vel = -1.0f;
	}
	else if ((keypress & KEYPRESS_RIGHT) == KEYPRESS_RIGHT)
	{
		player_direction = PLAYER_DIR_RIGHT;
		x_vel = +1.0f;
	}
	else
		x_vel = 0.0f;

	if ((keypress & KEYPRESS_UP) == KEYPRESS_UP)
	{
		player_direction = PLAYER_DIR_UP;
		y_vel = -1.0f;
	}
	else if ((keypress & KEYPRESS_DOWN) == KEYPRESS_DOWN)
	{
		player_direction = PLAYER_DIR_DOWN;
		y_vel = +1.0f;
	}
	else
		y_vel = 0.0f;

    x_test += x_vel;
    y_test += y_vel;

    if(x_test < 0) x_test = 0;
    if(y_test < 0) y_test = 0;
    if(x_test > buffer_width - TILE_WIDTH) x_test = buffer_width - TILE_WIDTH;
    if(y_test > buffer_height - TILE_HEIGHT) x_test = buffer_height - TILE_HEIGHT;

    // handle player animation
    if(x_vel != 0 || y_vel != 0)
    {
        player_animation_counter++;

        if(player_animation_counter == player_animation_max_count)
        {
            // cycle_animation
            player_animation_counter = 0;
            player_animation_frame += 1;
            if(player_animation_frame >= player_animation_num_frames)
                player_animation_frame = 0;
        }
    }
    else
    {
        // clear animation frame
        player_animation_counter = 0;
        player_animation_frame = 0;
    }

    // handle camera

	if (x_test != buffer_width / 2)
	{
		camera.x += (x_test - (buffer_width / 2));
		camera.x = max(camera.x, 0);

		if (camera.x > 0)
		{
			x_test = buffer_width / 2;
		}
	}

	if (y_test != buffer_height / 2)
	{
		camera.y += (y_test - (buffer_height / 2));
		camera.y = max(camera.y, 0);

		if (camera.y > 0)
		{
			y_test = buffer_height / 2;
		}
	}

}

static void draw_scene()
{
    // Clear buffer to bg_color 
    memset(back_buffer,5, buffer_width*buffer_height*BYTES_PER_PIXEL);
    
    // draw world
    draw_world(camera);

    // draw player
    draw_tile(x_test,y_test,player_direction+player_animation_frame_order[player_animation_frame]);

    // draw debug stuff
    char* button_presses_str = to_string((int)keypress);
    draw_string(button_presses_str,0,buffer_height - 7,1.0f, 8);
    free(button_presses_str);

    char* x_vel_str = to_string((int)x_vel);
    draw_string(x_vel_str,18,buffer_height - 7,1.0f, 8);
    free(x_vel_str);

    char* y_vel_str = to_string((int)y_vel);
    draw_string(y_vel_str,36,buffer_height - 7,1.0f, 8);
    free(y_vel_str);
	//

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
    fp = fopen("data\\palette","r");

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

		while (color_index < 256)
		{
			bmi.acolors[color_index].rgbRed = 0;
			bmi.acolors[color_index].rgbGreen = 0;
			bmi.acolors[color_index].rgbBlue = 0;

			color_index++;
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
