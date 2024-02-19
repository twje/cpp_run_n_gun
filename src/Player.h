#pragma once

// Includes
//------------------------------------------------------------------------------
// Game
#include "Entity.h"
#include "TiledMap.h"
#include "Settings.h"

//------------------------------------------------------------------------------
class Player : public Entity
{
public:
    Player(const sf::Vector2f& position, TiledLayerSpatialQuery& collisionLayer, Group& collisionObjects, 
          IFireBulletCallback* fireBulletCallback)
        : Entity(position, 10, 200, "graphics/player", "right", fireBulletCallback)
        , mCollisionLayer(collisionLayer)
        , mCollisionObjects(collisionObjects)                   
        , mWalkSpeed(400.0f)
        , mGravity(40.0f)
        , mJumpSpeed(1200.0f)        
        , mIsDucking(false)
        , mIsOnFloor(false)
        , mIsJumping(false)
        , mCntJumpKeyHeld(false)
        , mPrvJumpKeyHeld(false)        
    {
        mHitbox = GetGlobalBounds();
        mPreviousHitbox = mHitbox;
    }

    virtual FloatRect GetHitbox() const override
    {
        return mHitbox;
    }

    virtual FloatRect GetPreviousHitbox() const override
    {
        return mPreviousHitbox;
    }

    void Input()
    {
        // Direction
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
        {
            mDirection.x = 1.0f;
            SetStatus("right");
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
        {
            mDirection.x = -1.0f;
            SetStatus("left");            
        }
        else
        {
            mDirection.x = 0.0f;
        }

        // Jump
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) && mIsOnFloor)
        {                        
            mDirection.y = -mJumpSpeed;
            mIsJumping = true;
        }                 

        // Duck
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) && mIsOnFloor)
        {
            mIsDucking = true;
        }
        else
        {
            mIsDucking = false;
        }

        // Fire bullet
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && CanFireBullet())
        {
            FireBullet(mHitbox, 60.0f, true);
            PlayShootSound();
        }
    }

    void UpdateStatus()
    {
        if (mDirection.x == 0 && mIsOnFloor)
        {
            SetStatus(SplitAndGetElement(GetStatus(), '_', 0) + "_idle");
        }

        if (!mIsOnFloor)
        {
            SetStatus(SplitAndGetElement(GetStatus(), '_', 0) + "_jump");
        }

        if (mIsOnFloor && mIsDucking)
        {
            SetStatus(SplitAndGetElement(GetStatus(), '_', 0) + "_duck");
        }
    }

    void Move(const sf::Time& timeslice)
    {
        if (mIsDucking && mIsOnFloor)
        {
            mDirection.x = 0;
        }

        sf::Vector2f delta = mDirection * mWalkSpeed * timeslice.asSeconds();

        // Hort movement
        mHitbox.MoveLeft(delta.x);
        CheckAndResolveHortCollision();

        // Ensure the player sticks to platform during abrupt downward velocity changes
        if (!mIsJumping && mMovingTileUnderPlayer)
        {
            if (mMovingTileUnderPlayer->GetVelocity().y > 0) // Down
            {
                mDirection.y = mMovingTileUnderPlayer->GetVelocity().y;
            }
        }

        mDirection.y += mGravity;
        mHitbox.MoveTop(mDirection.y * timeslice.asSeconds());

        CheckAndResolveVertCollision();
        SetPosition({ std::round(mHitbox.GetLeft()), std::round(mHitbox.GetTop()) });        
    }

    virtual void Update(const sf::Time& timeslice)
    {
        mPreviousHitbox = mHitbox;
        Input();
        UpdateStatus();
        Move(timeslice);
        Animate(timeslice);
        UpdateTimerCooldowns(timeslice);
        Blink();
        CheckDeath();
    };

    virtual bool IsDucking() const override { return mIsDucking; }

    virtual void CheckDeath() override { }

    void ResetPlayerPosition(sf::Vector2f position)
    {
        SetPosition(position);
        mDirection = sf::Vector2f();
        mHitbox = GetGlobalBounds();
        mPreviousHitbox = mHitbox;
    }

