#pragma once

#define MAX_FLOATING_NUMBERS 10000

typedef struct
{
    int number;
	int board_index;
    float x;
    float y;
    int float_duration_counter;
    int float_duration_counter_max;
    int color;
    BOOL is_string;
    char* string;
} FloatingNumber;

FloatingNumber floating_numbers[MAX_FLOATING_NUMBERS];
int num_floating_numbers = 0;

static void spawn_floating_number(float x, float y, int number,int color,int board_index)
{
    floating_numbers[num_floating_numbers].number = number;
    floating_numbers[num_floating_numbers].x = x;
    floating_numbers[num_floating_numbers].y = y; 
    floating_numbers[num_floating_numbers].color = color;
    floating_numbers[num_floating_numbers].float_duration_counter = 0;
    floating_numbers[num_floating_numbers].float_duration_counter_max = 60;
    floating_numbers[num_floating_numbers].is_string = FALSE;
	floating_numbers[num_floating_numbers].board_index = board_index;

    num_floating_numbers++;
    if(num_floating_numbers > MAX_FLOATING_NUMBERS-1)
        num_floating_numbers = MAX_FLOATING_NUMBERS -1;
}

static void spawn_floating_string(float x, float y, char* string, int color, int board_index)
{
    floating_numbers[num_floating_numbers].string = string;
    floating_numbers[num_floating_numbers].x = x;
    floating_numbers[num_floating_numbers].y = y; 
    floating_numbers[num_floating_numbers].color = color;
    floating_numbers[num_floating_numbers].float_duration_counter = 0;
    floating_numbers[num_floating_numbers].float_duration_counter_max = 60;
    floating_numbers[num_floating_numbers].is_string = TRUE;
	floating_numbers[num_floating_numbers].board_index = board_index;

    num_floating_numbers++;
    if(num_floating_numbers > MAX_FLOATING_NUMBERS-1)
        num_floating_numbers = MAX_FLOATING_NUMBERS -1;
}

static void remove_floating_number(int index)
{
	num_floating_numbers--;
	floating_numbers[index] = floating_numbers[num_floating_numbers];
}

static void update_floating_numbers()
{
    for(int i = num_floating_numbers -1; i >= 0; --i)
    {
        floating_numbers[i].y -= 0.25f;
        floating_numbers[i].float_duration_counter++;

        if(floating_numbers[i].float_duration_counter >= floating_numbers[i].float_duration_counter_max)
        {
            remove_floating_number(i);
        }
    }
}

static void draw_floating_number(int i)
{
	if (floating_numbers[i].board_index != current_board_index)
		return;

    int number_x = floating_numbers[i].x - camera.x;
    int number_y = floating_numbers[i].y - camera.y;

    if(floating_numbers[i].is_string)
    {
        draw_string(floating_numbers[i].string, number_x+1, number_y+1, 1.0f, 254);
        draw_string(floating_numbers[i].string, number_x, number_y, 1.0f, floating_numbers[i].color + 16*(floating_numbers[i].float_duration_counter/10));
    }
    else
    {
        draw_number_string(floating_numbers[i].number, number_x+1, number_y+1, 1.0f, 254);
        draw_number_string(floating_numbers[i].number, number_x, number_y, 1.0f, floating_numbers[i].color + 16*(floating_numbers[i].float_duration_counter/10));
    }
}

static void draw_floating_numbers()
{
    for(int i = 0; i < num_floating_numbers; ++i)
    {
        draw_floating_number(i);
    }
}
