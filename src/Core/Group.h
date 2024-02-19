#pragma once

// Includes
//------------------------------------------------------------------------------
// System
#include <unordered_set>
#include <vector>
#include <functional>
#include <algorithm>
#include <cstdint>

// Forward declarations
//------------------------------------------------------------------------------
class GameObject;
class Group;

//------------------------------------------------------------------------------
class GroupIterator
{
public:
    explicit GroupIterator(std::vector<GameObject*>::iterator it, Group* group);
    ~GroupIterator();

    GroupIterator& operator++();
    GameObject* operator*();
    bool operator!=(const GroupIterator& other);

private:
    void SkipMarked();

    std::vector<GameObject*>::iterator mCurrent;
    Group* mGroup;
};

//------------------------------------------------------------------------------
class Group
{
    friend GroupIterator;

public:
    void AddGameObject(GameObject* obj);
    void RemoveGameObject(GameObject* obj);
    void Sort(const std::function<bool(GameObject*, GameObject*)>& compareFunc);    

    GroupIterator begin()
    {
        return GroupIterator(mSortedGameObjects.begin(), this);
    }

    GroupIterator end()
    {
        return GroupIterator(mSortedGameObjects.end(), this);
    }

private:
    void ProcessQueues()
    {
        if (mIterationCounter == 0)
        {
            for (GameObject* obj : mAddQueue)
            {
                mSortedGameObjects.push_back(obj);
            }
            mAddQueue.clear();

            for (GameObject* obj : mRemoveQueue)
            {
                auto it = std::find(mSortedGameObjects.begin(), mSortedGameObjects.end(), obj);
                if (it != mSortedGameObjects.end())
                {
                    mSortedGameObjects.erase(it);
                }
            }
            mRemoveQueue.clear();
        }
    }
    
    std::vector<GameObject*> mSortedGameObjects;
    std::unordered_set<GameObject*> mRemoveQueue;
    std::unordered_set<GameObject*> mAddQueue;
    uint32_t mIterationCounter{ 0 };
};