#define WIN32_LEAN_AND_MEAN
#define GLOBAL_FONT_SCALE 2.0f
#define MAX_ITEMS 10

#include <windows.h>
#include <windowsx.h> // for GET_X_LPARAM,GET_Y_LPARAM

#include <math.h>     // for abs,atan,cos,sin
#include <time.h>     // for time function
#include "common.h"
#include "cdraw.h"
#include "cutil.h"
#include "cfontsimple.h"

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

POINT curr_pt = {0};

int buffer_width;
int buffer_height;

int window_width;
int window_height;

const int BYTES_PER_PIXEL = 1;

double TARGET_FPS = 100.0f;

// prototypes
static LRESULT CALLBACK MainWndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);
static void setup_window(HINSTANCE hInstance);
static void update_scene();
static void draw_scene();
static BOOL set_working_directory();

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, s32 nshowcmd)
{
	set_working_directory();
	setup_window(hinstance);
    init_font("font.png");

	srand(time(NULL));

    // init_timer
	LARGE_INTEGER freq;
	LARGE_INTEGER t0, t1;

	QueryPerformanceFrequency(&freq);

	double seconds_per_tick = 1.0f / (double)freq.QuadPart;
	double target_spf = 1.0f / TARGET_FPS;
	double accum_time = 0.0;

	QueryPerformanceCounter(&t0);
    // end
    
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
        
        // timer query
		QueryPerformanceCounter(&t1);
		__int64 interval = t1.QuadPart - t0.QuadPart;
		double seconds_gone_by = seconds_per_tick * (double)interval;

		accum_time += seconds_gone_by;

		if (accum_time >= target_spf)
		{
			accum_time -= target_spf;

            update_scene();
            draw_scene();

            // Blit to screen
            StretchDIBits(dc, 0, 0, window_width, window_height, 0, 0, window_width, window_height, back_buffer, (BITMAPINFO*)&bmi, DIB_RGB_COLORS, SRCCOPY);
        }

		QueryPerformanceCounter(&t0);
	}

	free_font();
	//free(back_buffer);
	ReleaseDC(main_window, dc);

	return EXIT_SUCCESS;
}

static void update_scene()
{

}

static void draw_scene()
{
    // Clear buffer to bg_color 
    memset(back_buffer,0, buffer_width*buffer_height*BYTES_PER_PIXEL);
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
	buffer_width = 1024;
	buffer_height = 768;

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

    // generate palette colors
    for (int i = 0; i < 256; ++i)
    {
        bmi.acolors[i].rgbRed   = rand() % 256;
        bmi.acolors[i].rgbGreen = rand() % 256; 
        bmi.acolors[i].rgbBlue  = rand() % 256;
    }

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
            break;
        } 
        case WM_SETCURSOR:
        {
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
            break;
        } 
        
        default:
        {
            break;
        }
	}

	return DefWindowProc(hwnd, umsg, wparam, lparam);
}
