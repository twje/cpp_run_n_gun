#pragma once

// Includes
//------------------------------------------------------------------------------
// Game
#include "Settings.h"
#include "Entity.h"

// Third party
#include<SFML/Graphics.hpp>

// Core
#include "Core/GameObject.h"
#include "Core/Resources.h"
#include "Core/Animate.h"
#include "Core/Events.h"
#include "Core/GameObjectManager.h"

// System
#include <iostream>

//------------------------------------------------------------------------------
class Bullet : public GameObject
{
public:
    Bullet(const sf::Vector2f& position, const sf::Vector2f& direction, const sf::Color& tintColor)
        : mSprite(LoadTexture(Resources::BulletTexture))
        , mDirection(direction)
        , mDepth(LAYERS.at("Level"))
        , mSpeed(1200.0f)
        , mTimeToLiveInSeconds(0.0f)
    { 
        mSprite.setColor(tintColor);
        SetOrigin(sf::Vector2f(mSprite.getTextureRect().getSize()) / 2.0f);
        SetPosition(position);
    }

    virtual FloatRect GetGlobalBounds() const
    {
        return GetTransform().transformRect(mSprite.getLocalBounds());
    }

    virtual uint32_t GetDepth() const { return mDepth; }
    
    virtual void Update(const sf::Time& timeslice) 
    { 
        sf::Vector2f newPosition = GetPosition() + mDirection * mSpeed * timeslice.asSeconds();
        SetPosition(newPosition);

        mTimeToLiveInSeconds += timeslice.asSeconds();
        if (mTimeToLiveInSeconds > 1.0f)
        {
            Kill();
        }
    };
    
    virtual void draw(sf::RenderTarget& target, const sf::RenderStates& states) const 
    {
        sf::RenderStates statesCopy(states);
        statesCopy.transform *= GetTransform();
        target.draw(mSprite, statesCopy);
    }

    const sf::Sprite& GetSprite() { return mSprite; }

private:
    sf::Vector2f mDirection;
    float mSpeed;
    uint32_t mDepth;
    sf::Sprite mSprite;
    float mTimeToLiveInSeconds;
};

//------------------------------------------------------------------------------
class FireAnimation : public GameObject
{
public:
    FireAnimation(uint32_t entityId, const sf::Vector2f& direction, const sf::Color& tintColor)
        : mEntityId(entityId)
        , mDirection(direction)
        , mDepth(LAYERS.at("Level"))
        , mSprite(LoadTexture(Resources::PlaceholderTexture))
        , mAnimation(15)
    {
        mSprite.setColor(tintColor);
        CreateAnimation();
        SetAnimationFrame();        
        SetPositionWithOffset();        
    }

    virtual void HandleEvent(Event* event) override
    {
        if (event->IsType(EntityCoreEventType::ENTITY_REMOVE_FROM_SCENE) && event->IsFromSender(mEntityId))
        {
            Kill();
        }
    }

    virtual FloatRect GetGlobalBounds() const
    {
        return GetTransform().transformRect(mSprite.getLocalBounds());
    }

    virtual uint32_t GetDepth() const { return mDepth; }

    virtual void Update(const sf::Time& timeslice)
    {
        if (mAnimation.Update(timeslice))
        {
            Kill();
        }
        else
        {            
            SetAnimationFrame();
            SetPositionWithOffset();
        }
    };

    virtual void draw(sf::RenderTarget& target, const sf::RenderStates& states) const
    {
        sf::RenderStates statesCopy(states);
        statesCopy.transform *= GetTransform();
        target.draw(mSprite, statesCopy);
    }

private:
    void CreateAnimation()
    {
        const std::string sequenceId = "fireAnimation";

        auto textures = std::make_unique<std::vector<sf::Texture*>>();
        textures->push_back(&LoadTexture(Resources::fireAnimationTexture0));
        textures->push_back(&LoadTexture(Resources::fireAnimationTexture1));

        mAnimation.AddSequence({ sequenceId, std::move(textures) });
        mAnimation.SetSequence(sequenceId);
    }

    void SetAnimationFrame()
    {
        mSprite.setTexture(mAnimation.GetTexture(), true);
        if (mDirection.x < 0.0f)
        {
            SetScale({ -1.0f, 1.0f });            
        }
        SetOrigin(sf::Vector2f(mSprite.getTextureRect().getSize()) / 2.0f);
    }

    void SetPositionWithOffset()
    {
        Entity* entity = static_cast<Entity*>(GameObjectManager::Instance().GetInstance(mEntityId));
        if (entity)
        {
            sf::Vector2f positionOffset;
            positionOffset.x = mDirection.x > 0 ? 60.0f : -60.0f;        
            positionOffset.y = entity->IsDucking() ? 10.0f : -16.0f;

            SetPosition(entity->GetHitbox().GetCenter() + positionOffset);
        }
    }

    FloatRect mHitbox;
    Animation mAnimation;
    uint32_t mEntityId;
    sf::Vector2f mDirection;
    uint32_t mDepth;
    sf::Sprite mSprite;
};