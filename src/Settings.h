#pragma once

// Includes
//------------------------------------------------------------------------------
// System
#include <cstdint>
#include <unordered_map>
#include <string>

constexpr uint32_t WINDOW_WIDTH = 1280;
constexpr uint32_t WINDOW_HEIGHT = 720;
constexpr uint32_t MAX_LEVEL_HEIGHT = 3500;

const extern std::unordered_map<std::string, uint32_t> LAYERS;

namespace Resources{
 
    constexpr char TiledMap[] = "data/map.json";
    constexpr char PlaceholderTexture[] = "graphics/placeholder.png";
    constexpr char BulletTexture[] = "graphics/bullet.png";
    constexpr char fireAnimationTexture0[] = "graphics/fire/0.png";
    constexpr char fireAnimationTexture1[] = "graphics/fire/1.png";
    constexpr char ParallaxForeground[] = "graphics/sky/fg_sky.png";
    constexpr char ParallaxBackground[] = "graphics/sky/bg_sky.png";
    constexpr char HealthPoint[] = "graphics/health.png";
    constexpr char Music[] = "audio/music.wav";
    constexpr char HitSound[] = "audio/hit.wav";
    constexpr char ShootSound[] = "audio/bullet.wav";
    constexpr char Font[] = "fonts/subatomic.ttf";
}
