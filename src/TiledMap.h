#pragma once

// Includes
//------------------------------------------------------------------------------
// Core
#include "Core/Resources.h"
#include "Core/DrawUtils.h"
#include "Core/GameObject.h"

// Third party
#include <SFML/Graphics.hpp>
#include <tileson.hpp>

// System 
#include <filesystem>

namespace fs = std::filesystem;

//------------------------------------------------------------------------------
template<typename T>
sf::Rect<T> ConvertToSFMLRect(const tson::Rect& rect)
{
    return sf::Rect<T>({ static_cast<T>(rect.x),  static_cast<T>(rect.y) }, 
                       { static_cast<T>(rect.width),  static_cast<T>(rect.height) });
}

//------------------------------------------------------------------------------
template<typename T>
sf::Vector2f ConvertToSFMLVector2f(const tson::Vector2<T>& tVector)
{
    return sf::Vector2f(static_cast<float>(tVector.x), static_cast<float>(tVector.y));
}

//------------------------------------------------------------------------------
class TiledMap
{
public:
    TiledMap(fs::path filepath)
    {
        fs::path fullpath = RESOURCES_PATH + filepath.generic_string();
        tson::Tileson parser;                
        mMap = parser.parse(fullpath);        

        for (tson::Tileset& tileset : mMap->getTilesets())
        {
            LoadTilesetTextures(tileset, fullpath.parent_path());
        }
    }    

    const std::vector<tson::Object>& GetTileObjectData(const std::string& layerName) const
    {
        return mMap->getLayer(layerName)->getObjects();
    }

    size_t LayerCount() 
    { 
        return mMap->getLayers().size(); 
    }
    
    std::vector<tson::Layer>& GetLayers()
    {
        return mMap->getLayers();
    }

    tson::Layer& GetLayer(const std::string& layerName)
    {
        return GetLayer(GetLayerIndexByName(layerName));
    }

    tson::Layer& GetLayer(uint32_t index) 
    { 
        return mMap->getLayers().at(index); 
    }

    uint32_t GetLayerIndexByName(const std::string& layerName)
    {     
        uint32_t index = 0;

        for (const auto& layer : mMap->getLayers())
        {
            if (layer.getName() == layerName)
            {
                return index;
            }
            ++index;
        }

        return std::numeric_limits<uint32_t>::max();
    }

    sf::Vector2f GetTileSize()
    {    
        return ConvertToSFMLVector2f(mMap->getTileSize());        
    }    

    sf::Vector2f GetMapSize()
    {
        sf::Vector2f tileSize = GetTileSize();
        sf::Vector2f tileCount = ConvertToSFMLVector2f(mMap->getSize());

        return { tileSize.x * tileCount.x, tileSize.y * tileCount.y };
    }

    const sf::Texture& GetTexture(uint32_t gid)
    {
        return *mTextureLookup.at(gid);
    }

    const sf::IntRect GetTextureRegion(uint32_t gid)
    {
        tson::Tile* tile = GetTileByGid(gid);
        return ConvertToSFMLRect<int32_t>(tile->getDrawingRect());
    }

    const std::pair<const sf::Texture*, sf::IntRect> GetTextureAndRegion(uint32_t gid)
    {
        return std::make_pair(&GetTexture(gid), GetTextureRegion(gid));
    }

private:
    tson::Tile* GetTileByGid(uint32_t gid)
    {
        tson::Tileset* tileset = mMap->getTilesetByGid(gid);
        uint32_t id = gid - tileset->getFirstgid() + 1;
        return tileset->getTile(id);
    }

