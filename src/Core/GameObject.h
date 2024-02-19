#pragma once

// Includes
//------------------------------------------------------------------------------
// Game
#include "Group.h"

// Third party
#include <SFML/Graphics.hpp>

// Core
#include "Transformable.h"
#include "FloatRect.h"
#include "EventQueue.h"

//------------------------------------------------------------------------------
class GameObject : public sf::Drawable, public Tranformable
{
public:
    virtual ~GameObject() = default;

    virtual FloatRect GetGlobalBounds() const = 0;
    virtual uint32_t GetDepth() const { return 0; }
    virtual void Update(const sf::Time& timeslice) { };
    virtual void draw(sf::RenderTarget& target, const sf::RenderStates& states) const { }

    // Collision detection
    virtual FloatRect GetHitbox() const { return GetGlobalBounds(); }
    virtual FloatRect GetPreviousHitbox() const { return GetHitbox(); }    
    virtual const sf::Vector2f GetVelocity() const { return { }; };    
    bool IsDownCollision(const GameObject& other) const;    
    bool IsUpCollision(const GameObject& other) const;
    bool IsLeftCollision(const GameObject& other) const;        
    bool IsRightCollision(const GameObject& other) const;

    // Group Membership
    void Kill();
    void TrackGroupMembership(Group* group);
    void UntrackGroupMembership(Group* group);
    bool IsMarkedForRemoval() const { return mIsMarkedForRemoval; }
    
    // Event Handling
    void SetEntityId(uint32_t entityId) { mEntityId = entityId; }
    uint32_t GetEntityId() const { return mEntityId; }
    virtual void HandleEvent(Event* event) { };

private:    
    std::unordered_set<Group*> mTrackedGroups;
    bool mIsMarkedForRemoval = false;
    uint32_t mEntityId = 0;
};