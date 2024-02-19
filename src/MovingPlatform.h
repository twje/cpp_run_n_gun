#pragma once

// Includes
//------------------------------------------------------------------------------
// Game
#include "Settings.h"

// Core
#include <Core/GameObject.h>
#include <Core/RectUtils.h>
#include <Core/FloatRect.h>

// Third party
#include <SFML/Graphics.hpp>

//------------------------------------------------------------------------------
class MovingPlatform : public GameObject
{
public:
    MovingPlatform(const sf::Vector2f& position, const sf::Texture& texture, const sf::IntRect& textureRect, std::vector<sf::FloatRect>& platformWayPoints)
        : mSprite(texture, textureRect)
        , mPlatformWayPoints(platformWayPoints)
        , mDirection(0.0f, -1.0f)
        , mDepth(LAYERS.at("Level"))
        , mSpeed(200.0f)
    {
        SetPosition(position);
        mHitbox = GetGlobalBounds();
    }

    virtual FloatRect GetGlobalBounds() const override
    {
        return GetTransform().transformRect(mSprite.getLocalBounds());
    }

    virtual FloatRect GetHitbox() const override
    {
        return mHitbox;
    }

    virtual FloatRect GetPreviousHitbox() const 
    { 
        return mPreviousHitbox;
    }

    virtual const sf::Vector2f GetVelocity() const
    {
        return mDirection * mSpeed;
    }

    virtual uint32_t GetDepth() const override
    {
        return mDepth;
    }

    virtual void Update(const sf::Time& timeslice)
    {
        mPreviousHitbox = mHitbox;
        mHitbox.MoveTop(mDirection.y * mSpeed * timeslice.asSeconds());        
     
        for (FloatRect waypoint : mPlatformWayPoints)
        {
            if (GetGlobalBounds().FindIntersection(waypoint))
            {
                if (IsMovingDown())
                {
                    SetBottom(waypoint.GetTop());
                    ReverseDirection();
                }
                else
                {                    
                    SetTop(waypoint.GetBottom());
                    ReverseDirection();
                }
            }
        }        
        
        SetPosition({ std::round(mHitbox.GetLeft()), std::round(mHitbox.GetTop()) });
    }

    virtual void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override
    {
        sf::RenderStates statesCopy(states);
        statesCopy.transform *= GetTransform();
        target.draw(mSprite, statesCopy);
    }

private:    
    bool IsMovingDown() const { return mDirection.y > 0.0f; }
    void ReverseDirection() { mDirection.y = -mDirection.y; }

    void SetTop(float value)
    {
        mHitbox.SetTop(value);
        SetPosition({ std::round(mHitbox.GetLeft()), std::round(mHitbox.GetTop()) });
    }

    void SetBottom(float value)
    {
        mHitbox.SetBottom(value);
        SetPosition({ std::round(mHitbox.GetLeft()), std::round(mHitbox.GetTop()) });
    }

    std::vector<sf::FloatRect>& mPlatformWayPoints;
    FloatRect mHitbox;
    FloatRect mPreviousHitbox;
    sf::Vector2f mDirection;
    sf::Sprite mSprite;
    uint32_t mDepth;
    float mSpeed;
};