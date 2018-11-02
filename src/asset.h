typedef enum
{
	ASSET_TYPE_CREATURE,
	ASSET_TYPE_ITEM,
    ASSET_TYPE_ZONE
} ASSET_TYPE;

static void load_assets(ASSET_TYPE type);
static void load_all_assets();

static void load_all_assets()
{
    load_assets(ASSET_TYPE_CREATURE);
    load_assets(ASSET_TYPE_ITEM);
    load_assets(ASSET_TYPE_ZONE);
}

static void load_assets(ASSET_TYPE type)
{
    char* directory;
    char* file_extension;

    switch(type)
    {
        case ASSET_TYPE_CREATURE:
            directory = "data\\creatures";
            file_extension = ".creature";
            break;
        case ASSET_TYPE_ITEM:
            directory = "data\\items";
            file_extension = ".item";
            break;
        case ASSET_TYPE_ZONE:
            directory = "data\\zones";
            file_extension = ".zone";
            break;
        default:
            return;
    }

	char paths[100][MAX_PATH];
    for(int i = 0; i < 100; ++i)
    {
        memset(paths[i],0,MAX_PATH);
    }

	int num_files = get_files_in_directory_with_extension(directory, file_extension, paths);

	int asset_counter = 0;

    for(int i = 0; i < num_files; ++i)
    {
        // open file
        FILE* fp = fopen(paths[i],"r");
        
        // Expected:
        // key : value \n
        // key : value \n
        // ...
        // key : value EOF

        BOOL is_key = TRUE;
        BOOL is_string = FALSE;

        int ikey = 0;
        int ivalue = 0;

        char key[100] = { 0 };
        char value[100] = { 0 };

        int c;
        do
        {
            c = fgetc(fp);
            if (c == '#')
            {
                // comment, read til end of line or end of file, whichever comes first.
                while(c != EOF && c != '\n')
                    c = fgetc(fp);
            }
            
			if (c == '"')
            {
                is_string = !is_string;
            }
            else if (c == ':')
            {
                is_key = FALSE;
                key[ikey] = '\0';
                ikey = 0;
            }
            else if (c == '\n' || c == EOF)
            {
                is_key = TRUE;
                ivalue = 0;
                
                switch(type)
                {
                    case ASSET_TYPE_CREATURE:
						if (strcmp(key, "name") == 0) creature_list[asset_counter].name = _strdup(value);
                        else if (strcmp(key, "x_offset") == 0) C_atoi(value, &creature_list[asset_counter].phys.x_offset);
                        else if (strcmp(key, "y_offset") == 0) C_atoi(value, &creature_list[asset_counter].phys.y_offset);
						else if (strcmp(key, "species") == 0) creature_list[asset_counter].species = _strdup(value);
                        else if (strcmp(key, "hp") == 0) C_atoi(value, &creature_list[asset_counter].phys.hp);
                        else if (strcmp(key, "max_hp") == 0) C_atoi(value, &creature_list[asset_counter].phys.max_hp);
                        else if (strcmp(key, "xp") == 0) C_atoi(value, &creature_list[asset_counter].xp);
                        else if (strcmp(key, "speed") == 0) creature_list[asset_counter].phys.speed = atof(value);
                        else if (strcmp(key, "gold_drop_max") == 0) C_atoi(value, &creature_list[asset_counter].gold_drop_max);
                        else if (strcmp(key, "behavior") == 0) C_atoi(value, &creature_list[asset_counter].behavior);
                        else if (strcmp(key, "untargetable") == 0) C_atoi(value, &creature_list[asset_counter].untargetable);
						else if (strcmp(key, "tileset_name") == 0) creature_list[asset_counter].tileset_name = _strdup(value);
                        else if (strcmp(key, "tile_index") == 0) C_atoi(value, &creature_list[asset_counter].tile_index);
                        else if (strcmp(key, "reproductive") == 0) C_atoi(value, &creature_list[asset_counter].reproductive);
                        else if (strcmp(key, "mating_radius") == 0) C_atoi(value, &creature_list[asset_counter].mating_radius);
                        else if (strcmp(key, "birth_recovery_time") == 0) C_atoi(value, &creature_list[asset_counter].birth_recovery_time);
                        else if (strcmp(key, "adult_age") == 0) C_atoi(value, &creature_list[asset_counter].adult_age);
                        else if (strcmp(key, "gestation_period") == 0) C_atoi(value, &creature_list[asset_counter].gestation_period);
                        else if (strcmp(key, "litter_max") == 0) C_atoi(value, &creature_list[asset_counter].litter_max);
                        else if (strcmp(key, "max_age") == 0) C_atoi(value, &creature_list[asset_counter].max_age);
                        else if (strcmp(key, "width") == 0) C_atoi(value, &creature_list[asset_counter].phys.width);
                        else if (strcmp(key, "length") == 0) C_atoi(value, &creature_list[asset_counter].phys.length);
                        else if (strcmp(key, "height") == 0) C_atoi(value, &creature_list[asset_counter].phys.height);
                        else if (strcmp(key, "is_npc") == 0) C_atoi(value, &creature_list[asset_counter].npc_props.is_npc);
                        else if (strcmp(key, "is_vendor") == 0) C_atoi(value, &creature_list[asset_counter].npc_props.is_vendor);
                        else if (strcmp(key, "num_dialogue") == 0) C_atoi(value, &creature_list[asset_counter].npc_props.num_dialogue);
						else if (strcmp(key, "dialogue0") == 0) creature_list[asset_counter].npc_props.dialogue[0] = _strdup(value);
						else if (strcmp(key, "dialogue1") == 0) creature_list[asset_counter].npc_props.dialogue[1] = _strdup(value);
						else if (strcmp(key, "dialogue2") == 0) creature_list[asset_counter].npc_props.dialogue[2] = _strdup(value);
						else if (strcmp(key, "dialogue3") == 0) creature_list[asset_counter].npc_props.dialogue[3] = _strdup(value);
						else if (strcmp(key, "dialogue4") == 0) creature_list[asset_counter].npc_props.dialogue[4] = _strdup(value);
						else if (strcmp(key, "dialogue5") == 0) creature_list[asset_counter].npc_props.dialogue[5] = _strdup(value);
						else if (strcmp(key, "dialogue6") == 0) creature_list[asset_counter].npc_props.dialogue[6] = _strdup(value);
						else if (strcmp(key, "dialogue7") == 0) creature_list[asset_counter].npc_props.dialogue[7] = _strdup(value);
						else if (strcmp(key, "dialogue8") == 0) creature_list[asset_counter].npc_props.dialogue[8] = _strdup(value);
						else if (strcmp(key, "dialogue9") == 0) creature_list[asset_counter].npc_props.dialogue[9] = _strdup(value);

                        break;

                    case ASSET_TYPE_ITEM:
						if (strcmp(key, "name") == 0) item_list[asset_counter].name = _strdup(value);
						else if (strcmp(key, "description") == 0) item_list[asset_counter].description = _strdup(value);
                        else if (strcmp(key, "value") == 0) C_atoi(value, &item_list[asset_counter].value);
						else if (strcmp(key, "tileset_name") == 0) item_list[asset_counter].tileset_name = _strdup(value);
                        else if (strcmp(key, "tile_index") == 0) C_atoi(value, &item_list[asset_counter].tile_index);
                        else if (strcmp(key, "consummable") == 0) C_atoi(value, &item_list[asset_counter].consummable);
                        else if (strcmp(key, "type") == 0) C_atoi(value, &item_list[asset_counter].type);
                        else if (strcmp(key, "weapon_type") == 0) C_atoi(value, &item_list[asset_counter].weapon_props.weapon_type);
                        else if (strcmp(key, "min_damage") == 0) C_atoi(value, &item_list[asset_counter].weapon_props.min_damage);
                        else if (strcmp(key, "max_damage") == 0) C_atoi(value, &item_list[asset_counter].weapon_props.max_damage);
                        else if (strcmp(key, "attack_speed") == 0) item_list[asset_counter].weapon_props.attack_speed = atof(value);
                        else if (strcmp(key, "attack_range") == 0) C_atoi(value, &item_list[asset_counter].weapon_props.attack_range);
                        else if (strcmp(key, "defence") == 0) C_atoi(value, &item_list[asset_counter].armor_props.defence);
                        else if (strcmp(key, "armor_type") == 0) C_atoi(value, &item_list[asset_counter].armor_props.armor_type);
                        else if (strcmp(key, "armor_y_offset") == 0) C_atoi(value, &item_list[asset_counter].armor_props.armor_y_offset);
                        else if (strcmp(key, "x_offset") == 0) C_atoi(value, &item_list[asset_counter].phys.x_offset);
                        else if (strcmp(key, "y_offset") == 0) C_atoi(value, &item_list[asset_counter].phys.y_offset);
                        else if (strcmp(key, "coin_value") == 0) C_atoi(value, &item_list[asset_counter].coin_value);
                        else if (strcmp(key, "width") == 0) C_atoi(value, &item_list[asset_counter].phys.width);
                        else if (strcmp(key, "length") == 0) C_atoi(value, &item_list[asset_counter].phys.length);
                        else if (strcmp(key, "height") == 0) C_atoi(value, &item_list[asset_counter].phys.height);

                        break;
                    case ASSET_TYPE_ZONE:
						if (strcmp(key, "name") == 0) zone_list[asset_counter].name = _strdup(value);
                        else if (strcmp(key, "board_name") == 0) zone_list[asset_counter].board_name = _strdup(value);
						else if (strcmp(key, "x") == 0) zone_list[asset_counter].x = atof(value);
						else if (strcmp(key, "y") == 0) zone_list[asset_counter].y = atof(value);
						else if (strcmp(key, "w") == 0) zone_list[asset_counter].w = atof(value);
						else if (strcmp(key, "h") == 0) zone_list[asset_counter].h = atof(value);
                        else if (strcmp(key, "num_creatures") == 0) C_atoi(value, &zone_list[asset_counter].num_creatures);
                        else if (strcmp(key, "num_creature_types") == 0) C_atoi(value, &zone_list[asset_counter].num_creature_types);
						else if (strcmp(key, "creature_type_name0") == 0) zone_list[asset_counter].creature_type_names[0] = _strdup(value);
						else if (strcmp(key, "creature_type_name1") == 0) zone_list[asset_counter].creature_type_names[1] = _strdup(value);
						else if (strcmp(key, "creature_type_name2") == 0) zone_list[asset_counter].creature_type_names[2] = _strdup(value);
						else if (strcmp(key, "creature_type_name3") == 0) zone_list[asset_counter].creature_type_names[3] = _strdup(value);
						else if (strcmp(key, "creature_type_name4") == 0) zone_list[asset_counter].creature_type_names[4] = _strdup(value);
						else if (strcmp(key, "creature_type_name5") == 0) zone_list[asset_counter].creature_type_names[5] = _strdup(value);
						else if (strcmp(key, "creature_type_name6") == 0) zone_list[asset_counter].creature_type_names[6] = _strdup(value);
						else if (strcmp(key, "creature_type_name7") == 0) zone_list[asset_counter].creature_type_names[7] = _strdup(value);
						else if (strcmp(key, "creature_type_name8") == 0) zone_list[asset_counter].creature_type_names[8] = _strdup(value);
						else if (strcmp(key, "creature_type_name9") == 0) zone_list[asset_counter].creature_type_names[9] = _strdup(value);
						else if (strcmp(key, "creature_type_name10") == 0) zone_list[asset_counter].creature_type_names[10] = _strdup(value);
						else if (strcmp(key, "creature_type_name11") == 0) zone_list[asset_counter].creature_type_names[11] = _strdup(value);
						else if (strcmp(key, "creature_type_name12") == 0) zone_list[asset_counter].creature_type_names[12] = _strdup(value);
						else if (strcmp(key, "creature_type_name13") == 0) zone_list[asset_counter].creature_type_names[13] = _strdup(value);
						else if (strcmp(key, "creature_type_name14") == 0) zone_list[asset_counter].creature_type_names[14] = _strdup(value);
						else if (strcmp(key, "creature_type_name15") == 0) zone_list[asset_counter].creature_type_names[15] = _strdup(value); 

                        break;
                }

                memset(key, 0, 100);
                memset(value, 0, 100);
            }
            else
            {
                if(!is_string && (c == ' ' || c == '\t' || c == '\r'))
                {
                    // ignore white space unless it's inside a string
                }
                else
                {
                    if (is_key)
                        key[ikey++] = c;
                    else
                        value[ivalue++] = c;
                }
            }

        } while (c != EOF);

        fclose(fp);

        ++asset_counter;
    }


    switch(type)
    {
        case ASSET_TYPE_CREATURE: num_creature_list = asset_counter; break;
        case ASSET_TYPE_ITEM: num_item_list = asset_counter; break;
        case ASSET_TYPE_ZONE: num_zones = asset_counter; break;
    }
}
