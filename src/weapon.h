#define MAX_WEAPONS     1000
#define MAX_WEAPON_LIST 100
#define KNIFE 0
#define SWORD 1
#define AXE   2
#define BOW   3
#define STAFF 5
#define CROSSBOW 7 

typedef enum
{
    WEAPON_TYPE_MELEE,
    WEAPON_TYPE_RANGED
} WeaponType;

typedef struct
{
    char* name;
    float attack_speed;
    int attack_range;
    int min_damage;
    int max_damage;
    char* tileset_name;
    int tile_index;
    WeaponType type;
} Weapon;

Weapon weapons[MAX_WEAPONS];
Weapon weapon_list[MAX_WEAPON_LIST];

int num_weapons = 0;
int num_weapon_list = 0;

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
            weapon->tileset_name = weapon_list[i].tileset_name;
            weapon->tile_index = weapon_list[i].tile_index;

            return TRUE;
        }
    }
}
static void init_weapons()
{
    num_weapons = 0;
}

