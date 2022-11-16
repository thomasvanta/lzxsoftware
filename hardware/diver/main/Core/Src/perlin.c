#include "perlin.h"

float lin_inter(float x, float y, float s)
{
    return x + s * (y - x);
}

float smooth_inter(float x, float y, float s)
{
    return lin_inter(x, y, s * s * (3 - 2 * s));
}

//==================1D=====================
int noise1(int x)
{
    int tmp = hash[SEED % 256];
    return hash[(tmp + x) % 256];
}

float noise1d(float x)
{
    int x_int = x;
    float x_frac = x - x_int;
    int s = noise1(x_int);
    int t = noise1(x_int + 1);
    return smooth_inter(s, t, x_frac);
}

float perlin1d(float x, float freq, int depth)
{
    float xa = x * freq;
    float amp = 1.0;
    float fin = 0;
    float div = 0.0;

    int i;
    for (i = 0; i < depth; i++)
    {
        div += 256 * amp;
        fin += noise1d(xa) * amp;
        amp /= 2;
        xa *= 2;
    }

    return fin / div * 1023.0;
}

//==================2D=====================

int noise2(int x, int y)
{
    int tmp = hash[(y + SEED) % 256];
    return hash[(tmp + x) % 256];
}

float noise2d(float x, float y)
{
    int x_int = x;
    int y_int = y;
    float x_frac = x - x_int;
    float y_frac = y - y_int;
    int s = noise2(x_int, y_int);
    int t = noise2(x_int + 1, y_int);
    int u = noise2(x_int, y_int + 1);
    int v = noise2(x_int + 1, y_int + 1);
    float low = smooth_inter(s, t, x_frac);
    float high = smooth_inter(u, v, x_frac);
    return smooth_inter(low, high, y_frac);
}

float perlin2d(float x, float y, float freq, int depth)
{
    float xa = x * freq;
    float ya = y * freq;
    float amp = 1.0;
    float fin = 0;
    float div = 0.0;

    int i;
    for (i = 0; i < depth; i++)
    {
        div += 256 * amp;
        fin += noise2d(xa, ya) * amp;
        amp /= 2;
        xa *= 2;
        ya *= 2;
    }

    return fin / div * 1023.0;
}
