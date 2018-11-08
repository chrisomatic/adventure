#pragma once

#define MAX_ZONES 100

typedef struct
{
    char* name;
    char* board_name;
    int num_creatures;
    int num_creature_types;
    char* creature_type_names[16];
    float x;
    float y;
    float w;
    float h;
} Zone;
Zone zone_list[MAX_ZONES];
int num_zones = 0;

static BOOL get_zone_by_name(const char* name,Zone* zone)
{
    for(int i = 0; i < num_zones; ++i)
    {
		if (zone_list[i].name == NULL)
			continue;

        if(strcmp(zone_list[i].name, name) == 0)
        {
            zone->name = zone_list[i].name;
            zone->board_name = zone_list[i].board_name;
            zone->num_creatures = zone_list[i].num_creatures;
            zone->num_creature_types = zone_list[i].num_creature_types;
            zone->creature_type_names[0] = zone_list[i].creature_type_names[0];
			zone->creature_type_names[1] = zone_list[i].creature_type_names[1];
			zone->creature_type_names[2] = zone_list[i].creature_type_names[2];
			zone->creature_type_names[3] = zone_list[i].creature_type_names[3];
			zone->creature_type_names[4] = zone_list[i].creature_type_names[4];
			zone->creature_type_names[5] = zone_list[i].creature_type_names[5];
			zone->creature_type_names[6] = zone_list[i].creature_type_names[6];
			zone->creature_type_names[7] = zone_list[i].creature_type_names[7];
			zone->creature_type_names[8] = zone_list[i].creature_type_names[8];
			zone->creature_type_names[9] = zone_list[i].creature_type_names[9];
			zone->creature_type_names[10] = zone_list[i].creature_type_names[10];
			zone->creature_type_names[11] = zone_list[i].creature_type_names[11];
			zone->creature_type_names[12] = zone_list[i].creature_type_names[12];
			zone->creature_type_names[13] = zone_list[i].creature_type_names[13];
			zone->creature_type_names[14] = zone_list[i].creature_type_names[14];
			zone->creature_type_names[15] = zone_list[i].creature_type_names[15];
            zone->x = zone_list[i].x;
            zone->y = zone_list[i].y;
            zone->w = zone_list[i].w;
            zone->h = zone_list[i].h;

            return TRUE;
        }
    }

	return FALSE;
}

static int get_zone_index_by_name(const char* name)
{
    for(int i = 0; i < num_zones; ++i)
    {
		if (zone_list[i].name == NULL)
			continue;

        if(strcmp(zone_list[i].name, name) == 0)
        {
            return i;
        }
    }

    return -1;

}