    void LoadTilesetTextures(tson::Tileset& tileset, fs::path relativeMapDir)
    {
        if (tileset.getType() == tson::TilesetType::ImageTileset)
        {
            fs::path relativePath = fs::relative(tileset.getFullImagePath(), RESOURCES_PATH);
            sf::Texture* texture = &LoadTexture(relativePath.generic_string());
            for (const auto& tile : tileset.getTiles())
            {
                mTextureLookup[tile.getGid()] = texture;
            }
        }
        else if (tileset.getType() == tson::TilesetType::ImageCollectionTileset)
        {
            for (const auto& tile : tileset.getTiles())
            {
                fs::path fullPath = relativeMapDir / tile.getImage();
                fs::path relativePath = fs::relative(fullPath, RESOURCES_PATH);
                mTextureLookup[tile.getGid()] = &LoadTexture(relativePath.generic_string());
            }
        }
    }

    std::unique_ptr<tson::Map> mMap;    
    std::unordered_map<uint32_t, sf::Texture*> mTextureLookup;    
};

//------------------------------------------------------------------------------
struct TiledMapVisibleRegion
{
    TiledMapVisibleRegion(const sf::FloatRect visibleRegion, const sf::Vector2f tileSize)
    {
        mStartX = static_cast<int32_t>(std::floor(visibleRegion.left / tileSize.x));
        mStartY = static_cast<int32_t>(std::floor(visibleRegion.top / tileSize.y));
        mEndX = static_cast<int32_t>(std::ceil((visibleRegion.left + visibleRegion.width) / tileSize.x));
        mEndY = static_cast<int32_t>(std::ceil((visibleRegion.top + visibleRegion.height) / tileSize.y));
    }

    int32_t mStartX;
    int32_t mStartY;
    int32_t mEndX;
    int32_t mEndY;
};

//------------------------------------------------------------------------------
class RenderableTileLayer
{
    static constexpr uint32_t TILE_VERTEX_COUNT = 6;

public:
    RenderableTileLayer(TiledMap& tiledMap, tson::Layer& layer, const sf::Vector2u& windowSize)
        : mTiledMap(tiledMap)
        , mLayer(layer)
    {
        mTileVertices.resize(layer.getSize().x * layer.getSize().y * TILE_VERTEX_COUNT);
        for (int32_t tileY = 0; tileY < layer.getSize().y; tileY++)
        {
            for (int32_t tileX = 0; tileX < layer.getSize().x; tileX++)
            {
                tson::TileObject* tileObject = layer.getTileObject(tileX, tileY);
                if (!tileObject)
                {
                    continue;
                }

                tson::Tile* tile = layer.getTileObject(tileX, tileY)->getTile();
                sf::FloatRect textureRegion = ConvertToSFMLRect<float>(tile->getDrawingRect());
                
                size_t index = (tileX + tileY * mLayer.getSize().x) * TILE_VERTEX_COUNT;
                sf::Vector2f tileSize = tiledMap.GetTileSize();

                // Position
                mTileVertices[index + 0].position = { tileX * tileSize.x, tileY * tileSize.y };
                mTileVertices[index + 1].position = { (tileX + 1) * tileSize.x, tileY * tileSize.y };
                mTileVertices[index + 2].position = { tileX * tileSize.x, (tileY + 1) * tileSize.y };

                mTileVertices[index + 3].position = { tileX * tileSize.x, (tileY + 1) * tileSize.y };
                mTileVertices[index + 4].position = { (tileX + 1) * tileSize.x, tileY * tileSize.y };
                mTileVertices[index + 5].position = { (tileX + 1) * tileSize.x, (tileY + 1) * tileSize.y };

                // Texture coordinates
                mTileVertices[index + 0].texCoords = sf::Vector2f(textureRegion.left, textureRegion.top);
                mTileVertices[index + 1].texCoords = sf::Vector2f(textureRegion.left + textureRegion.width, textureRegion.top);
                mTileVertices[index + 2].texCoords = sf::Vector2f(textureRegion.left, textureRegion.top + textureRegion.height);

                mTileVertices[index + 3].texCoords = sf::Vector2f(textureRegion.left, textureRegion.top + textureRegion.height);
                mTileVertices[index + 4].texCoords = sf::Vector2f(textureRegion.left + textureRegion.width, textureRegion.top);
                mTileVertices[index + 5].texCoords = sf::Vector2f(textureRegion.left + textureRegion.width, textureRegion.top + textureRegion.height);
         
                // Optimisation
                sf::Texture* texture = const_cast<sf::Texture*>(&mTiledMap.GetTexture(tile->getGid()));
                mLayerVertices[texture].reserve(GetVertexReserveSize(windowSize));
            }
        }        
    }

