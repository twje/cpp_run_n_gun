#include "SpriteComparisonUtils.h"

// Includes
//------------------------------------------------------------------------------
// System
#include <algorithm>

//------------------------------------------------------------------------------
sf::IntRect GetTransformedBounds(const sf::Transformable& transformable, const sf::Vector2u size)
{
    sf::FloatRect localBounds(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(size));
    return sf::IntRect(transformable.getTransform().transformRect(localBounds));
}

//------------------------------------------------------------------------------
sf::Image CreateImageFromTexture(const sf::Texture& texture, const sf::IntRect& textureRect)
{
    if (texture.getSize() != sf::Vector2u(textureRect.getSize()))
    {
        sf::RenderTexture renderTexture;
        if (!renderTexture.create(sf::Vector2u(textureRect.getSize())))
        {
            throw std::runtime_error("Render texture creation failed");
        }

        // Preserve source alpha chanel
        sf::BlendMode blendMode(sf::BlendMode::Factor::SrcColor, sf::BlendMode::Factor::Zero);

        renderTexture.clear();
        renderTexture.draw(sf::Sprite(texture, textureRect), blendMode);
        renderTexture.display();

        return renderTexture.getTexture().copyToImage();
    }
    else
    {
        return texture.copyToImage();
    }
}

//------------------------------------------------------------------------------
bool ComparePixels(const sf::Image& image1,
    const sf::Image& image2,
    const sf::IntRect& compareBounds,
    const sf::Transform& inverseTransform1,
    const sf::Transform& inverseTransform2)
{
    for (int32_t y = compareBounds.top; y < compareBounds.top + compareBounds.height; ++y)
    {
        for (int32_t x = compareBounds.left; x < compareBounds.left + compareBounds.width; ++x)
        {
            sf::Vector2f globalPos = sf::Vector2f(static_cast<float>(x), static_cast<float>(y));
            sf::Vector2i localPos1 = sf::Vector2i(inverseTransform1.transformPoint(globalPos));
            sf::Vector2i localPos2 = sf::Vector2i(inverseTransform2.transformPoint(globalPos));

            sf::Vector2u imageSize1 = image1.getSize();
            sf::Vector2u imageSize2 = image2.getSize();

            bool isInImage1Bounds = (0 <= localPos1.x && localPos1.x < imageSize1.x && 0 <= localPos1.y && localPos1.y < imageSize1.y);
            bool isInImage2Bounds = (0 <= localPos2.x && localPos2.x < imageSize2.x && 0 <= localPos2.y && localPos2.y < imageSize2.y);

            if (isInImage1Bounds && isInImage2Bounds)
            {
                sf::Color pixel1 = image1.getPixel(sf::Vector2u(localPos1));
                sf::Color pixel2 = image2.getPixel(sf::Vector2u(localPos2));

                if (pixel1.a != 0 && pixel2.a != 0)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

//------------------------------------------------------------------------------
bool BitmaskCompare(const sf::Texture& texture1,
    const sf::IntRect& textureRect1,
    const sf::Transformable& transformable1,
    const sf::Texture& texture2,
    const sf::IntRect& textureRect2,
    const sf::Transformable& transformable2)
{

    sf::Image image1 = CreateImageFromTexture(texture1, textureRect1);
    sf::Image image2 = CreateImageFromTexture(texture2, textureRect2);

    sf::IntRect bounds1 = GetTransformedBounds(transformable1, image1.getSize());
    sf::IntRect bounds2 = GetTransformedBounds(transformable2, image2.getSize());

    std::optional<sf::IntRect> compareBounds = bounds1.findIntersection(bounds2);
    if (!compareBounds)
    {
        return false;
    }

    return ComparePixels(image1,
        image2,
        compareBounds.value(),
        transformable1.getInverseTransform(),
        transformable2.getInverseTransform());
}

//------------------------------------------------------------------------------
bool BitmaskCompare(const sf::Sprite& sprite1,
    const sf::Transformable& transformable1,
    const sf::Sprite& sprite2,
    const sf::Transformable& transformable2)
{
    return BitmaskCompare(sprite1.getTexture(),
        sprite1.getTextureRect(),
        transformable1,
        sprite2.getTexture(),
        sprite2.getTextureRect(),
        transformable2);
}

//------------------------------------------------------------------------------
bool BitmaskCompare(const sf::Sprite& sprite1, const sf::Sprite& sprite2)
{
    return BitmaskCompare(sprite1.getTexture(),
        sprite1.getTextureRect(),
        sprite1,
        sprite2.getTexture(),
        sprite2.getTextureRect(),
        sprite2);
}