#pragma once

// Includes
//------------------------------------------------------------------------------
// Third party
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

// System
#include <string>

//------------------------------------------------------------------------------
std::unique_ptr<std::vector<sf::Texture*>> LoadTexuresFromDirectory(const std::string directory);
sf::Texture& LoadTexture(const std::string& filename);
const sf::Font& LoadFont(const std::string& filename);
const sf::SoundBuffer& LoadSoundBuffer(const std::string& filename);
sf::Music& LoadMusic(const std::string& filename);