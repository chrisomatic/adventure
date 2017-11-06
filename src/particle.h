#define MAX_PARTICLES 10000

typedef struct
{
    float x;
    float y;
    float x_vel;
    float y_vel;
    int   life_counter;
    int   life_max;
    int   size;
    unsigned char color;
    unsigned char character;
} Particle;

Particle particles[MAX_PARTICLES];
int num_particles;

static void spawn_particle(int x, int y, int size,int intensity,unsigned char character,unsigned char color)
{
    particles[num_particles].x = x;
    particles[num_particles].y = y;
    particles[num_particles].color = color;
    particles[num_particles].character = character;
    particles[num_particles].size = size;
    particles[num_particles].life_counter = 0;
    particles[num_particles].life_max = 30;

    // x -> -.60,.60
    // y -> 1.00,2.50
    
    particles[num_particles].x_vel = (rand() % 120 - 60)/100.0f;
    particles[num_particles].y_vel = (rand() % intensity*50 + 100)/100.0f;

    num_particles++;
    if(num_particles > MAX_PARTICLES -1)
        num_particles = MAX_PARTICLES - 1;
}

static void remove_particle(int index)
{
	num_particles--;
	particles[index] = particles[num_particles];
}

static void update_particles()
{
    for(int i = num_particles - 1; i >= 0; --i)
    {
        // update position
        particles[i].x += particles[i].x_vel;
        particles[i].y -= particles[i].y_vel;

        if(particles[i].life_counter % 10 == 0)
            particles[i].y_vel -= 1.0f;

        particles[i].y_vel = max(0,particles[i].y_vel);

        particles[i].life_counter++;
        if(particles[i].life_counter >= particles[i].life_max)
        {
            remove_particle(i);
        }
    }

}

static void draw_particle(int i)
{
    int particle_x = particles[i].x - camera.x;
    int particle_y = particles[i].y - camera.y;

    int shade_amount = rand() % 6;

    if(particles[i].character > 0)
    {
        draw_char_scaled(particles[i].character,particle_x-1, particle_y-1,particles[i].size, particles[i].color + (16*10));
        draw_char_scaled(particles[i].character,particle_x, particle_y,particles[i].size, particles[i].color + (16*shade_amount));
    }
    else
    {
        for (int j = 0; j < particles[i].size; ++j)
        {
            for (int k = 0; k < particles[i].size;++k)
            {
                draw_pixel8(particle_x+j, particle_y+k,particles[i].color + (16*shade_amount));
            }
        }
    }
}

static void draw_particles()
{
    for(int i = 0; i < num_particles; ++i)
    {
        draw_particle(i);
    }
}
