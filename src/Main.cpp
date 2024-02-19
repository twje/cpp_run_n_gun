// Includes
//------------------------------------------------------------------------------
// Game
#include "Settings.h"
#include "TiledMap.h"
#include "Player.h"
#include "MovingPlatform.h"
#include "Interface.h"
#include "Bullet.h"
#include "ParallaxBackground.h"
#include "Enemy.h"

// Third party
#include <SFML/Graphics.hpp>

// Core
#include "Core/GameObjectManager.h"
#include "Core/Resources.h"
#include "Core/SpriteComparisonUtils.h"

//------------------------------------------------------------------------------
class Overlay
{
public:
    Overlay(Player& player)
        : mPlayer(player)
        , mHealthPoint(LoadTexture(Resources::HealthPoint))
    { }

    void Draw(sf::RenderWindow& window)
    {     
        sf::FloatRect bounds = mHealthPoint.getLocalBounds();
        for (uint32_t hp = 0; hp < mPlayer.GetHealth(); hp++)
        {
            float x = 10.0f + hp * (bounds.width + 4.0f);
            float y = 10.0f;
            mHealthPoint.setPosition({ x, y });
            window.draw(mHealthPoint);
        }
    }

private:
    Player& mPlayer;
    sf::Sprite mHealthPoint;
};

//------------------------------------------------------------------------------
class LayerStack;

//------------------------------------------------------------------------------
class Layer
{
public:
    virtual ~Layer() = default;
    Layer(LayerStack& layerStack)
        : mLayerStack(layerStack)
    { }

    // Hooks
    virtual bool HandleEvent(const sf::Event& event) { return true; };
    virtual bool Update(const sf::Time& timeslice) { return true; };
    virtual bool Draw(sf::RenderWindow& window) { return true; };
    virtual void Resize(const sf::Vector2f& size) { };
    virtual void OnEnter() { };
    virtual void OnExit() { };

    // Layer management    
    LayerStack& GetLayerStack() { return mLayerStack; }

private:
    LayerStack& mLayerStack;
};

//------------------------------------------------------------------------------
class LayerStack 
{
public:
    void PushLayer(std::unique_ptr<Layer> layer) 
    {
        if (layer) 
        {
            layer->OnEnter();
            mLayers.push_back(std::move(layer));
        }
    }

    void PopLayer() 
    {
        if (!mLayers.empty()) 
        {
            mLayers.back()->OnExit();
            mLayers.pop_back();
        }
    }

    Layer* GetTop() { return mLayers.back().get(); }

    void Clear() 
    {
        for (auto& layer : mLayers) 
        {
            layer->OnExit();
        }
        mLayers.clear();
    }

    void HandleEvent(const sf::Event& event)
    {
        for (size_t i = mLayers.size(); i-- > 0; )
        {
            if (!mLayers[i]->HandleEvent(event))
            {
                break;
            }
        }
    }

    void Update(const sf::Time& timeslice) 
    {
        for (size_t i = mLayers.size(); i-- > 0; )
        {
            if (!mLayers[i]->Update(timeslice)) 
            {
                break;
            }
        }
    }

    void Draw(sf::RenderWindow& window) 
    {
        for (size_t i = 0; i < mLayers.size(); ++i) 
        {
            if (!mLayers[i]->Draw(window)) {
                break;
            }
        }
    }

    void Resize(const sf::Vector2f& size) {
        for (size_t i = 0; i < mLayers.size(); ++i)
        {
            mLayers[i]->Resize(size);
        }
    }

private:
    std::vector<std::unique_ptr<Layer>> mLayers;
};

//------------------------------------------------------------------------------
class Pause : public Layer
{
public:
    Pause(LayerStack& layerStack, const sf::Vector2u& windowSize)
        : Layer(layerStack)
        , mPauseText(LoadFont(Resources::Font), "", 30)
    { 
        mView.setSize(sf::Vector2f(windowSize));
        mView.setCenter(sf::Vector2f(windowSize) / 2.0f);

        mOverlay.setSize(sf::Vector2f(windowSize));
        mOverlay.setFillColor(sf::Color(0, 0, 0, 64));

        mPauseText.setString("Pause");
        mPauseText.setOrigin(GetRectCenter(mPauseText.getLocalBounds()));
        mPauseText.setPosition({ windowSize.x / 2.0f, windowSize.y / 4.0f });
    }

