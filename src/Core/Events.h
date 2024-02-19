#pragma once

// Includes
//------------------------------------------------------------------------------
// Core
#include <Core/EventQueue.h>

//------------------------------------------------------------------------------
enum class EntityCoreEventType : uint32_t
{
    ENTITY_REMOVE_FROM_SCENE = 0,
    LAST_CORE_EVENT
};

//------------------------------------------------------------------------------
struct EntityRemovedFromSceneEvent : public Event 
{
    EntityRemovedFromSceneEvent(uint32_t senderId)
        : Event(static_cast<uint32_t>(EntityCoreEventType::ENTITY_REMOVE_FROM_SCENE), senderId)
    { }
};