    size_t GetVertexReserveSize(const sf::Vector2u& windowSize)
    {
        uint32_t maxHortTiles = windowSize.x / static_cast<uint32_t>(mTiledMap.GetTileSize().x);
        uint32_t maxVertTiles = windowSize.y / static_cast<uint32_t>(mTiledMap.GetTileSize().y);
        return maxHortTiles * maxVertTiles * TILE_VERTEX_COUNT;
    }

    void Draw(sf::RenderWindow& window, const TiledMapVisibleRegion& visibleRegion)
    {
        for (auto& [_, vertices] : mLayerVertices)
        {
            vertices.clear();
        }

        // Iterate over the visible region
        for (int32_t tileY = visibleRegion.mStartY; tileY < visibleRegion.mEndY; ++tileY)
        {
            for (int32_t tileX = visibleRegion.mStartX; tileX < visibleRegion.mEndX;)
            {
                tson::TileObject* tileObject = mLayer.getTileObject(tileX, tileY);
                if (!tileObject)
                {
                    ++tileX; // Move to the next tile if the current one is empty
                    continue;
                }

                tson::Tile* tile = tileObject->getTile();
                sf::Texture* texture = const_cast<sf::Texture*>(&mTiledMap.GetTexture(tile->getGid()));

                // Start batching from this tile
                size_t startIndex = (tileX + tileY * mLayer.getSize().x) * TILE_VERTEX_COUNT;
                size_t endIndex = startIndex + TILE_VERTEX_COUNT; // Initially assume only one tile in the batch

                // Attempt to batch consecutive tiles with the same texture
                int32_t nextTileX = tileX + 1;
                while (nextTileX < visibleRegion.mEndX)
                {
                    tson::TileObject* nextTileObject = mLayer.getTileObject(nextTileX, tileY);
                    if (!nextTileObject || const_cast<sf::Texture*>(&mTiledMap.GetTexture(nextTileObject->getTile()->getGid())) != texture)
                    {
                        break; // End batch if the next tile is different or empty
                    }

                    endIndex += TILE_VERTEX_COUNT; // Extend the batch
                    ++nextTileX;
                }

                // Insert all vertices of the batched tiles at once
                std::vector<sf::Vertex>& layerVertices = mLayerVertices[texture];
                layerVertices.insert(layerVertices.end(), mTileVertices.begin() + startIndex, mTileVertices.begin() + endIndex);

                tileX = nextTileX; // Continue from the tile next to the batch
            }
        }

        // Draw all batched tiles
        for (auto& [texture, vertices] : mLayerVertices)
        {
            if (!vertices.empty())
            {
                sf::RenderStates renderStates;
                renderStates.texture = texture;
                window.draw(&vertices[0], vertices.size(), sf::PrimitiveType::Triangles, renderStates);
            }
        }
    }

private:
    TiledMap& mTiledMap;
    tson::Layer& mLayer;
    std::vector<sf::Vertex> mTileVertices;
    std::unordered_map<sf::Texture*, std::vector<sf::Vertex>> mLayerVertices;
};