    virtual bool HandleEvent(const sf::Event& event) 
    { 
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Pause)
        {
            GetLayerStack().PopLayer();
        }
        return false; 
    };
    
    virtual bool Update(const sf::Time& timeslice) 
    { 
        return false; 
    };

    virtual bool Draw(sf::RenderWindow& window) 
    { 
        window.setView(mView);
        window.draw(mOverlay);
        window.draw(mPauseText);

        return true; 
    };
        
    virtual void Resize(const sf::Vector2f& size) 
    {
        mView.setSize(size);
    };

private:
    sf::RectangleShape mOverlay;
    sf::View mView;
    sf::Text mPauseText;
};

//------------------------------------------------------------------------------
class IGame
{
public:
    virtual void ResetScene() = 0;
};

//------------------------------------------------------------------------------
class GameOver : public Layer
{
public:
    GameOver(LayerStack& layerStack, IGame& game, const sf::Vector2u& windowSize)
        : Layer(layerStack)
        , mGame(game)
        , mGameOverText(LoadFont(Resources::Font), "", 30)
        , mRestartText(LoadFont(Resources::Font), "", 30)
    {
        mView.setSize(sf::Vector2f(windowSize));
        mView.setCenter(sf::Vector2f(windowSize) / 2.0f);

        mOverlay.setSize(sf::Vector2f(windowSize));
        mOverlay.setFillColor(sf::Color(0, 0, 0, 64));    

        mGameOverText.setString("Game Over");
        mGameOverText.setOrigin(GetRectCenter(mGameOverText.getLocalBounds()));
        mGameOverText.setPosition({ windowSize.x / 2.0f, windowSize.y / 4.0f });

        mRestartText.setString("Press SPacebar to Restart");
        mRestartText.setOrigin(GetRectCenter(mRestartText.getLocalBounds()));
        mRestartText.setPosition({ windowSize.x / 2.0f, windowSize.y / 3.0f });
    }

    virtual bool HandleEvent(const sf::Event& event)
    {
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Space)
        {
            mGame.ResetScene();
            GetLayerStack().PopLayer();
        }
        return false;
    };

    virtual bool Update(const sf::Time& timeslice)
    {
        return false;
    };

    virtual bool Draw(sf::RenderWindow& window)
    {
        window.setView(mView);
        window.draw(mOverlay);
        window.draw(mGameOverText);
        window.draw(mRestartText);

        return true;
    };

    virtual void Resize(const sf::Vector2f& size)
    {
        mView.setSize(size);
    };

private:
    sf::RectangleShape mOverlay;
    sf::View mView;
    IGame& mGame;
    sf::Text mGameOverText;
    sf::Text mRestartText;
};

//------------------------------------------------------------------------------
class Game : public Layer, public IGame, public IFireBulletCallback
{
public:
    Game(LayerStack& layerStack, GameObjectManager& manager, const sf::Vector2u& windowSize)
        : Layer(layerStack)
        , mManager(manager)
        , mTiledMap(Resources::TiledMap)
        , mPlayer{ nullptr }
        , mMusic(LoadMusic(Resources::Music))
    {         
        mLayerRenderer = std::make_unique<TiledMapLayerRenderer>(mTiledMap, windowSize);
        mLayerRenderer->EnablAllLayersForRender();

        mMusic.play();
        mMusic.setLoop(true);

        mGameView.setSize(sf::Vector2f(windowSize));
        mGameView.setCenter(sf::Vector2f(windowSize) / 2.0f);
        mHUDView = mGameView;

        mCollisionLayer = std::make_unique<TiledLayerSpatialQuery>(mTiledMap.GetLayer("Level"));

        ParallaxLayer foregroundLayer(LoadTexture(Resources::ParallaxBackground), 2.5f);
        ParallaxLayer backgroundLayer(LoadTexture(Resources::ParallaxForeground), 2.0f);

        mBackground.AddLayer(std::move(foregroundLayer));
        mBackground.AddLayer(std::move(backgroundLayer));

        PopulateScene(windowSize);
    }

