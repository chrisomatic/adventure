#define MAX_PROJECTILES 1000
#define ARROW 4
#define FIREBALL 6

const char* projectile_tileset_name = "weapons";

typedef struct
{
    int tile_index;
    int life_counter;
    int life_span;
    float angle;
    float damage_modifier;
    float friction;
    float x;
    float y;
    float z;
    float height;
    float x_vel;
    float y_vel;
    float z_vel;
	BOOL shot;
    BOOL collision;
} Projectile;

Projectile projectiles[MAX_PROJECTILES];
static int num_projectiles = 0;

static int spawn_projectile(float x, float y, float z, float x_vel, float y_vel, float z_vel, int tile_index, float rotation_angle, float damage_modifier)
{
    projectiles[num_projectiles].tile_index = tile_index;
    projectiles[num_projectiles].angle = rotation_angle;
    projectiles[num_projectiles].damage_modifier = damage_modifier;
    projectiles[num_projectiles].friction = AIR_RESISTANCE;
    projectiles[num_projectiles].x = x;
    projectiles[num_projectiles].y = y;
    projectiles[num_projectiles].z = z;
    projectiles[num_projectiles].height = 2;
    projectiles[num_projectiles].x_vel = x_vel;
    projectiles[num_projectiles].y_vel = y_vel;
    projectiles[num_projectiles].z_vel = z_vel;
	projectiles[num_projectiles].shot = FALSE;
    projectiles[num_projectiles].life_counter = 0;
    projectiles[num_projectiles].life_span = 180;
	
    num_projectiles++;

    if(num_projectiles > MAX_PROJECTILES -1)
    {
        num_projectiles = MAX_PROJECTILES - 1;
        return -1;
    }

    return num_projectiles-1;
}

static void remove_projectile(int i)
{
	num_projectiles--;
	projectiles[i] = projectiles[num_projectiles];

    // update notched_index
    if(player.notch_index == num_projectiles)
        player.notch_index = i;
}

static void update_projectiles()
{
    for(int i = num_projectiles -1; i >= 0;--i)
    {

        projectiles[i].x += projectiles[i].x_vel;
        projectiles[i].y += projectiles[i].y_vel;
        projectiles[i].z += projectiles[i].z_vel;

        if(projectiles[i].z < 0)
        {
            // hit the ground
            projectiles[i].z = 0.0f;
            projectiles[i].z_vel = 0.0f;
            projectiles[i].friction = GROUND_FRICTION;
        }
        else
        {
            projectiles[i].z_vel -= GRAVITY; // gravity acceleration
        }

        if(projectiles[i].x_vel != 0.0f || projectiles[i].y_vel != 0.0f)
        {
            // friction
            if(projectiles[i].x_vel < 0) projectiles[i].x_vel += projectiles[i].friction;
            else projectiles[i].x_vel -= projectiles[i].friction;

            if(projectiles[i].y_vel < 0) projectiles[i].y_vel += projectiles[i].friction;
            else projectiles[i].y_vel -= projectiles[i].friction;

            if(abs(projectiles[i].x_vel) <= 0.2f) projectiles[i].x_vel = 0.0f;
            if(abs(projectiles[i].y_vel) <= 0.2f) projectiles[i].y_vel = 0.0f;
        }
        
        // check collisions with creatures
		if(projectiles[i].shot)
		{
			for(int j = 0; j < num_creatures; ++j)
			{
				double distance = get_distance(projectiles[i].x + TILE_WIDTH / 2, projectiles[i].y + TILE_HEIGHT / 2, creatures[j].x + TILE_WIDTH / 2, creatures[j].y + TILE_HEIGHT / 2);
				if(distance <= 20)
				{
					BoundingBox a,b;

					a.x = projectiles[i].x + TILE_WIDTH/4.0f;
					a.y = projectiles[i].y + TILE_HEIGHT/4.0f;
					a.z = projectiles[i].z;

					a.width = TILE_WIDTH/2.0f;
					a.length = TILE_HEIGHT/2.0f;
					a.height = projectiles[i].height;

					b.x = creatures[j].x;
					b.y = creatures[j].y;
					b.z = creatures[j].z;

					b.width = TILE_WIDTH;
					b.length = TILE_HEIGHT/2.0f;
					b.height = creatures[j].height;

					if(are_entities_colliding(&a, &b))
					{
                        int damage = (rand() % (player.weapon.weapon_props.max_damage - player.weapon.weapon_props.min_damage + 1)) + player.weapon.weapon_props.min_damage;
                        
                        // add floating number
                        spawn_floating_number(projectiles[i].x,projectiles[i].y,damage,6);
                        
                        // creature hurt!
                        creatures[j].hp -= damage;

                        // check if creature died
                        if (creatures[j].hp <= 0)
                        {
                            foes_killed++;
                            
                            // player get xp
                            player.xp += creatures[j].xp;
                            if(player.xp >= next_level)
                            {
                                gain_level();
                            }

                            creature_death(j);

                        }
                        else
                        {
                            creatures[j].state = CREATURE_STATE_STUNNED;
                        }
						remove_projectile(i);
					}
				}
			}
		}
        if(projectiles[i].shot && projectiles[i].z == 0.0f)
        {
            // projectile on ground, count down to remove
            ++projectiles[i].life_counter;
            if(projectiles[i].life_counter >= projectiles[i].life_span)
            {
                remove_projectile(i);
            }
        }
    }
}

static void draw_projectile(int i)
{
    switch(projectiles[i].tile_index)
    {
        case ARROW:
            break;
        case FIREBALL:
            spawn_particle(rand() % (TILE_WIDTH/2) + projectiles[i].x + TILE_WIDTH/4,projectiles[i].y+TILE_HEIGHT/2,rand() % 3 + 1,3,0,6,current_board_index);
            spawn_particle(rand() % TILE_WIDTH + projectiles[i].x,projectiles[i].y,rand()%2+1,2,0,3, current_board_index);
            break;
    }
		draw_tile_rotated_shadow(projectiles[i].x - camera.x, projectiles[i].y - camera.y,projectile_tileset_name, projectiles[i].tile_index,projectiles[i].angle,max(0,10 - day_cycle_shade_amount)); // shadow
        draw_tile_rotated(projectiles[i].x - camera.x, projectiles[i].y - camera.y - projectiles[i].z*0.5f,projectile_tileset_name, projectiles[i].tile_index,projectiles[i].angle,day_cycle_shade_amount);
}
