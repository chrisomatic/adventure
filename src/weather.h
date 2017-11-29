BOOL is_raining = FALSE;

int weather_spawn_counter = 0;
int weather_spawn_counter_max = 1;

static void update_weather()
{
    if(is_raining)
	{
        ++weather_spawn_counter;
        if(weather_spawn_counter == weather_spawn_counter_max)
        {
            weather_spawn_counter = 0;

			int x = (rand() % buffer_width) + (player.phys.x - buffer_width / 2);
			int y = (rand() % buffer_height) + (player.phys.y - buffer_height / 2);

            spawn_particle(x,y,rand()%2,2,0,8,current_board_index);
        }
    }
}
