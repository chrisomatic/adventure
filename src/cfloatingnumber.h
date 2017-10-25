#define MAX_FLOATING_NUMBERS 1000

typedef struct
{
    int number;
    float x;
    float y;
    int float_duration_counter;
    int float_duration_counter_max;
    int color;
} FloatingNumber;

FloatingNumber floating_numbers[MAX_FLOATING_NUMBERS];
int num_floating_numbers = 0;

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

static void draw_floating_numbers()
{
    for(int i = 0; i < num_floating_numbers; ++i)
    {
		draw_number_string(floating_numbers[i].number, floating_numbers[i].x - camera.x, floating_numbers[i].y - camera.y, 1.0f, floating_numbers[i].color + 16*(floating_numbers[i].float_duration_counter/10));
    }
}
