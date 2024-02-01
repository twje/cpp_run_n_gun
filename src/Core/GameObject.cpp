// Includes
//------------------------------------------------------------------------------
#include "GameObject.h"

// Game
#include "Group.h"

// Third party
#include <SFML/Graphics.hpp>

//------------------------------------------------------------------------------
void GameObject::AddToGroup(Group* group)
{
    assert(!mIsKilled);
    mGroups.insert(group);
    group->DeferredAddGameObject(this);
}

//------------------------------------------------------------------------------
void GameObject::RemoveFromGroup(Group* group)
{
    assert(!mIsKilled);
    group->DeferredAddGameObject(this);
}

//------------------------------------------------------------------------------
void GameObject::Kill()
{
    for (auto group : mGroups)
    {
        group->DeferredRemoveGameObject(this);
    }
    mIsKilled = true;
}

//------------------------------------------------------------------------------
void GameObject::SyncGameObjectChanges()
{
    for (auto group : mGroups)
    {
        group->SyncGameObjectChanges();
    }

    for (auto it = mGroups.begin(); it != mGroups.end(); )
    {
        if (!(*it)->ContainsGameObject(this))
        {
            it = mGroups.erase(it);
        }
        else
        {
            ++it;
        }
    }
}