#pragma once

// Includes
//------------------------------------------------------------------------------
// Core
#include "GameObject.h"

//------------------------------------------------------------------------------
class GameObjectManager
{
public:
    GameObjectManager(const GameObjectManager&) = delete;             // Copy constructor
    GameObjectManager& operator=(const GameObjectManager&) = delete;  // Copy assignment operator
    GameObjectManager(GameObjectManager&&) = delete;                  // Move constructor
    GameObjectManager& operator=(GameObjectManager&&) = delete;       // Move assignment operator

    static GameObjectManager& Instance();

    template<typename T, typename... Args>
    T* CreateGameObject(Args&&... args)
    {
        auto gameObject = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = gameObject.get();
        mGameObjects.insert(std::move(gameObject));
        return ptr;
    }

    void SyncGameObjectChanges();

private:
    GameObjectManager() = default;

    std::unordered_set<std::unique_ptr<GameObject>> mGameObjects;
};