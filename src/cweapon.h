#define MAX_WEAPONS     1000
#define MAX_WEAPON_LIST 100
#define KNIFE 80
#define SWORD 81
#define AXE   82
#define BOW   83
#define STAFF 85

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
    int tile_index;
    WeaponType type;
} Weapon;

Weapon weapons[MAX_WEAPONS];
Weapon weapon_list[MAX_WEAPON_LIST];

int num_weapons = 0;

static void init_weapons()
{
    num_weapons = 0;
}