    void PopulateScene(const sf::Vector2u& windowSize)
    {        
        mGameView.setSize(sf::Vector2f(windowSize));
        mGameView.setCenter(sf::Vector2f(windowSize) / 2.0f);
        mHUDView = mGameView;

        mCollisionLayer = std::make_unique<TiledLayerSpatialQuery>(mTiledMap.GetLayer("Level"));

        for (const tson::Object& object : mTiledMap.GetTileObjectData("Entities"))
        {
            if (object.getName() == "Player")
            {
                mPlayerStartposition = ConvertToSFMLVector2f(object.getPosition());
                mPlayer = mManager.CreateGameObject<Player>(mPlayerStartposition, *mCollisionLayer, mCollisionObjects, this);
                mDrawGroup.AddGameObject(mPlayer);
                mGameView.setCenter(mPlayerStartposition);
            }
            else if (object.getName() == "Enemy")
            {
                sf::Vector2f position = ConvertToSFMLVector2f(object.getPosition());
                Enemy* enemy = mManager.CreateGameObject<Enemy>(position, *mPlayer, *mCollisionLayer, this);
                mDrawGroup.AddGameObject(enemy);
                mVulnerableObjects.AddGameObject(enemy);
                mPreUpdateGroup.AddGameObject(enemy);
            }
        }

        for (const tson::Object& object : mTiledMap.GetTileObjectData("Platforms"))
        {
            if (object.getName() == "Platform")
            {
                auto [texture, textureRegion] = mTiledMap.GetTextureAndRegion(object.getGid());
                sf::Vector2f position = ConvertToSFMLVector2f(object.getPosition());
                position.y -= textureRegion.height; // Tiled map object origin is bottom left

                auto platform = mManager.CreateGameObject<MovingPlatform>(position, *texture, textureRegion, mPlatformWayPoints);
                mDrawGroup.AddGameObject(platform);
                mCollisionObjects.AddGameObject(platform);
                mPreUpdateGroup.AddGameObject(platform);
            }
            else if (object.getName() == "Border")
            {
                sf::Vector2f position = ConvertToSFMLVector2f(object.getPosition());
                sf::Vector2f size = ConvertToSFMLVector2f(object.getSize());
                mPlatformWayPoints.push_back(sf::FloatRect(position, size));
            }
        }

        mOverlay = std::make_unique<Overlay>(*mPlayer);
    }

    virtual void ResetScene() override
    {
        GameObjectManager::Instance().RemoveAllGameObjects();
        PopulateScene(sf::Vector2u(mGameView.getSize()));
    }

    virtual void FireBullet(const sf::Vector2f& position, const sf::Vector2f& direction, Entity& entity, bool isPlayerBullet) override
    {        
        sf::Color tintColor = sf::Color::White;
        if (isPlayerBullet)
        {
            tintColor = sf::Color(128, 0, 255, 255);
        }

        auto bullet = mManager.CreateGameObject<Bullet>(position, direction, tintColor);
        mDrawGroup.AddGameObject(bullet);        
        mPostUpdateGroup.AddGameObject(bullet);
        mBulletObjects.AddGameObject(bullet);

        auto fireAnimation = mManager.CreateGameObject<FireAnimation>(entity.GetEntityId(), direction, tintColor);
        mDrawGroup.AddGameObject(fireAnimation);
        mPostUpdateGroup.AddGameObject(fireAnimation);

        if (isPlayerBullet)
        {
            mPlayerBulletObjects.AddGameObject(bullet);
        }
        else
        {
            mEnemyBulletObjects.AddGameObject(bullet);
        }
    }

