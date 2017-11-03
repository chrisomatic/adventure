typedef enum
{
    ASSET_TYPE_ENEMY,
    ASSET_TYPE_ITEM,
    ASSET_TYPE_NPC,
    ASSET_TYPE_WEAPON
} ASSET_TYPE;

static int get_files_in_directory_with_extension(const char* directory_path, const char* extension,char file_paths[100][MAX_PATH]);
static BOOL copy_weapon_by_name(const char* name,Weapon* copy_to_weapon);
static void load_assets(ASSET_TYPE type);
static void load_all_assets();

static BOOL get_weapon_by_name(const char* name,Weapon* weapon)
{
    for(int i = 0; i < MAX_WEAPON_LIST; ++i)
    {
		if (weapon_list[i].name == NULL)
			continue;

        if(strcmp(weapon_list[i].name, name) == 0)
        {
            weapon->name = weapon_list[i].name;
			weapon->type = weapon_list[i].type;
            weapon->attack_speed = weapon_list[i].attack_speed;
            weapon->attack_range = weapon_list[i].attack_range;
            weapon->min_damage = weapon_list[i].min_damage;
            weapon->max_damage = weapon_list[i].max_damage;
            weapon->tile_index = weapon_list[i].tile_index;

            return TRUE;
        }
    }
}

static void load_all_assets()
{
    load_assets(ASSET_TYPE_ENEMY);
    load_assets(ASSET_TYPE_ITEM);
    load_assets(ASSET_TYPE_NPC);
    load_assets(ASSET_TYPE_WEAPON);
}

static void load_assets(ASSET_TYPE type)
{
    char* directory;
    char* file_extension;

    switch(type)
    {
        case ASSET_TYPE_ENEMY:
            directory = "data\\enemies";
            file_extension = ".enemy";
            break;
        case ASSET_TYPE_ITEM:
            directory = "data\\items";
            file_extension = ".item";
            break;
        case ASSET_TYPE_NPC:
            directory = "data\\npcs";
            file_extension = ".npc";
            break;
        case ASSET_TYPE_WEAPON:
            directory = "data\\weapons";
            file_extension = ".weapon";
            break;
        default:
            return;
    }

	char paths[100][MAX_PATH] = { 0 };
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
                    case ASSET_TYPE_ENEMY:
						if (strcmp(key, "name") == 0) enemy_list[asset_counter].name = _strdup(value);
                        else if (strcmp(key, "hp") == 0) C_atoi(value, &enemy_list[asset_counter].hp);
                        else if (strcmp(key, "max_hp") == 0) C_atoi(value, &enemy_list[asset_counter].max_hp);
                        else if (strcmp(key, "xp") == 0) C_atoi(value, &enemy_list[asset_counter].xp);
                        else if (strcmp(key, "speed") == 0) enemy_list[asset_counter].speed = atof(value);
                        else if (strcmp(key, "gold_drop_max") == 0) C_atoi(value, &enemy_list[asset_counter].gold_drop_max);
                        else if (strcmp(key, "behavior") == 0) C_atoi(value, &enemy_list[asset_counter].behavior);
                        else if (strcmp(key, "untargetable") == 0) C_atoi(value, &enemy_list[asset_counter].untargetable);
                        else if (strcmp(key, "tile_index") == 0) C_atoi(value, &enemy_list[asset_counter].tile_index);
                        break;

                    case ASSET_TYPE_ITEM:
						if (strcmp(key, "name") == 0) item_list[asset_counter].name = _strdup(value);
						else if (strcmp(key, "description") == 0) item_list[asset_counter].description = _strdup(value);
                        else if (strcmp(key, "value") == 0) C_atoi(value, &item_list[asset_counter].value);
                        else if (strcmp(key, "tile_index") == 0) C_atoi(value, &item_list[asset_counter].tile_index);
                        else if (strcmp(key, "consummable") == 0) C_atoi(value, &item_list[asset_counter].consummable);
                        else if (strcmp(key, "type") == 0) C_atoi(value, &item_list[asset_counter].type);
                        break;

                    case ASSET_TYPE_NPC:
						if (strcmp(key, "name") == 0) npc_list[asset_counter].name = _strdup(value);
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
                        else if (strcmp(key, "tile_index") == 0) C_atoi(value, &npc_list[asset_counter].tile_index);
                        break;
                    case ASSET_TYPE_WEAPON:

						if (strcmp(key, "name") == 0) weapon_list[asset_counter].name = _strdup(value);
                        else if (strcmp(key, "type") == 0) C_atoi(value, &weapon_list[asset_counter].type);
                        else if (strcmp(key, "min_damage") == 0) C_atoi(value, &weapon_list[asset_counter].min_damage);
                        else if (strcmp(key, "max_damage") == 0) C_atoi(value, &weapon_list[asset_counter].max_damage);
                        else if (strcmp(key, "attack_speed") == 0) weapon_list[asset_counter].attack_speed = atof(value);
                        else if (strcmp(key, "attack_range") == 0) C_atoi(value, &weapon_list[asset_counter].attack_range);
                        else if (strcmp(key, "tile_index") == 0) C_atoi(value, &weapon_list[asset_counter].tile_index);
                        break;
                }

                memset(key, 0, 100);
                memset(value, 0, 100);
            }
            else
            {
                if(!is_string && (c == ' ' || c == '\t'))
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
}

static int get_files_in_directory_with_extension(const char* directory_path, const char* extension,char file_paths[100][MAX_PATH])
{
    WIN32_FIND_DATA ffd;
    LARGE_INTEGER filesize;
    TCHAR szDir[MAX_PATH];
    HANDLE hFind = INVALID_HANDLE_VALUE;
    size_t length_of_arg;
    int num_files = 0;

    StringCchLength(directory_path, MAX_PATH, &length_of_arg);

    if (length_of_arg > (MAX_PATH - 3))
    {
        printf(TEXT("\nDirectory path is too long.\n"));
        return (-1);
    }

    StringCchCopy(szDir, MAX_PATH, directory_path);
    StringCchCat(szDir, MAX_PATH, TEXT("\\*")); 
    StringCchCat(szDir, MAX_PATH, TEXT(extension));

    hFind = FindFirstFile(szDir, &ffd);

    if (INVALID_HANDLE_VALUE == hFind) 
    {
        printf(TEXT("\nError getting first file.\n"));
        return -1;
    } 

    char full_file_path[MAX_PATH] = {0};
    StringCchCopy(full_file_path,MAX_PATH,directory_path);
	StringCchCat(full_file_path, MAX_PATH, "\\");
	StringCchCat(full_file_path, MAX_PATH, ffd.cFileName);
    StringCchCopy(file_paths[num_files],MAX_PATH,full_file_path);

	do
    {
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            // directory
        }
        else
        {
            StringCchCopy(full_file_path,MAX_PATH,directory_path);
            StringCchCat(full_file_path, MAX_PATH, "\\");
            StringCchCat(full_file_path, MAX_PATH, ffd.cFileName);
            StringCchCopy(file_paths[num_files++],MAX_PATH,full_file_path);
        }
	} while (FindNextFile(hFind, &ffd) != 0);
    

    FindClose(hFind); 

    return num_files;
}
