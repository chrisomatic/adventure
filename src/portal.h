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

static void init_portals()
{
    Portal a,b;

	portal_links[num_portal_links].a.board_index = get_board_index_by_name("Astoria");
	portal_links[num_portal_links].a.x = 83 * TILE_WIDTH;
	portal_links[num_portal_links].a.y = 64 * TILE_HEIGHT;

	portal_links[num_portal_links].b.board_index = get_board_index_by_name("Sewers");
	portal_links[num_portal_links].b.x = 8 * TILE_WIDTH;
	portal_links[num_portal_links].b.y = 8 * TILE_HEIGHT;

	++num_portal_links;


	portal_links[num_portal_links].a.board_index = get_board_index_by_name("Sewers");
	portal_links[num_portal_links].a.x = 60 * TILE_WIDTH;
	portal_links[num_portal_links].a.y = 63 * TILE_HEIGHT;

	portal_links[num_portal_links].b.board_index = get_board_index_by_name("Sewers");
	portal_links[num_portal_links].b.x = 96 * TILE_WIDTH;
	portal_links[num_portal_links].b.y = 16 * TILE_HEIGHT;

	++num_portal_links;



	portal_links[num_portal_links].b.board_index = get_board_index_by_name("Sewers");
	portal_links[num_portal_links].b.x = 157 * TILE_WIDTH;
	portal_links[num_portal_links].b.y = 20 * TILE_HEIGHT;

	portal_links[num_portal_links].a.board_index = get_board_index_by_name("Sewers");
	portal_links[num_portal_links].a.x = 183 * TILE_WIDTH;
	portal_links[num_portal_links].a.y = 4 * TILE_HEIGHT;

	++num_portal_links;

}
