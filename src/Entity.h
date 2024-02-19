#pragma once

// Includes
//------------------------------------------------------------------------------
// Game
#include "Interface.h"
#include "Settings.h"

// Core
#include "Core/GameObject.h"
#include "Core/Animate.h"
#include "Core/StringUtils.h"
#include "Core/Timer.h"
#include "Core/Resources.h"

// Third party
#include <SFML/Graphics.hpp>

// System
#include <filesystem>

namespace fs = std::filesystem;

//------------------------------------------------------------------------------
std::vector<std::string> GetDirectoryNames(const std::string& path)
{
    std::vector<std::string> directories;
    for (const auto& entry : fs::directory_iterator(path))
    {
        if (fs::is_directory(entry.status()))
        {
            directories.push_back(entry.path().filename().string());
        }
    }
    return directories;
}

//------------------------------------------------------------------------------
class Entity : public GameObject
{
public:
    Entity(const sf::Vector2f& position, int32_t health, int32_t firsCooldownTimeMsc, const std::string& animTextureDirectory, 
           const std::string& status, IFireBulletCallback* fireBulletCallback)
        : mSprite(LoadTexture(Resources::PlaceholderTexture))
        , mStatus(status)
        , mHealth(health)
        , mBulletFireCooldown(sf::milliseconds(firsCooldownTimeMsc))
        , mFireBulletCallback(fireBulletCallback)
        , mDepth(LAYERS.at("Level"))
        , mVolnerabilityTimer(sf::milliseconds(500))
        , mHitSound(LoadSoundBuffer(Resources::HitSound))
        , mShootSound(LoadSoundBuffer(Resources::ShootSound))
    {
        mBulletFireCooldown.Finish();
        mVolnerabilityTimer.Finish();

        ImportAssets(animTextureDirectory, status);
        mSprite.setTexture(mAnimation.GetTexture(), true);
        SetPosition(position);
    }

    virtual uint32_t GetDepth() const override
    {
        return mDepth;
    }

    virtual FloatRect GetGlobalBounds() const override
    {
        return GetTransform().transformRect(mSprite.getLocalBounds());
    }

    virtual void draw(sf::RenderTarget& target, const sf::RenderStates& states) const
    {
        sf::RenderStates statesCopy(states);
        statesCopy.transform *= GetTransform();
        target.draw(mSprite, statesCopy);
    }

    void Animate(const sf::Time& timeslice)
    {
        mAnimation.SetSequence(mStatus);
        mAnimation.Update(timeslice);
        mSprite.setTexture(mAnimation.GetTexture(), true);
    }

    void UpdateTimerCooldowns(const sf::Time& timeslice)
    {
        mBulletFireCooldown.Update(timeslice);
        mVolnerabilityTimer.Update(timeslice);
    }

    void FireBullet(const FloatRect& hotbox, float hortOffset, bool isPlayerBullet)
    {
        sf::Vector2f direction(-1.0f, 0.0f);
        if (SplitAndGetElement(mStatus, '_', 0) == "right")
        {
            direction.x = 1.0f;
        }
        sf::Vector2f position = hotbox.GetCenter() + direction * hortOffset;

        sf::Vector2f yOffset(0.0f, -16.0f);
        if (IsDucking())
        {
            yOffset.y = 10.0f;
        }

        mFireBulletCallback->FireBullet(position + yOffset, direction, *this, isPlayerBullet);
        mBulletFireCooldown.Reset(true);
    }

    bool CanFireBullet()
    {
        return mBulletFireCooldown.IsFinished();
    }

    const std::string& GetStatus()
    {
        return mStatus;
    }

    void SetStatus(const std::string& value)
    {
        mStatus = value;
    }

    void Demage()
    {
        if (mVolnerabilityTimer.IsFinished())
        {
            if (mHealth > 0)
            {
                mVolnerabilityTimer.Reset(true);
                mHealth -= 1;
                mHitSound.play();
            }
        }
    }

    void Blink()
    {
        if (!mVolnerabilityTimer.IsFinished())
        {
            mSprite.setColor(sf::Color::Red);
        }
        else
        {
            mSprite.setColor(sf::Color::White);
        }
    }

    virtual void CheckDeath() 
    { 
        if (mHealth <= 0)
        {            
            Kill();
        }
    }

    virtual bool IsDucking() const { return false; }

    uint32_t GetHealth() { return mHealth; }

    void PlayShootSound() { mShootSound.play(); }

    const sf::Sprite& GetSprite() { return mSprite; }

private:
    void ImportAssets(const std::string& animTextureDirectory, const std::string& animaSequenceId)
    {
        for (const std::string& directoryName : GetDirectoryNames(RESOURCES_PATH + animTextureDirectory))
        {
            std::string relativeDirectory = animTextureDirectory + "/" + directoryName + "/";
            mAnimation.AddSequence({ directoryName, LoadTexuresFromDirectory(relativeDirectory) });
        }
        mAnimation.SetSequence(animaSequenceId);
    }

    Animation mAnimation;
    sf::Sprite mSprite;
    std::string mStatus;
    Timer mBulletFireCooldown;
    Timer mVolnerabilityTimer;
    IFireBulletCallback* mFireBulletCallback;
    uint32_t mDepth;
    int32_t mHealth;
    sf::Sound mHitSound;
    sf::Sound mShootSound;    
};