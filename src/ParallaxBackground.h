#pragma once

// Includes
//------------------------------------------------------------------------------
// Third party
#include <SFML/Graphics.hpp>

// System
#include<vector>

//------------------------------------------------------------------------------
class ParallaxLayer
{
public:
    ParallaxLayer(const sf::Texture& texture, float hortSpeed)
        : mSprite(texture)
        , mHortSpeed(hortSpeed)
    { }

    void Draw(sf::RenderTarget& target, const sf::FloatRect& region)
    {
        float tileWidth = mSprite.getGlobalBounds().width;
        float initialOffset = 0.0f;

        // Ensure initialOffset is within the valid range [0, tileWidth)
        initialOffset = std::fmod(initialOffset, tileWidth);
        if (initialOffset < 0.0f)
        {
            // Adjust for negative offsets to wrap within the valid range
            initialOffset += tileWidth;
        }

        float parallaxOffset = std::fmod(region.left / mHortSpeed + initialOffset, tileWidth);
        float startX = region.left - parallaxOffset;
        if (region.left < 0)
        {
            startX -= tileWidth;
        }

        float tileHeight = mSprite.getGlobalBounds().height;
        for (float x = startX; x < region.left + region.width; x += tileWidth)
        {
            mSprite.setPosition({ x, region.top + (900 - region.getCenter().y / mHortSpeed) });
            target.draw(mSprite);
        }
    }

private:
    sf::Sprite mSprite;
    float mHortSpeed;    
};

//------------------------------------------------------------------------------
class ParallaxBackground
{
public:
    void AddLayer(ParallaxLayer&& layer)
    {
        mLayers.push_back(std::move(layer));
    }

    void Draw(sf::RenderTarget& target, const sf::FloatRect& region)
    {
        for (ParallaxLayer& layer : mLayers)
        {
            layer.Draw(target, region);
        }
    }

private:
    std::vector<ParallaxLayer> mLayers;
};