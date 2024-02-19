#pragma once

// Includes
//------------------------------------------------------------------------------
// Game
#include "Entity.h"
#include "Player.h"

//------------------------------------------------------------------------------
class Enemy : public Entity
{
public:
    Enemy(const sf::Vector2f& position, Player& player, TiledLayerSpatialQuery& collisionLayer, IFireBulletCallback* fireBulletCallback)
        : Entity(position, 3, 1000, "graphics/enemy", "right", fireBulletCallback)
        , mPlayer(player)
    {
        mHitbox = GetGlobalBounds();
        for (const GameObject& obj : collisionLayer.QueryRegion(mHitbox))
        {
            mHitbox.SetBottom(obj.GetHitbox().GetTop());
        }
        mPreviousHitbox = mHitbox;
        SetPosition({ std::round(mHitbox.GetLeft()), std::round(mHitbox.GetTop()) });
    }

    virtual FloatRect GetHitbox() const override
    {
        return mHitbox;
    }

    virtual FloatRect GetPreviousHitbox() const override
    {
        return mPreviousHitbox;
    }

    void UpdateStatus()
    {
        if (mPlayer.GetHitbox().GetCenterX() < mHitbox.GetCenterX())
        {
            SetStatus("left");
        }
        else
        {
            SetStatus("right");
        }  
    }

    void CheckFire()
    {
        float distance = (mHitbox.GetCenter() - mPlayer.GetHitbox().GetCenter()).length();

        float playerCenterY = mPlayer.GetHitbox().GetCenterY();
        bool sameY = (mHitbox.GetTop() - 20 < playerCenterY && playerCenterY < mHitbox.GetBottom() + 20);

        if (distance < 600.0f && CanFireBullet() && sameY)
        {
            FireBullet(mHitbox, 40.0f, false);
            PlayShootSound();
        }
    }

    virtual void Update(const sf::Time& timeslice)
    {
        mPreviousHitbox = mHitbox;
        UpdateStatus();
        Animate(timeslice);
        UpdateTimerCooldowns(timeslice);
        CheckFire();
        Blink();
        CheckDeath();
    };

private:
    FloatRect mHitbox;
    FloatRect mPreviousHitbox;
    Player& mPlayer;
};