private:
    void CheckAndResolveHortCollision()
    {
        // Level collisions
        for (const GameObject& obj : mCollisionLayer.QueryRegion(mHitbox))
        {
            if (mDirection.x > 0)
            {
                mHitbox.SetRight(obj.GetHitbox().GetLeft());
            }
            else if (mDirection.x < 0)
            {
                mHitbox.SetLeft(obj.GetHitbox().GetRight());
            }
        }

        // Obstacle collisions
        for (GameObject* obj : mCollisionObjects)
        {            
            const FloatRect& objCntHitbox = obj->GetHitbox();
            if (objCntHitbox.FindIntersection(mHitbox))
            {
                const FloatRect& objPrvHitbox = obj->GetPreviousHitbox();

                // Left
                if (mHitbox.GetLeft() <= objCntHitbox.GetRight() && mPreviousHitbox.GetLeft() >= objPrvHitbox.GetRight())
                {
                    mHitbox.SetLeft(objCntHitbox.GetRight());
                }

                // Right
                if (mHitbox.GetRight() >= objCntHitbox.GetLeft() && mPreviousHitbox.GetRight() <= objPrvHitbox.GetLeft())
                {
                    mHitbox.SetRight(objCntHitbox.GetLeft());
                }
            }
        }
    }

    void CheckAndResolveVertCollision()
    {
        mIsOnFloor = false;        
        std::vector<const GameObject*> objectsBelow;
        std::vector<const GameObject*> objectsAbove;

        // Process level collisions
        for (const GameObject& object : mCollisionLayer.QueryRegion(mHitbox))
        {
            if (mDirection.y > 0.0f)
            {
                objectsBelow.emplace_back(&object);
            }
            else if (mDirection.y < 0.0f)
            {
                objectsAbove.emplace_back(&object);
            }
        }

        // Process dynamic obstacle collisions
        for (const GameObject* object : mCollisionObjects)
        {            
            FloatRect objectHitbox = object->GetHitbox();

            if (objectHitbox.FindIntersection(mHitbox))
            {
                if (IsDownCollision(*object))
                {
                    objectsBelow.emplace_back(object);
                }
                else if (IsUpCollision(*object))
                {
                    objectsAbove.emplace_back(object);
                }
            }
        }

        ResolveVertCollision(objectsBelow, objectsAbove);
    }

    void ResolveVertCollision(std::vector<const GameObject*>& objectsBelow, std::vector<const GameObject*>& objectsAbove)
    {
        // Resolve ground objects
        const GameObject* lowestYVelocityoObject = nullptr;
        for (const GameObject* object : objectsBelow)
        {
            if (!lowestYVelocityoObject || object->GetVelocity().y < lowestYVelocityoObject->GetVelocity().y)
            {
                lowestYVelocityoObject = object;
            }
        }
        
        mMovingTileUnderPlayer = nullptr;
        if (lowestYVelocityoObject)
        {
            mHitbox.SetBottom(lowestYVelocityoObject->GetHitbox().GetTop());
            mDirection.y = lowestYVelocityoObject->GetVelocity().y;
            mIsOnFloor = true;     
            mIsJumping = false;
            if (mDirection.y != 0.0f)
            {
                mMovingTileUnderPlayer = lowestYVelocityoObject;
            }
        }

        // Resolve ceiling objects
        const GameObject* highestYVelocityObject = nullptr;
        for (const GameObject* object : objectsAbove)
        {
            if (!highestYVelocityObject || object->GetVelocity().y > highestYVelocityObject->GetVelocity().y)
            {
                highestYVelocityObject = object;
            }
        }

        if (highestYVelocityObject)
        {                 
            mHitbox.SetTop(highestYVelocityObject->GetHitbox().GetBottom());
            mDirection.y = highestYVelocityObject->GetVelocity().y;
        }
    }
    
    const GameObject* mMovingTileUnderPlayer = nullptr;
    TiledLayerSpatialQuery& mCollisionLayer;
    Group& mCollisionObjects;    
    sf::Vector2f mDirection;
    FloatRect mHitbox;
    FloatRect mPreviousHitbox;
    bool mIsDucking;    
    float mWalkSpeed;
    float mGravity;
    float mJumpSpeed;
    bool mIsOnFloor;
    bool mIsJumping;
    bool mCntJumpKeyHeld;
    bool mPrvJumpKeyHeld;  
};