// Includes
//------------------------------------------------------------------------------
#include "GameObjectManager.h"

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
        (*it)->SyncGameObjectChanges();

        if ((*it)->IsKilled())
        {
            it = mGameObjects.erase(it);
        }
        else
        {
            ++it;
        }
    }
}