    virtual bool HandleEvent(const sf::Event& event)
    {
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Pause)
        {
            GetLayerStack().PushLayer(std::make_unique<Pause>(GetLayerStack(), sf::Vector2u(mGameView.getSize())));
        }
        return true;
    }

    virtual void Resize(const sf::Vector2f& size) override
    {
        mGameView.setSize(size);      
    }

    void BulletCollision()
    {
        // Enevironment collision
        for (GameObject* bullet : mBulletObjects)
        {
            for (const GameObject& obj : mCollisionLayer->QueryRegion(bullet->GetHitbox()))
            {
                bullet->Kill();
                break;
            }
        }

        for (GameObject* bullet : mBulletObjects)
        {
            for (GameObject* obstacle : mCollisionObjects)
            {
                if (bullet->GetHitbox().FindIntersection(obstacle->GetHitbox()))
                {                                        
                    bullet->Kill();
                    break;
                }
            }
        }

        // Enemy hit by bullet
        for (GameObject* bullet : mPlayerBulletObjects)
        {
            for (GameObject* obstacle : mVulnerableObjects)
            {
                if (bullet->GetHitbox().FindIntersection(obstacle->GetHitbox()))
                {
                    if (BitmaskCompare(static_cast<Bullet*>(bullet)->GetSprite(),
                                       bullet->GetInternaleTransformable(),
                                       static_cast<Entity*>(obstacle)->GetSprite(),
                                       obstacle->GetInternaleTransformable()))
                    {
                        bullet->Kill();
                        static_cast<Entity*>(obstacle)->Demage();
                    }
                }
            }
        }

        // Player hit by bullet
        for (GameObject* bullet : mEnemyBulletObjects)
        {         
            if (mPlayer->GetHitbox().FindIntersection(bullet->GetHitbox()))
            {                
                if (BitmaskCompare(static_cast<Bullet*>(bullet)->GetSprite(),
                                   bullet->GetInternaleTransformable(),
                                   mPlayer->GetSprite(),
                                   mPlayer->GetInternaleTransformable()))
                {
                    bullet->Kill();
                    mPlayer->Demage();
                    break;
                }                                
            }
        }
    }

    virtual bool Update(const sf::Time& timeslice) override
    {   
        for (GameObject* object : mPreUpdateGroup)
        {
            object->Update(timeslice);
        }

        mPlayer->Update(timeslice);

        for (GameObject* object : mPostUpdateGroup)
        {
            object->Update(timeslice);
        }

        BulletCollision();
                
        if (mPlayer->GetPosition().y > MAX_LEVEL_HEIGHT)
        {                        
            mPlayer->ResetPlayerPosition(mPlayerStartposition);
            mPlayer->Demage();
        }

        if (mPlayer->GetHealth() == 0)
        {
            GetLayerStack().PushLayer(std::make_unique<GameOver>(GetLayerStack(), *this, sf::Vector2u(mGameView.getSize())));
        }

        mGameView.setCenter(mPlayer->GetPosition());

        return true;
    }

    virtual bool Draw(sf::RenderWindow& window) override
    {
        window.setView(mGameView);               
        
        mBackground.Draw(window, ComputeVisibleRegion());

        TiledMapVisibleRegion tiledMapVisibleRegion(ComputeVisibleRegion(), mTiledMap.GetTileSize());

        for (uint32_t index = 0; index < mTiledMap.LayerCount(); index++)
        {
            mLayerRenderer->DrawLayer(window, index, tiledMapVisibleRegion);

            for (const GameObject* obj : mDrawGroup)
            {                                
                if (obj->GetDepth() == index)
                {
                    window.draw(*obj);                
                }
            }
        }

        window.setView(mHUDView);

        mOverlay->Draw(window);

        return true;
    }

    virtual void OnExit() 
    { 
        mMusic.stop(); 
        GameObjectManager::Instance().RemoveAllGameObjects();
    }

private:
    sf::FloatRect ComputeVisibleRegion()
    {
        sf::Vector2f topleft = mGameView.getCenter() - mGameView.getSize() / 2.0f;
        return { topleft, mGameView.getSize() };
    }
    
    sf::View mGameView;
    sf::View mHUDView;
    GameObjectManager& mManager;
    Group mPlayerBulletObjects;    
    Group mEnemyBulletObjects;
    Group mBulletObjects;
    Group mCollisionObjects;
    Group mVulnerableObjects;
    Group mDrawGroup;
    Group mPreUpdateGroup;
    Group mPostUpdateGroup;
    sf::Vector2f mPlayerStartposition;
    TiledMap mTiledMap;
    std::unique_ptr<TiledMapLayerRenderer> mLayerRenderer;
    std::vector<sf::FloatRect> mPlatformWayPoints;
    std::unique_ptr<TiledLayerSpatialQuery> mCollisionLayer;
    ParallaxBackground mBackground;
    Player* mPlayer;
    std::unique_ptr<Overlay> mOverlay;
    sf::Music& mMusic;
};

//------------------------------------------------------------------------------
int main()
{
    GameObjectManager& manager = GameObjectManager::Instance();
    
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(WINDOW_WIDTH, WINDOW_HEIGHT)), "Run-And-Gun");
    window.setVerticalSyncEnabled(true);
    
    sf::Clock clock;
    const sf::Time timePerFrame = sf::seconds(1.0f / 60.0f);
    sf::Time timeSinceLastUpdate = sf::Time::Zero;

    LayerStack layerStack;
    layerStack.PushLayer(std::make_unique<Game>(layerStack, manager, window.getSize()));

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                layerStack.Clear();
                window.close();
            }
            else if (event.type == sf::Event::Resized) 
            {        
                layerStack.Resize(sf::Vector2f(static_cast<float>(event.size.width), static_cast<float>(event.size.height)));                
            }
            else
            {
                layerStack.HandleEvent(event);
            }
        }

        timeSinceLastUpdate += clock.restart();
        while (timeSinceLastUpdate >= timePerFrame)
        {
            timeSinceLastUpdate -= timePerFrame;            
            layerStack.Update(timePerFrame);
            manager.SyncGameObjectChanges();
            
            window.clear({ 249, 131, 103 });
            layerStack.Draw(window);
            window.display();                       
        }
    }

    return 0;
}