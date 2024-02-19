#pragma once

// Includes
//------------------------------------------------------------------------------
// Core
#include "GameObject.h"
#include "EventQueue.h"

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
        gameObject->SetEntityId(++mEntityIdCounter);
        
        T* ptr = gameObject.get();
        mGameObjectLookup[mEntityIdCounter] = ptr;                     
        mGameObjects.insert(std::move(gameObject));

        return ptr;
    }

    void SyncGameObjectChanges();
    GameObject* GetInstance(uint32_t entityId);
    void RemoveAllGameObjects();

private:
    GameObjectManager();

    std::unordered_map<uint32_t, GameObject*> mGameObjectLookup;
    std::unordered_set<std::unique_ptr<GameObject>> mGameObjects;    
    uint32_t mEntityIdCounter = 0;
};