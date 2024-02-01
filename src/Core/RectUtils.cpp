// Includes
//------------------------------------------------------------------------------
#include "RectUtils.h"

//------------------------------------------------------------------------------
sf::FloatRect InflateRect(const sf::FloatRect& source, float inflateX, float inflateY)
{
    sf::FloatRect target = source;

    target.left = source.left - inflateX / 2.0f;
    target.width = source.width + inflateX;
    target.top = source.top - inflateY / 2.0f;
    target.height = source.height + inflateY;
    return target;
}

//------------------------------------------------------------------------------
void MoveRect(sf::FloatRect& rect, const sf::Vector2f& delta)
{
    rect.left += delta.x;
    rect.top += delta.y;
}

//------------------------------------------------------------------------------
sf::Vector2f GetRectCenter(const sf::FloatRect& rect)
{
    return sf::Vector2f(rect.left + rect.width / 2.0f, rect.top + rect.height / 2.0f);
}

//------------------------------------------------------------------------------
sf::Vector2f GetRectMidRight(const sf::FloatRect& rect)
{
    return sf::Vector2f(rect.left + rect.width, rect.top + rect.height / 2.0f);
}

//------------------------------------------------------------------------------
sf::Vector2f GetRectMidLeft(const sf::FloatRect& rect)
{
    return sf::Vector2f(rect.left, rect.top + rect.height / 2.0f);
}

//------------------------------------------------------------------------------
sf::Vector2f GetRectMidTop(const sf::FloatRect& rect)
{
    return sf::Vector2f(rect.left + rect.width / 2.0f, rect.top);
}

//------------------------------------------------------------------------------
sf::Vector2f GetRectMidBottom(const sf::FloatRect& rect)
{
    return sf::Vector2f(rect.left + rect.width / 2.0f, rect.top + rect.height);
}