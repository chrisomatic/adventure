#define KNIFE 64
#define SWORD 65
#define AXE   66

typedef struct
{
    char* name;
    float attack_speed;
    int attack_range;
    int min_damage;
    int max_damage;
    int   tile_index;
} Weapon;

Weapon weapons[20];

int num_weapons = 0;

static void init_weapons()
{
    // knife
    weapons[0].name = "Knife";
    weapons[0].attack_speed = 2.0f;
    weapons[0].attack_range = 16;
    weapons[0].min_damage = 1;
    weapons[0].max_damage = 2;
    weapons[0].tile_index = KNIFE; 
    num_weapons++;
    
    // sword
    weapons[1].name = "Sword";
    weapons[1].attack_speed = 1.0f;
    weapons[1].attack_range = 20;
    weapons[1].min_damage = 2;
    weapons[1].max_damage = 3;
    weapons[1].tile_index = SWORD; 
    num_weapons++;
    
    // axe
    weapons[2].name = "Axe";
    weapons[2].attack_speed = 0.5f;
    weapons[2].attack_range = 20;
    weapons[2].min_damage = 3;
    weapons[2].max_damage = 4;
    weapons[2].tile_index = AXE;
    num_weapons++;
    
    // ultimate
    weapons[3].name = "Ultimate";
    weapons[3].attack_speed = 3.0f;
    weapons[3].attack_range = 100;
    weapons[3].min_damage = 100;
    weapons[3].max_damage = 100;
    weapons[3].tile_index = AXE;
    num_weapons++;

}

