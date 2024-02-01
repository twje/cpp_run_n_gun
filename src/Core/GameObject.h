#pragma once

// Includes
//------------------------------------------------------------------------------
// Game
#include "Group.h"

// Third party
#include <SFML/Graphics.hpp>

// Core
#include "Transformable.h"

//------------------------------------------------------------------------------
class GameObject : public sf::Drawable, public Tranformable
{
public:
    virtual ~GameObject() = default;
    virtual sf::FloatRect GetGlobalBounds() const = 0;
    virtual sf::FloatRect GetHitbox() const { return GetGlobalBounds(); }
    virtual void Update(const sf::Time& timeslice) { };
    virtual void draw(sf::RenderTarget& target, const sf::RenderStates& states) const { }

    void AddToGroup(Group* group);
    void RemoveFromGroup(Group* group);
    void Kill();
    void SyncGameObjectChanges();
    bool IsKilled() { return mIsKilled; }

private:
    std::unordered_set<Group*> mGroups;
    bool mIsKilled = false;
};