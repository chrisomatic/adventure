#define DISPLAY_SCREEN_WIDTH buffer_width/3
#define INVENTORY_SELECTION_MAX 5 

BOOL display_stats = FALSE;
BOOL display_inventory = FALSE;

int stat_anim_counter = 0;
int inventory_anim_counter = 0;
int inventory_selection_index = 0;

int current_fps = 0;

static void draw_message()
{
    // 240, 176
    for(int j = buffer_height - 28; j < buffer_height -7; ++j)
    {
        for(int i = 40; i < buffer_width - 40; ++i)
        {
            shade_pixel8(i,j,12 - day_cycle_shade_amount);
        }
    } 

    draw_string(message.name,42, 149,1.0f,8);
    draw_string(message.message,50,156,1.0f,message.color);

    if(message.value > 0)
    {
        draw_char(CHAR_COIN,buffer_width - 40 - 18,149,14);
        draw_number_string(message.value,buffer_width - 40 - 12,149,1.0f,14);
    }
}

static void draw_stats()
{
    for(int j = 0; j < buffer_height; ++j)
    {
        for(int i = 0; i < stat_anim_counter; ++i)
        {
            shade_pixel8(i,j,12 - day_cycle_shade_amount);
        }
    } 

    int string_x = stat_anim_counter - DISPLAY_SCREEN_WIDTH;

    draw_string("Stats",string_x+25,5,1.0f,10);

    int string_y = 20;

    draw_string("Name:",string_x+2,string_y,1.0f,10);
    draw_string(player.name,string_x+30,string_y,1.0f,12);

    string_y += 10;
    draw_string("Level:",string_x+2,string_y,1.0f,10);
    draw_number_string(player.lvl,string_x+36,string_y,1.0f,12);

    float xp_percentage = player.xp / next_level;

    string_y += 10;
    draw_string_with_shadow("XP:",string_x+2, string_y,1.0f,7); 
    if(stat_anim_counter >= DISPLAY_SCREEN_WIDTH)
    {
        draw_rect8(string_x+20,string_y,50,4,1,TRUE);
        draw_rect8(string_x+20,string_y,50*xp_percentage,4,8,TRUE);
    }

    string_y += 10;
    draw_string("Damage:",string_x+2,string_y,1.0f,10);
    draw_number_string(player.weapon.weapon_props.min_damage,string_x+60,string_y,1.0f,12);

    string_y += 10;
    draw_string("Defence",string_x+2,string_y,1.0f,10);
    draw_number_string(player.armor_head.armor_props.defence,string_x+60,string_y,1.0f,12);

    string_y += 10;
    draw_string("Stat Points:",string_x+2,string_y,1.0f,10);
    draw_number_string(player.available_stat_points,string_x+60,string_y,1.0f,12);

    string_y += 20;
    draw_string("Strength",string_x+2,string_y,1.0f,10);
    draw_number_string(player.stats.strength,string_x+60,string_y,1.0f,12);

    string_y += 10;
    draw_string("Dexterity",string_x+2,string_y,1.0f,10);
    draw_number_string(player.stats.dexterity,string_x+60,string_y,1.0f,12);

    string_y += 10;
    draw_string("Vitality",string_x+2,string_y,1.0f,10);
    draw_number_string(player.stats.vitality,string_x+60,string_y,1.0f,12);

    string_y += 10;
    draw_string("Energy",string_x+2,string_y,1.0f,10);
    draw_number_string(player.stats.energy,string_x+60,string_y,1.0f,12);
}

