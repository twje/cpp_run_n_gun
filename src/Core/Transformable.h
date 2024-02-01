#pragma once

// Includes
//------------------------------------------------------------------------------
// Third party
#include <SFML/Graphics.hpp>

//------------------------------------------------------------------------------
class Tranformable
{
public:
    virtual void SetPosition(const sf::Vector2f& position)
    {
        mTransformable.setPosition(position);
    }
    
    void SetOrigin(const sf::Vector2f& origin)
    {
        mTransformable.setOrigin(origin);
    }

    void SetScale(const sf::Vector2f& factors)
    {
        mTransformable.setScale(factors);
    }

    const sf::Vector2f& GetPosition() const
    {
        return mTransformable.getPosition();
    }

    const sf::Transform& GetTransform() const
    {
        return mTransformable.getTransform();
    }

    sf::Transformable& GetInternaleTransformable()
    {
        return mTransformable;
    }

private:
    sf::Transformable mTransformable;
};