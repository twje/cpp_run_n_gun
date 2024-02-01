#include "RandomUtils.h"

int32_t RandomInteger(int32_t min, int32_t max)
{    
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}

float RandomFloat(float min, float max)
{    
    std::uniform_real_distribution<float> distrib(min, max);
    return distrib(gen);
}