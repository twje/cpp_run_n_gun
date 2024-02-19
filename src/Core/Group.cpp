// Includes
//------------------------------------------------------------------------------
// Core
#include "Group.h"
#include "GameObjectManager.h"

//------------------------------------------------------------------------------
GroupIterator::GroupIterator(std::vector<GameObject*>::iterator it, Group* group)
    : mCurrent(it)
    , mGroup(group)
{
    ++mGroup->mIterationCounter;
    SkipMarked();
}

//------------------------------------------------------------------------------
GroupIterator::~GroupIterator()
{
    if (--mGroup->mIterationCounter == 0)
    {
        mGroup->ProcessQueues();
    }
}

//------------------------------------------------------------------------------
GroupIterator& GroupIterator::operator++()
{
    ++mCurrent;
    SkipMarked();
    return *this;
}

//------------------------------------------------------------------------------
GameObject* GroupIterator::operator*()
{
    return *mCurrent;
}

//------------------------------------------------------------------------------
bool GroupIterator::operator!=(const GroupIterator& other)
{
    return mCurrent != other.mCurrent;
}

//------------------------------------------------------------------------------
void GroupIterator::SkipMarked()
{
    while (mCurrent != mGroup->mSortedGameObjects.end() && mGroup->mRemoveQueue.find(*mCurrent) != mGroup->mRemoveQueue.end())
    {
        ++mCurrent;
    }
}

//------------------------------------------------------------------------------
void Group::AddGameObject(GameObject* obj)
{
    if (!obj->IsMarkedForRemoval() && mAddQueue.find(obj) == mAddQueue.end())
    {
        obj->TrackGroupMembership(this);

        mAddQueue.insert(obj);
        mRemoveQueue.erase(obj);
        ProcessQueues();
    }
}

//------------------------------------------------------------------------------
void Group::RemoveGameObject(GameObject* obj)
{
    if (mRemoveQueue.find(obj) == mRemoveQueue.end())
    {

        obj->UntrackGroupMembership(this);

        mRemoveQueue.insert(obj);
        mAddQueue.erase(obj);
        ProcessQueues();
    }
}

//------------------------------------------------------------------------------
void Group::Sort(const std::function<bool(GameObject*, GameObject*)>& compareFunc)
{
    std::sort(mSortedGameObjects.begin(), mSortedGameObjects.end(), compareFunc);
}