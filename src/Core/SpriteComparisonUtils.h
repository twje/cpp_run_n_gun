// Includes
//------------------------------------------------------------------------------
// Third party
#include <SFML/Graphics.hpp>

//------------------------------------------------------------------------------
bool BitmaskCompare(const sf::Texture& texture1,
    const sf::IntRect& textureRect1,
    const sf::Transformable& transformable1,
    const sf::Texture& texture2,
    const sf::IntRect& textureRect2,
    const sf::Transformable& transformable2);
bool BitmaskCompare(const sf::Sprite& sprite1,
    const sf::Transformable& transformable1,
    const sf::Sprite& sprite2,
    const sf::Transformable& transformable2);
bool BitmaskCompare(const sf::Sprite& sprite1, const sf::Sprite& sprite2);