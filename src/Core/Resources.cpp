#include "Resources.h"

// Includes
//------------------------------------------------------------------------------
// System
#include <unordered_map>
#include <iostream>
#include <stdexcept>
#include <filesystem>

namespace fs = std::filesystem;

//------------------------------------------------------------------------------
std::unique_ptr<std::vector<sf::Texture*>> LoadTexuresFromDirectory(const std::string directory)
{
    auto textures = std::make_unique<std::vector<sf::Texture*>>();

    for (const auto& entry : fs::directory_iterator(std::string(RESOURCES_PATH) + directory))
    {
        std::string filepath = directory + entry.path().filename().generic_string();
        textures->push_back(&LoadTexture(filepath));
    }
    return textures;
}

//------------------------------------------------------------------------------
sf::Texture& LoadTexture(const std::string& filename)
{
    static std::unordered_map<std::string, sf::Texture> textureStore;

    auto it = textureStore.find(filename);
    if (it != textureStore.end())
    {
        return it->second;
    }

    sf::Texture texture;
    if (!texture.loadFromFile(std::string(RESOURCES_PATH) + filename))
    {
        throw std::runtime_error("Failed to load texture: " + filename);
    }

    auto inserted = textureStore.emplace(filename, std::move(texture));
    return inserted.first->second;
}

//------------------------------------------------------------------------------
const sf::Font& LoadFont(const std::string& filename)
{
    static std::unordered_map<std::string, sf::Font> fontStore;
    auto it = fontStore.find(filename);
    if (it != fontStore.end())
    {
        return it->second;
    }

    sf::Font font;
    if (!font.loadFromFile(std::string(RESOURCES_PATH) + filename))
    {
        throw std::runtime_error("Failed to load font: " + filename);
    }

    auto inserted = fontStore.emplace(filename, std::move(font));
    return inserted.first->second;
}

//------------------------------------------------------------------------------
const sf::SoundBuffer& LoadSoundBuffer(const std::string& filename)
{
    static std::unordered_map<std::string, sf::SoundBuffer> soundBufferStore;
    auto it = soundBufferStore.find(filename);
    if (it != soundBufferStore.end())
    {
        return it->second;
    }

    sf::SoundBuffer soundBuffer;
    if (!soundBuffer.loadFromFile(std::string(RESOURCES_PATH) + filename))
    {
        throw std::runtime_error("Failed to load font: " + filename);
    }

    auto inserted = soundBufferStore.emplace(filename, std::move(soundBuffer));
    return inserted.first->second;
}

//------------------------------------------------------------------------------
sf::Music& LoadMusic(const std::string& filename)
{
    static std::unordered_map<std::string, std::unique_ptr<sf::Music>> musicStore;

    auto it = musicStore.find(filename);
    if (it != musicStore.end())
    {
        return *(it->second);
    }

    std::unique_ptr<sf::Music> music = std::make_unique<sf::Music>();
    if (!music->openFromFile(std::string(RESOURCES_PATH) + filename))
    {
        throw std::runtime_error("Failed to load music: " + filename);
    }

    auto& storedMusic = *music;
    musicStore[filename] = std::move(music);

    return storedMusic;
}