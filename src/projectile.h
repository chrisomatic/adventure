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
    int board_index;
    PhysicalProperties phys;
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
    projectiles[num_projectiles].phys.x = x;
    projectiles[num_projectiles].phys.y = y;
    projectiles[num_projectiles].phys.z = z;
    projectiles[num_projectiles].phys.height = 2;
    projectiles[num_projectiles].phys.x_vel = x_vel;
    projectiles[num_projectiles].phys.y_vel = y_vel;
    projectiles[num_projectiles].phys.z_vel = z_vel;
    projectiles[num_projectiles].phys.speed = 1.0f;
    projectiles[num_projectiles].phys.base_speed = 1.0f;
    projectiles[num_projectiles].board_index = current_board_index;
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

        projectiles[i].phys.x += projectiles[i].phys.x_vel*projectiles[i].phys.speed;
        projectiles[i].phys.y += projectiles[i].phys.y_vel*projectiles[i].phys.speed;
        projectiles[i].phys.z += projectiles[i].phys.z_vel*projectiles[i].phys.speed;

        handle_terrain_collision(projectiles[i].board_index, &projectiles[i].phys);

		if (projectiles[i].phys.z < 0)
        {
            // hit the ground
            projectiles[i].phys.z = 0.0f;
            projectiles[i].phys.z_vel = 0.0f;
            projectiles[i].friction = GROUND_FRICTION;
        }
        else
        {
            projectiles[i].phys.z_vel -= GRAVITY; // gravity acceleration
        }

        if(projectiles[i].phys.x_vel != 0.0f || projectiles[i].phys.y_vel != 0.0f)
        {
            // friction
            if(projectiles[i].phys.x_vel < 0) projectiles[i].phys.x_vel += projectiles[i].friction;
            else projectiles[i].phys.x_vel -= projectiles[i].friction;

            if(projectiles[i].phys.y_vel < 0) projectiles[i].phys.y_vel += projectiles[i].friction;
            else projectiles[i].phys.y_vel -= projectiles[i].friction;

            if(abs(projectiles[i].phys.x_vel) <= 0.2f) projectiles[i].phys.x_vel = 0.0f;
            if(abs(projectiles[i].phys.y_vel) <= 0.2f) projectiles[i].phys.y_vel = 0.0f;
        }
        
        // check collisions with creatures
		if(projectiles[i].shot)
		{
			for(int j = 0; j < num_creatures; ++j)
			{
                if(creatures[j].untargetable)
                    continue;

                if(creatures[j].board_index != current_board_index)
                    continue;

				double distance = get_distance(projectiles[i].phys.x + TILE_WIDTH / 2, projectiles[i].phys.y + TILE_HEIGHT / 2, creatures[j].phys.x + TILE_WIDTH / 2, creatures[j].phys.y + TILE_HEIGHT / 2);
				if(distance <= 20)
				{
					if(are_entities_colliding(&projectiles[i].phys, &creatures[j].phys))
					{
                        int damage = (rand() % (player.weapon.weapon_props.max_damage - player.weapon.weapon_props.min_damage + 1)) + player.weapon.weapon_props.min_damage;
                        
                        // add floating number
                        spawn_floating_number(projectiles[i].phys.x,projectiles[i].phys.y,damage,6);
                        
                        // creature hurt!
                        creatures[j].phys.hp -= damage;

                        // check if creature died
                        if (creatures[j].phys.hp <= 0)
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
                            creatures[j].stunned = TRUE;

                            if(creatures[j].behavior == CREATURE_BEHAVIOR_PASSIVE)
                            {
                                creatures[j].behavior = CREATURE_BEHAVIOR_AGGRESSIVE;
                                creatures[i].deaggress = TRUE;
                            }
                        }
						remove_projectile(i);
					}
				}
			}
		}
        if(projectiles[i].shot && projectiles[i].phys.z == 0.0f)
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
            spawn_particle(rand() % (TILE_WIDTH/2) + projectiles[i].phys.x + TILE_WIDTH/4,projectiles[i].phys.y+TILE_HEIGHT/2,rand() % 3 + 1,3,0,6,current_board_index);
            spawn_particle(rand() % TILE_WIDTH + projectiles[i].phys.x,projectiles[i].phys.y,rand()%2+1,2,0,3, current_board_index);
            break;
    }
		draw_tile_rotated_shadow(projectiles[i].phys.x - camera.x, projectiles[i].phys.y - camera.y,projectile_tileset_name, projectiles[i].tile_index,projectiles[i].angle,max(0,10 - day_cycle_shade_amount)); // shadow
        draw_tile_rotated(projectiles[i].phys.x - camera.x, projectiles[i].phys.y - camera.y - projectiles[i].phys.z*0.5f,projectile_tileset_name, projectiles[i].tile_index,projectiles[i].angle,day_cycle_shade_amount);
}
