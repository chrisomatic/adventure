#define MAX_COINS 10000

const char* coin_tileset_name = "items";

typedef enum
{
    COIN_BRONZE = 0,
    COIN_SILVER = 4,
    COIN_GOLD   = 8
} CoinType;

typedef struct
{
    float x;
    float y;
    float z;
    float x_vel;
    float y_vel;
    float z_vel;
    float friction;
    int board_index;
    CoinType type;
    Animation anim;
} Coin;

Coin coins[MAX_COINS];
int num_coins = 0;

static BOOL spawn_coin(float x, float y, float z, float x_vel, float y_vel, float z_vel,CoinType type, int board_index)
{
    coins[num_coins].x = x;
    coins[num_coins].y = y;
    coins[num_coins].z = z;
    coins[num_coins].x_vel = x_vel;
    coins[num_coins].y_vel = y_vel;
    coins[num_coins].z_vel = z_vel;
    coins[num_coins].board_index = board_index;
    coins[num_coins].friction = AIR_RESISTANCE;
    coins[num_coins].type = type;
    coins[num_coins].anim.counter = rand() % 5;
    coins[num_coins].anim.max_count = 5;
    coins[num_coins].anim.frame = 0;
    coins[num_coins].anim.num_frames = 4;
    coins[num_coins].anim.frame_order[0] = 0;
    coins[num_coins].anim.frame_order[1] = 1;
    coins[num_coins].anim.frame_order[2] = 2;
    coins[num_coins].anim.frame_order[3] = 3;

    num_coins++;
    if(num_coins > MAX_COINS -1)
    {
        num_coins = MAX_COINS - 1;
        return FALSE;
    }

    return TRUE;
}

static void remove_coin(int index)
{
	num_coins--;
	coins[index] = coins[num_coins];
}

static void update_coins()
{
    for(int i = num_coins -1; i >= 0; --i)
    {
        coins[i].x += coins[i].x_vel;
        coins[i].y += coins[i].y_vel;
        coins[i].z += coins[i].z_vel;

        if(coins[i].z < 0)
        {
            // hit the ground
            coins[i].z = 0.0f;
            coins[i].z_vel = 0.0f;
			coins[i].friction = GROUND_FRICTION;
        }
		else
		{
			coins[i].z_vel -= GRAVITY; // gravity acceleration
		}

        if(coins[i].x_vel != 0 || coins[i].y_vel != 0)
        {
            // friction
            if(coins[i].x_vel < 0) coins[i].x_vel += coins[i].friction;
            else coins[i].x_vel -= coins[i].friction;
            if(coins[i].y_vel < 0) coins[i].y_vel += coins[i].friction;
            else coins[i].y_vel -= coins[i].friction;

            if(abs(coins[i].x_vel) <= 0.1f) coins[i].x_vel = 0.0f;
            if(abs(coins[i].y_vel) <= 0.1f) coins[i].y_vel = 0.0f;
        }

        // update coin animation
        coins[i].anim.counter++;

        if(coins[i].anim.counter >= coins[i].anim.max_count)
        {
            // cycle_animation
            coins[i].anim.counter = 0;
            coins[i].anim.frame += 1;
            if(coins[i].anim.frame >= coins[i].anim.num_frames)
                coins[i].anim.frame = 0;
        }
    }

}

static void draw_coin(int i)
{
    if(coins[i].board_index != current_board_index)
        return;

    draw_tile_shadow(coins[i].x - camera.x, coins[i].y - camera.y, coin_tileset_name,coins[i].type + coins[i].anim.frame_order[coins[i].anim.frame],max(0,10 - day_cycle_shade_amount)); // shadow
    draw_tile(coins[i].x - camera.x, coins[i].y - camera.y - coins[i].z*0.5f,coin_tileset_name, coins[i].type + coins[i].anim.frame_order[coins[i].anim.frame],day_cycle_shade_amount);
}

static void draw_coins()
{
    for(int i = 0; i < num_coins; ++i)
    {
        draw_coin(i);
    }

}