static void draw_inventory()
{
    for(int j = 0; j < buffer_height; ++j)
    {
        for(int i = 0; i < inventory_anim_counter; ++i)
        {
            shade_pixel8(buffer_width - i,j,12 - day_cycle_shade_amount);
        }
    } 

    int string_x = buffer_width - inventory_anim_counter;

    draw_string("Gear",string_x +30,5,1.0f,10);

    int string_y = 20;

	if (inventory_anim_counter >= DISPLAY_SCREEN_WIDTH)
		draw_rect8(string_x + 59, 20 + inventory_selection_index * 16 - 5, TILE_WIDTH+2, TILE_HEIGHT+2, 11, TRUE);

    draw_string("Weapon",string_x + 5,string_y,1.0f,10);
    draw_tile(string_x + 60,string_y -4,player.weapon.tileset_name,player.weapon.tile_index,0);

    string_y += 16;
    draw_string("Head",string_x + 5,string_y,1.0f,10);
    if(player.armor_head.name != NULL)
        draw_tile(string_x + 60,string_y -4,player.armor_head.tileset_name,player.armor_head.tile_index,0);
    string_y += 16;
    draw_string("Body",string_x + 5,string_y,1.0f,10);
    if(player.armor_body.name != NULL)
        draw_tile(string_x + 60,string_y -4,player.armor_body.tileset_name,player.armor_body.tile_index,0);
    string_y += 16;
    draw_string("Feet",string_x + 5,string_y,1.0f,10);
    if(player.armor_feet.name != NULL)
        draw_tile(string_x + 60,string_y -4,player.armor_feet.tileset_name,player.armor_feet.tile_index,0);
    string_y += 16;
    draw_string("Hands",string_x + 5,string_y,1.0f,10);
    if(player.armor_hands.name != NULL)
        draw_tile(string_x + 60,string_y -4,player.armor_hands.tileset_name,player.armor_hands.tile_index,0);

    string_y += 16;
    draw_char(CHAR_COIN,string_x + 5,string_y,14);
    draw_number_string(player.gold,string_x + 11,string_y,1.0f,14);
	
}

static void update_hud()
{
    int anim_speed = 4;

    if(display_stats)
    {
        if(stat_anim_counter < DISPLAY_SCREEN_WIDTH)
            stat_anim_counter += anim_speed;
    }
    else
    {
        if(stat_anim_counter > 0)
            stat_anim_counter -= anim_speed;
    }
    
    if(display_inventory)
    {
        if(inventory_anim_counter < DISPLAY_SCREEN_WIDTH)
            inventory_anim_counter += anim_speed;
    }
    else
    {
        if(inventory_anim_counter > 0)
            inventory_anim_counter -= anim_speed;
    }

}

static void draw_hud()
{
    if(message.active)
        draw_message();

    // health
    int ui_x = 0;

    for(int i = 0; i < player.phys.max_hp / 2.0f;++i)
    {
        draw_char_with_shadow(CHAR_HEART,ui_x,1,3);
        ui_x += 6;
    }

    float num_hearts = player.phys.hp / 2.0f;
    int heart_counter = 0;

    while(num_hearts >= 1)
    {
        draw_char_with_shadow(CHAR_HEART,6*heart_counter, 1, 6);
        num_hearts--;
        heart_counter++;
    }

    if(num_hearts > 0)
        draw_char(CHAR_HEART_HALF,6*heart_counter,1, 6);

    ui_x += 10; 
    int diamond_x = ui_x;

    for(int i = 0; i < player.max_mp / 2.0f;++i)
    {
        draw_char_with_shadow(CHAR_DIAMOND,ui_x,1,3);
        ui_x += 6;
    }

    float num_diamonds = player.mp / 2.0f;
    int diamond_counter = 0;

    while(num_diamonds >= 1)
    {
        draw_char_with_shadow(CHAR_DIAMOND,diamond_x + 6*diamond_counter,1, 8);
        num_diamonds--;
        diamond_counter++;
    }
    if(num_diamonds > 0)
        draw_char(CHAR_DIAMOND_HALF,diamond_x  + 6*diamond_counter,1, 8);

    ui_x += 20;
    
    // @TEMP: fps
    draw_number_string_with_shadow(current_fps,buffer_width - 12,buffer_height -7,1.0f,14);

    // @TEMP: debugging
    //for(int i = 0; i < num_npcs; ++i)
    //{
    //    draw_bounding_box(&npcs[i].phys);
    //}

    //if(display_stats)
    draw_stats();

    //if(display_inventory)
    draw_inventory();

}
