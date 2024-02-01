#pragma once

// Includes
//------------------------------------------------------------------------------
// System
#include <unordered_set>
#include <vector>
#include <functional>
#include <algorithm>

// Forward declarations
//------------------------------------------------------------------------------
class GameObject;

//------------------------------------------------------------------------------
class Group
{
public:
    void DeferredAddGameObject(GameObject* obj);
    void DeferredRemoveGameObject(GameObject* obj);
    bool ContainsGameObject(GameObject* obj);
    void SyncGameObjectChanges();    
    void Sort(const std::function<bool(GameObject*, GameObject*)>& compareFunc);
    
    class Iterator
    {
    public:
        explicit Iterator(std::vector<GameObject*>::iterator it, Group* group)
            : mCurrent(it)
            , mGroup(group)
        { }

        Iterator& operator++()
        {
            ++mCurrent;
            SkipMarked();
            return *this;
        }

        GameObject* operator*()
        {
            return *mCurrent;
        }

        bool operator!=(const Iterator& other)
        {
            return mCurrent != other.mCurrent;
        }

    private:
        void SkipMarked()
        {
            while (mCurrent != mGroup->mSortedGameObjects.end() && mGroup->IsMarkedForRemoval(*mCurrent))
            {
                ++mCurrent;
            }
        }

        std::vector<GameObject*>::iterator mCurrent;
        Group* mGroup;
    };

    Iterator begin()
    {
        return Iterator(mSortedGameObjects.begin(), this);
    }

    Iterator end()
    {
        return Iterator(mSortedGameObjects.end(), this);
    }

private:
    bool IsMarkedForRemoval(GameObject* obj) const
    {
        return mRemovalQueue.find(obj) != mRemovalQueue.end();
    }

    std::vector<GameObject*> mSortedGameObjects;
    std::unordered_set<GameObject*> mGameObjects;
    std::unordered_set<GameObject*> mAddQueue;
    std::unordered_set<GameObject*> mRemovalQueue;
};