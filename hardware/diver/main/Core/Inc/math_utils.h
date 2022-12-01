#ifndef __MATH_H__
#define __MATH_H__

float exp_approx(float x)
{
    float y = 1.0f + x * 0.0009765625f;
    y = y * y;
    y = y * y;
    y = y * y;
    y = y * y;
    y = y * y;
    y = y * y;
    y = y * y;
    y = y * y;
    y = y * y;
    y = y * y;
    return y;
}

float smoothstep(int order, float x)
{
    if (x < 0.0f)
    {
        return 0.0f;
    }
    else if (x > 1.0f)
    {
        return 1.0f;
    }

    switch (order)
    {
    case 1:
        return -2.0f * x * x * x + 3.0f * x * x;
    case 2:
        return 6.0f * x * x * x * x * x - 15.0f * x * x * x * x + 10.0f * x * x * x;
    case 3:
        return -20.0f * x * x * x * x * x * x * x + 70.0f * x * x * x * x * x * x - 84.0f * x * x * x * x * x + 35.0f * x * x * x * x;
    case 4:
        return 70.0f * x * x * x * x * x * x * x * x * x - 315.0f * x * x * x * x * x * x * x * x + 540.0f * x * x * x * x * x * x * x - 420.0f * x * x * x * x * x * x + 126 * x * x * x * x * x;
    default:
        return x;
    }
}

float logistic_curve(float x, float growth, float midpoint)
{
    return 1.0f / (1.0f + exp_approx(-growth * (x - midpoint)));
}

float constrain(float x, float min, float max)
{
    return std::max(std::min(x, max), min);
}

#endif