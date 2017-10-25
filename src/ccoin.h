#define MAX_COINS 10000
#define COIN_TILE_INDEX 96

typedef struct
{
    float x;
    float y;
    Animation anim;
} Coin;

Coin coins[MAX_COINS];
int num_coins = 0;

static void spawn_coin(int x, int y)
{
    coins[num_coins].x = x;
    coins[num_coins].y = y;
    coins[num_coins].anim.counter = rand() % 5;
    coins[num_coins].anim.max_count = 5;
    coins[num_coins].anim.frame = 0;
    coins[num_coins].anim.num_frames = 4;
    coins[num_coins].anim.frame_order[0] = 0;
    coins[num_coins].anim.frame_order[1] = 1;
    coins[num_coins].anim.frame_order[2] = 2;
    coins[num_coins].anim.frame_order[3] = 3;

    num_coins++;
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

static void draw_coins()
{
    for(int i = 0; i < num_coins; ++i)
    {
		draw_tile(coins[i].x - camera.x, coins[i].y - camera.y, COIN_TILE_INDEX + coins[i].anim.frame_order[coins[i].anim.frame],day_cycle_shade_amount);
    }

}
