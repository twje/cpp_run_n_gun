// Includes
//------------------------------------------------------------------------------
#include "GameObjectManager.h"

// Core
#include "Core/Events.h"

//------------------------------------------------------------------------------
GameObjectManager::GameObjectManager()
{
    EventQueue::CreateInstance();
}

//------------------------------------------------------------------------------
/*static*/ GameObjectManager& GameObjectManager::Instance()
{
    static GameObjectManager manager;
    return manager;
}

//------------------------------------------------------------------------------
void GameObjectManager::SyncGameObjectChanges()
{
    for (auto it = mGameObjects.begin(); it != mGameObjects.end(); )
    {
        if ((*it)->IsMarkedForRemoval())
        {
            EventQueue::Instance()->QueueEvent(std::make_unique<EntityRemovedFromSceneEvent>((*it)->GetEntityId()));
            mGameObjectLookup.erase((*it)->GetEntityId());
            it = mGameObjects.erase(it);
        }
        else
        {
            EventQueue::Instance()->DispatchEvents(it->get());
            ++it;
        }
    }
    EventQueue::Instance()->Clear();
}

//------------------------------------------------------------------------------
GameObject* GameObjectManager::GetInstance(uint32_t entityId)
{
    auto it = mGameObjectLookup.find(entityId);
    if (it != mGameObjectLookup.end())
    {
        return it->second;
    }
    return nullptr;
}

//------------------------------------------------------------------------------
void GameObjectManager::RemoveAllGameObjects()
{
    for (const std::unique_ptr<GameObject>& object : mGameObjects)
    {
        if (!object->IsMarkedForRemoval())
        {
            object->Kill();
        }
    }
    SyncGameObjectChanges();
}