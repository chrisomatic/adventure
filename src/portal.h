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
	portal_links[num_portal_links].a.x = 203*TILE_WIDTH;
	portal_links[num_portal_links].a.y = 9*TILE_HEIGHT;

	portal_links[num_portal_links].b.board_index = get_board_index_by_name("Caverns");
	portal_links[num_portal_links].b.x = 40*TILE_WIDTH;
	portal_links[num_portal_links].b.y = 76*TILE_HEIGHT;

    ++num_portal_links;

}
