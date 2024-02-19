#include "GameObject.h"

// Includes
//------------------------------------------------------------------------------
// Third party
#include <SFML/Graphics.hpp>

// Core
#include "Group.h"

//------------------------------------------------------------------------------
void GameObject::Kill()
{
    mIsMarkedForRemoval = true;
    for (auto group : mTrackedGroups)
    {
        group->RemoveGameObject(this);
    }
    mTrackedGroups.clear();
}

//------------------------------------------------------------------------------
void GameObject::TrackGroupMembership(Group* group)
{
    mTrackedGroups.insert(group);
}

//------------------------------------------------------------------------------
void GameObject::UntrackGroupMembership(Group* group)
{
    if (!mIsMarkedForRemoval)
    {
        mTrackedGroups.erase(group);
    }
}

//------------------------------------------------------------------------------
bool GameObject::IsDownCollision(const GameObject& other) const
{
    FloatRect hitbox0 = GetHitbox();
    FloatRect previousHitbox0 = GetPreviousHitbox();
    FloatRect hitbox1 = other.GetHitbox();
    FloatRect previousHitbox1 = other.GetPreviousHitbox();

    return hitbox0.GetBottom() >= hitbox1.GetTop() && previousHitbox0.GetBottom() <= previousHitbox1.GetTop();
}

//------------------------------------------------------------------------------
bool GameObject::IsUpCollision(const GameObject& other) const
{
    FloatRect hitbox0 = GetHitbox();
    FloatRect previousHitbox0 = GetPreviousHitbox();
    FloatRect hitbox1 = other.GetHitbox();
    FloatRect previousHitbox1 = other.GetPreviousHitbox();

    return hitbox0.GetTop() <= hitbox1.GetBottom() && previousHitbox0.GetTop() >= previousHitbox1.GetBottom();
}

//------------------------------------------------------------------------------
bool GameObject::IsLeftCollision(const GameObject& other) const
{
    FloatRect hitbox0 = GetHitbox();
    FloatRect previousHitbox0 = GetPreviousHitbox();
    FloatRect hitbox1 = other.GetHitbox();
    FloatRect previousHitbox1 = other.GetPreviousHitbox();

    return hitbox0.GetLeft() <= hitbox1.GetRight() && previousHitbox0.GetLeft() >= previousHitbox1.GetRight();
}

//------------------------------------------------------------------------------
bool GameObject::IsRightCollision(const GameObject& other) const
{
    FloatRect hitbox0 = GetHitbox();
    FloatRect previousHitbox0 = GetPreviousHitbox();
    FloatRect hitbox1 = other.GetHitbox();
    FloatRect previousHitbox1 = other.GetPreviousHitbox();

    return hitbox0.GetRight() >= hitbox1.GetLeft() && previousHitbox0.GetRight() <= previousHitbox1.GetLeft();
}