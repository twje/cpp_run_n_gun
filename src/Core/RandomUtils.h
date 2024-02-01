#pragma once

// Includes
//------------------------------------------------------------------------------
// System
#include <random>

// Global static variables
//------------------------------------------------------------------------------
static std::random_device rd;
static std::mt19937 gen(rd());

//------------------------------------------------------------------------------
template <typename T>
T& GetRandomElement(std::vector<T>& vec)
{
    if (vec.empty())
    {
        throw std::runtime_error("Vector cannot be empty");
    }

    std::uniform_int_distribution<size_t> distribution(0, vec.size() - 1);

    return vec[distribution(gen)];
}

//------------------------------------------------------------------------------
template <typename T>
T GetRandomIntegerFromList(std::initializer_list<T> list) 
{
    if (list.size() == 0) 
    {
        throw std::invalid_argument("List cannot be empty.");
    }
    
    std::uniform_int_distribution<std::ptrdiff_t> distrib(0, list.size() - 1);

    auto it = list.begin();
    std::advance(it, distrib(gen));
    return *it;
}

//------------------------------------------------------------------------------
int32_t RandomInteger(int32_t min, int32_t max);
float RandomFloat(float min, float max);