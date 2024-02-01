// Includes
//------------------------------------------------------------------------------
// Third party
#include <SFML/Graphics.hpp>

// Core
#include "Core/GameObjectManager.h"
#include "Core/Resources.h"
#include "Core/RectUtils.h"
#include "Core/RandomUtils.h"
#include "Core/DrawUtils.h"

//------------------------------------------------------------------------------
constexpr uint32_t WINDOW_WIDTH = 800;
constexpr uint32_t WINDOW_HEIGHT = 600;

//------------------------------------------------------------------------------
namespace Resources
{    
    constexpr char SFMLLogo[] = "1200px-SFML2.png";
}

//------------------------------------------------------------------------------
class SMFLLogo : public GameObject
{
public:
    SMFLLogo(const sf::Vector2f& position)
        : mSprite(LoadTexture(Resources::SFMLLogo))
        , mSpeed(200.0f)
    {               
        SetOrigin(GetRectCenter(mSprite.getLocalBounds()));
        SetScale({ 0.25f, 0.25f });
        SetPosition(position);        

        mHitbox = InflateRect(GetGlobalBounds(), -50, -40);        

        float dirX = GetRandomIntegerFromList({ -1.0f, 1.0f });
        float dirY = GetRandomIntegerFromList({ -1.0f, 1.0f });
        mDirection = { dirX, dirY };
    }

    virtual sf::FloatRect GetGlobalBounds() const override
    {
        return GetTransform().transformRect(mSprite.getLocalBounds());
    }

    virtual sf::FloatRect GetHitbox() const override
    {
        return mHitbox;
    }

    virtual void Update(const sf::Time& timeslice) override
    {                
        sf::Vector2f delta = mDirection * mSpeed * timeslice.asSeconds();
        MoveRect(mHitbox, delta);

        if (mHitbox.left < 0)
        {
            mDirection.x = -mDirection.x;
        }
        else if (mHitbox.left + mHitbox.width > WINDOW_WIDTH)
        {
            mDirection.x = -mDirection.x;
        }
        else if (mHitbox.top < 0)
        {
            mDirection.y = -mDirection.y;
        }
        else if (mHitbox.top + mHitbox.height > WINDOW_HEIGHT)
        {
            mDirection.y = -mDirection.y;
        }

        SyncPositionWithHitbox();
    }

    virtual void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override
    {
        sf::RenderStates statesCopy(states);
        statesCopy.transform *= GetTransform();
        target.draw(mSprite, statesCopy);
    }

    virtual void SetPosition(const sf::Vector2f& position) override
    {
        sf::Transformable& transformable = GetInternaleTransformable();
        transformable.setPosition(position);
        mHitbox.left = position.x - mHitbox.width / 2.0f;
        mHitbox.top = position.y - mHitbox.height / 2.0f;
    }

private:
    void SyncPositionWithHitbox()
    {
        sf::Transformable& transformable = GetInternaleTransformable();
        sf::Vector2f center = GetRectCenter(mHitbox);
        transformable.setPosition(center);
    }

    sf::Sprite mSprite;
    sf::FloatRect mHitbox;
    sf::Vector2f mDirection;
    float mSpeed;
};

//------------------------------------------------------------------------------
int main()
{
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(WINDOW_WIDTH, WINDOW_HEIGHT)), "SFML Template Project");
    window.setVerticalSyncEnabled(true);

    sf::Clock clock;
    const sf::Time timePerFrame = sf::seconds(1.0f / 60.0f);
    sf::Time timeSinceLastUpdate = sf::Time::Zero;

    GameObjectManager& manager = GameObjectManager::Instance();
    Group allSprites;

    SMFLLogo* logo = manager.CreateGameObject<SMFLLogo>(sf::Vector2f(window.getSize()) / 2.0f);
    logo->AddToGroup(&allSprites);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        timeSinceLastUpdate += clock.restart();
        while (timeSinceLastUpdate >= timePerFrame)
        {
            timeSinceLastUpdate -= timePerFrame;
            for (GameObject* obj : allSprites)
            {                
                obj->Update(timePerFrame);
            }
            manager.SyncGameObjectChanges();
        }

        window.clear();
        for (const GameObject* obj : allSprites)
        {
            window.draw(*obj);
            DrawRect(window, obj->GetGlobalBounds(), sf::Color::Red);
            DrawRect(window, obj->GetHitbox(), sf::Color::Blue);
        }        
        window.display();
    }

    return 0;
}