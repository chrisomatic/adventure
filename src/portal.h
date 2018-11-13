#pragma once

#define MAX_PORTAL_LINKS 100

typedef struct
{
    int board_index;
    float x;
    float y;
} Portal;

typedef struct
{
    Portal a;
    Portal b;
} PortalLink;

PortalLink portal_links[MAX_PORTAL_LINKS];
int num_portal_links = 0;
int num_portals = 0;

static void init_portals()
{
    Portal a,b;

	portal_links[num_portal_links].a.board_index = get_board_index_by_name("Astoria");
	portal_links[num_portal_links].a.x = 83 * TILE_WIDTH;
	portal_links[num_portal_links].a.y = 64 * TILE_HEIGHT;

	portal_links[num_portal_links].b.board_index = get_board_index_by_name("Sewers");
	portal_links[num_portal_links].b.x = 11 * TILE_WIDTH;
	portal_links[num_portal_links].b.y = 14 * TILE_HEIGHT;

	++num_portal_links;

	portal_links[num_portal_links].a.board_index = get_board_index_by_name("Sewers");
	portal_links[num_portal_links].a.x = 66 * TILE_WIDTH;
	portal_links[num_portal_links].a.y = 59 * TILE_HEIGHT;

	portal_links[num_portal_links].b.board_index = get_board_index_by_name("Sewers");
	portal_links[num_portal_links].b.x = 95 * TILE_WIDTH;
	portal_links[num_portal_links].b.y = 14 * TILE_HEIGHT;

	++num_portal_links;

	portal_links[num_portal_links].a.board_index = get_board_index_by_name("Sewers");
	portal_links[num_portal_links].a.x = 158 * TILE_WIDTH;
	portal_links[num_portal_links].a.y = 23 * TILE_HEIGHT;

	portal_links[num_portal_links].b.board_index = get_board_index_by_name("Sewers");
	portal_links[num_portal_links].b.x = 176 * TILE_WIDTH;
	portal_links[num_portal_links].b.y = 10 * TILE_HEIGHT;

	++num_portal_links;

}