//------------------------------------------------------------------------------
class TiledMapLayerRenderer
{    
public:
    TiledMapLayerRenderer(TiledMap& tiledMap, const sf::Vector2u& windowSize)
        : mTiledMap(tiledMap)
        , mShouldRenderLayer(tiledMap.LayerCount(), false)
    { 
        for (uint32_t index = 0; index < tiledMap.LayerCount(); index++)
        {
            tson::Layer& layer = tiledMap.GetLayer(index);
            if (layer.getType() == tson::LayerType::TileLayer)
            {
                mTileLayers.emplace(index, RenderableTileLayer(tiledMap, layer, windowSize));
            }
        }
    }

    void EnablAllLayersForRender()
    {
        // Use indicies instead - std::vector<bool> yields temporary values instead of actual references to its elements
        // and cannot bind to a non-const lvalue reference
        for (size_t i = 0; i < mShouldRenderLayer.size(); ++i) 
        {
            mShouldRenderLayer[i] = true;
        }
    }

    void EnablLayerForRender(const std::string& layerName)
    {
        uint32_t index = mTiledMap.GetLayerIndexByName(layerName);
        assert(index < mTiledMap.LayerCount());
        mShouldRenderLayer[index] = true;
    }

    void DrawLayer(sf::RenderWindow& window, uint32_t index, const TiledMapVisibleRegion& visibleRegion)
    {
        if (!mShouldRenderLayer[index])
        {
            return;
        }

        tson::Layer& layer = mTiledMap.GetLayer(index);

        if (layer.isVisible())
        {
            switch (layer.getType())
            {
                case tson::LayerType::TileLayer:
                {
                    RenderableTileLayer& tileLayer = mTileLayers.at(index);
                    tileLayer.Draw(window, visibleRegion);
                    break;
                }
            }
        }
    }

private:
    TiledMap& mTiledMap;   
    std::vector<bool> mShouldRenderLayer;     
    std::unordered_map<size_t, RenderableTileLayer> mTileLayers;
};

//------------------------------------------------------------------------------
class Tile : public GameObject
{
public:
    Tile(tson::Tile& tile, const sf::Vector2f& position, const sf::Vector2f& size)
        : mTile(tile)
        , mBounds(position, size)
    { }

    virtual FloatRect GetGlobalBounds() const 
    { 
        return mBounds; 
    }

    virtual const sf::Vector2f GetVelocity() const 
    { 
        return sf::Vector2f(); 
    }

private:
    FloatRect mBounds;
    tson::Tile& mTile;
};

//------------------------------------------------------------------------------
class TiledLayerSpatialQuery
{
public:
    TiledLayerSpatialQuery(tson::Layer& tileLayer)
        : mTileData(tileLayer.getTileData())
        , mTileSize(ConvertToSFMLVector2f(tileLayer.getMap()->getTileSize()))
    {
        assert(tileLayer.getType() == tson::LayerType::TileLayer);
    }

    const std::vector<Tile>& QueryRegion(const sf::FloatRect& region) const
    {
        int32_t startX = static_cast<int32_t>(std::floor(region.left / mTileSize.x));
        int32_t startY = static_cast<int32_t>(std::floor(region.top / mTileSize.y));
        int32_t endX = static_cast<int32_t>(std::ceil((region.left + region.width) / mTileSize.x));
        int32_t endY = static_cast<int32_t>(std::ceil((region.top + region.height) / mTileSize.y));

        mQueryResult.clear();
        for (int32_t tileY = startY; tileY < endY; tileY++)
        {
            for (int32_t tileX = startX; tileX < endX; tileX++)
            {                
                std::tuple<int32_t, int32_t> key = { tileX, tileY };
                if (mTileData.find(key) != mTileData.end())
                {
                    sf::Vector2f position(tileX * mTileSize.x, tileY * mTileSize.y);
                    mQueryResult.emplace_back(*mTileData.at(key), position, mTileSize);
                }
            }
        }

        return mQueryResult;
    }

private:
    const std::map<std::tuple<int32_t, int32_t>, tson::Tile*> mTileData;
    sf::Vector2f mTileSize;
    mutable std::vector<Tile> mQueryResult;
};