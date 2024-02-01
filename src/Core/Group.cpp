// Includes
//------------------------------------------------------------------------------
// Core
#include "Group.h"

//------------------------------------------------------------------------------
void Group::DeferredAddGameObject(GameObject* obj)
{
    auto it = mRemovalQueue.find(obj);
    if (it != mRemovalQueue.end())
    {
        mRemovalQueue.erase(it);
    }

    if (mAddQueue.find(obj) == mAddQueue.end())
    {
        if (mSortedGameObjects.empty())
        {
            // Immediate addition is safe as the group is not currently undergoing iteration.
            mSortedGameObjects.push_back(obj);
            mGameObjects.insert(obj);
        }
        else
        {
            mAddQueue.insert(obj);
        }
    }
}

//------------------------------------------------------------------------------
void Group::DeferredRemoveGameObject(GameObject* obj)
{
    auto it = mAddQueue.find(obj);
    if (it != mAddQueue.end())
    {
        mAddQueue.erase(it);
    }

    if (mRemovalQueue.find(obj) == mRemovalQueue.end())
    {
        mRemovalQueue.insert(obj);
    }
}

//------------------------------------------------------------------------------
bool Group::ContainsGameObject(GameObject* obj)
{
    return mGameObjects.find(obj) != mGameObjects.end();
}

//------------------------------------------------------------------------------
void Group::SyncGameObjectChanges()
{
    for (GameObject* obj : mRemovalQueue)
    {
        auto it = std::find(mSortedGameObjects.begin(), mSortedGameObjects.end(), obj);
        if (it != mSortedGameObjects.end())
        {
            mSortedGameObjects.erase(it);
            mGameObjects.erase(obj);  // Keep mGameObjects in sync
        }
    }
    mRemovalQueue.clear();

    for (GameObject* obj : mAddQueue)
    {
        if (mGameObjects.find(obj) == mGameObjects.end())
        {
            mSortedGameObjects.push_back(obj);
            mGameObjects.insert(obj);  // Keep mGameObjects in sync
        }
    }
    mAddQueue.clear();
}

//------------------------------------------------------------------------------
void Group::Sort(const std::function<bool(GameObject*, GameObject*)>& compareFunc)
{
    std::sort(mSortedGameObjects.begin(), mSortedGameObjects.end(), compareFunc);
}