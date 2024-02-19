#pragma once

// Includes
//------------------------------------------------------------------------------
// Third party
#include <SFML/Graphics.hpp>

// System
#include <memory>
#include <string>

//------------------------------------------------------------------------------
class AnimationSequence
{
public:
    AnimationSequence(const std::string& sequenceId, std::unique_ptr<std::vector<sf::Texture*>> sequence)
        : mSequenceId(sequenceId)
        , mSequence(std::move(sequence))
    { }

    const sf::Texture& GetTexture(uint32_t index) const
    {
        return *(*mSequence)[index];
    }

    size_t Size() const
    {
        return mSequence->size();
    }

    const std::string& GetSequenceId() const
    {
        return mSequenceId;
    }

private:
    std::string mSequenceId;
    std::unique_ptr<std::vector<sf::Texture*>> mSequence;
};

//------------------------------------------------------------------------------
class Animation
{
public:
    Animation(uint32_t framesPerSecond=7)
        : mFramesPerSecond(static_cast<float>(framesPerSecond))
        , mFrameIndex(0)
    { }

    bool Update(const sf::Time& timeslice)
    {
        bool isFinished = false;

        mFrameIndex += mFramesPerSecond * timeslice.asSeconds();
        if (mFrameIndex >= mCurrentSequence->Size())
        {
            mFrameIndex = 0;
            isFinished = true;
        }

        return isFinished;
    }

    void AddSequence(AnimationSequence&& sequence)
    {
        mSequenceMap.emplace(sequence.GetSequenceId(), std::move(sequence));
    }

    void SetSequence(const std::string& sequenceId)
    {
        if (!mCurrentSequence || mCurrentSequence->GetSequenceId() != sequenceId)
        {
            mFrameIndex = 0;
            mCurrentSequence = &mSequenceMap.at(sequenceId);
        }
    }

    std::string GetCurrentSequenceId() 
    { 
        return mCurrentSequence->GetSequenceId(); 
    }

    uint32_t GetFrameIndex()
    {
        return static_cast<uint32_t>(mFrameIndex);
    }

    const sf::Texture& GetTexture() const
    {
        return mCurrentSequence->GetTexture(static_cast<uint32_t>(mFrameIndex));
    }

    void Reset()
    {
        mFrameIndex = 0;
    }

private:
    std::unordered_map<std::string, AnimationSequence> mSequenceMap;
    AnimationSequence* mCurrentSequence = nullptr;
    float mFrameIndex;
    float mFramesPerSecond;
};