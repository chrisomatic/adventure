#pragma once

static LARGE_INTEGER freq;
static LARGE_INTEGER t0,t1;

double target_spf;
double seconds_per_tick;
double accum_time;

/*
Usage:

timer_init(60);

while(running)
{
    timer_tick();
    if (timer_ready())
    {
        // process frame
    }
}

*/

static void timer_init(double fps)
{
    QueryPerformanceFrequency(&freq);
	seconds_per_tick = 1.0f / (double)freq.QuadPart;

    target_spf = 1.0f / fps;
    accum_time = 0.0;

    QueryPerformanceCounter(&t0);
}

static void timer_tick()
{
    QueryPerformanceCounter(&t1);
    __int64 interval = t1.QuadPart - t0.QuadPart;
    double seconds_gone_by = seconds_per_tick * (double)interval;

    accum_time += seconds_gone_by;
    t0 = t1;
}

static BOOL timer_ready(int* frames_per_second)
{
    if(accum_time >= target_spf)
    {
        // just for reporting fps back
        *frames_per_second = ceil(1.0f / accum_time);

        while(accum_time >= target_spf)
            accum_time -= target_spf;

        return TRUE;
    }

    return FALSE;
}
