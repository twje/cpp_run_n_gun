#pragma once

// Includes
//------------------------------------------------------------------------------
// Third party
#include <SFML/Graphics.hpp>

// Forward declarations
//------------------------------------------------------------------------------
class Entity;

//------------------------------------------------------------------------------
class IFireBulletCallback
{
public:
    virtual void FireBullet(const sf::Vector2f& position, const sf::Vector2f& direction, Entity& entity, bool isPlayerBullet) = 0;
};