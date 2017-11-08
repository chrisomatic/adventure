typedef enum
{
	ASSET_TYPE_CREATURE,
	ASSET_TYPE_ITEM,
	ASSET_TYPE_NPC
} ASSET_TYPE;

static void load_assets(ASSET_TYPE type);
static void load_all_assets();

static void load_all_assets()
{
    load_assets(ASSET_TYPE_CREATURE);
    load_assets(ASSET_TYPE_ITEM);
    load_assets(ASSET_TYPE_NPC);
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
        case ASSET_TYPE_NPC:
            directory = "data\\npcs";
            file_extension = ".npc";
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
						else if (strcmp(key, "species") == 0) creature_list[asset_counter].species = _strdup(value);
                        else if (strcmp(key, "hp") == 0) C_atoi(value, &creature_list[asset_counter].hp);
                        else if (strcmp(key, "max_hp") == 0) C_atoi(value, &creature_list[asset_counter].max_hp);
                        else if (strcmp(key, "xp") == 0) C_atoi(value, &creature_list[asset_counter].xp);
                        else if (strcmp(key, "speed") == 0) creature_list[asset_counter].speed = atof(value);
                        else if (strcmp(key, "gold_drop_max") == 0) C_atoi(value, &creature_list[asset_counter].gold_drop_max);
                        else if (strcmp(key, "behavior") == 0) C_atoi(value, &creature_list[asset_counter].behavior);
                        else if (strcmp(key, "untargetable") == 0) C_atoi(value, &creature_list[asset_counter].untargetable);
						else if (strcmp(key, "tileset_name") == 0) creature_list[asset_counter].tileset_name = _strdup(value);
                        else if (strcmp(key, "tile_index") == 0) C_atoi(value, &creature_list[asset_counter].tile_index);
                        else if (strcmp(key, "reproductive") == 0) C_atoi(value, &creature_list[asset_counter].reproductive);

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

                        break;

                    case ASSET_TYPE_NPC:
						if (strcmp(key, "name") == 0) npc_list[asset_counter].name = _strdup(value);
                        else if (strcmp(key, "x") == 0) npc_list[asset_counter].x = atof(value);
                        else if (strcmp(key, "y") == 0) npc_list[asset_counter].y = atof(value);
                        else if (strcmp(key, "hp") == 0) C_atoi(value, &npc_list[asset_counter].hp);
                        else if (strcmp(key, "max_hp") == 0) C_atoi(value, &npc_list[asset_counter].max_hp);
                        else if (strcmp(key, "xp") == 0) C_atoi(value, &npc_list[asset_counter].xp);
                        else if (strcmp(key, "num_dialogue") == 0) C_atoi(value, &npc_list[asset_counter].num_dialogue);
						else if (strcmp(key, "dialogue0") == 0) npc_list[asset_counter].dialogue[0] = _strdup(value);
						else if (strcmp(key, "dialogue1") == 0) npc_list[asset_counter].dialogue[1] = _strdup(value);
						else if (strcmp(key, "dialogue2") == 0) npc_list[asset_counter].dialogue[2] = _strdup(value);
						else if (strcmp(key, "dialogue3") == 0) npc_list[asset_counter].dialogue[3] = _strdup(value);
						else if (strcmp(key, "dialogue4") == 0) npc_list[asset_counter].dialogue[4] = _strdup(value);
						else if (strcmp(key, "dialogue5") == 0) npc_list[asset_counter].dialogue[5] = _strdup(value);
						else if (strcmp(key, "dialogue6") == 0) npc_list[asset_counter].dialogue[6] = _strdup(value);
						else if (strcmp(key, "dialogue7") == 0) npc_list[asset_counter].dialogue[7] = _strdup(value);
						else if (strcmp(key, "dialogue8") == 0) npc_list[asset_counter].dialogue[8] = _strdup(value);
						else if (strcmp(key, "dialogue9") == 0) npc_list[asset_counter].dialogue[9] = _strdup(value);
						else if (strcmp(key, "tileset_name") == 0) npc_list[asset_counter].tileset_name = _strdup(value);
                        else if (strcmp(key, "tile_index") == 0) C_atoi(value, &npc_list[asset_counter].tile_index);

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
        case ASSET_TYPE_NPC: num_npc_list = asset_counter; break;
    }
}
