#pragma once

// Includes
//------------------------------------------------------------------------------
// Third party
#include <SFML/Graphics.hpp>

//------------------------------------------------------------------------------
template<typename T>
void DrawRect(sf::RenderTarget& target, const sf::Rect<T>& rect, const sf::Color& color)
{
	sf::RectangleShape rectangleShape;
	rectangleShape.setSize(rect.getSize());
	rectangleShape.setPosition(rect.getPosition());
	rectangleShape.setFillColor({ 0, 0, 0, 0 });
	rectangleShape.setOutlineColor(color);
	rectangleShape.setOutlineThickness(-1);

	target.draw(rectangleShape);
}