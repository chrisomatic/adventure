#define DISPLAY_SCREEN_WIDTH buffer_width/3

BOOL display_stats = FALSE;
BOOL display_inventory = FALSE;

int stat_anim_counter = 0;
int inventory_anim_counter = 0;

int current_fps = 0;

static void draw_message()
{
    // 240, 176
    for(int j = buffer_height - 28; j < buffer_height -7; ++j)
    {
        for(int i = 40; i < buffer_width - 40; ++i)
        {
            shade_pixel8(i,j,10 - day_cycle_shade_amount);
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
            shade_pixel8(i,j,10 - day_cycle_shade_amount);
        }
    } 

    int string_x = stat_anim_counter - DISPLAY_SCREEN_WIDTH;

    draw_string("Stats",string_x+18,5,1.0f,10);

    draw_string("Strength",string_x+2,20,1.0f,10);
    draw_number_string(player.stats.strength,string_x+60,20,1.0f,12);

    draw_string("Dexterity",string_x+2,35,1.0f,10);
    draw_number_string(player.stats.dexterity,string_x+60,35,1.0f,12);

    draw_string("Vitality",string_x+2,50,1.0f,10);
    draw_number_string(player.stats.vitality,string_x+60,50,1.0f,12);

    draw_string("Energy",string_x+2,65,1.0f,10);
    draw_number_string(player.stats.energy,string_x+60,65,1.0f,12);
}

static void draw_inventory()
{
    for(int j = 0; j < buffer_height; ++j)
    {
        for(int i = 0; i < inventory_anim_counter; ++i)
        {
            shade_pixel8(buffer_width - i,j,10 - day_cycle_shade_amount);
        }
    } 

    draw_string("Gear",buffer_width - (inventory_anim_counter - DISPLAY_SCREEN_WIDTH + 50),5,1.0f,10);

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
        draw_char_with_shadow(CHAR_HEART,ui_x,buffer_height -7,3);
        ui_x += 6;
    }

    float num_hearts = player.phys.hp / 2.0f;
    int heart_counter = 0;

    while(num_hearts >= 1)
    {
        draw_char_with_shadow(CHAR_HEART,6*heart_counter, buffer_height -7, 6);
        num_hearts--;
        heart_counter++;
    }

    if(num_hearts > 0)
        draw_char(CHAR_HEART_HALF,6*heart_counter, buffer_height -7, 6);

    ui_x += 10; 
    int diamond_x = ui_x;

    for(int i = 0; i < player.max_mp / 2.0f;++i)
    {
        draw_char_with_shadow(CHAR_DIAMOND,ui_x,buffer_height -7,3);
        ui_x += 6;
    }

    float num_diamonds = player.mp / 2.0f;
    int diamond_counter = 0;

    while(num_diamonds >= 1)
    {
        draw_char_with_shadow(CHAR_DIAMOND,diamond_x + 6*diamond_counter, buffer_height -7, 8);
        num_diamonds--;
        diamond_counter++;
    }
    if(num_diamonds > 0)
        draw_char(CHAR_DIAMOND_HALF,diamond_x  + 6*diamond_counter, buffer_height -7, 8);

    ui_x += 20;
    
    // gold
    draw_string_with_shadow("Gold:",ui_x, buffer_height -7,1.0f,7); ui_x += 30;
    draw_number_string_with_shadow(player.gold,ui_x,buffer_height -7,1.0f,14); ui_x += 30;

    // lvl
    draw_string_with_shadow("Lvl:",ui_x, buffer_height -7,1.0f,7); ui_x += 24;
    draw_number_string_with_shadow(player.lvl,ui_x,buffer_height -7,1.0f,14); ui_x += 18;
    
    // xp
    float xp_percentage = player.xp / next_level;

    draw_string_with_shadow("XP:",ui_x, buffer_height -7,1.0f,7); ui_x += 18;
    draw_rect8(ui_x,buffer_height-6,50,4,1,TRUE);
    draw_rect8(ui_x,buffer_height-6,50*xp_percentage,4,8,TRUE);
   
    // weapon
    draw_string_with_shadow(player.weapon.name,buffer_width - 50,0,1.0f,7);

    draw_string_with_shadow("Dead Foes:",0,0,1.0f,7);
    draw_number_string_with_shadow(foes_killed,60,0,1.0f,8);

    // @TEMP: num rats 
    draw_string_with_shadow("Num Rats:",0,7,1.0f,7);
    draw_number_string_with_shadow(num_creatures,55,7,1.0f,9);
    
    // @TEMP: fps
    draw_number_string_with_shadow(current_fps,buffer_width - 12,7,1.0f,14);

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
