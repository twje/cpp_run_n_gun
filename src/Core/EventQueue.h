#pragma once

// Includes
//------------------------------------------------------------------------------
// System
#include <vector>
#include <memory>
#include <functional>
#include <cassert>

// Forward Declarations
//------------------------------------------------------------------------------
class GameObject;

//------------------------------------------------------------------------------
struct Event 
{
    virtual ~Event() = default;
    Event(uint32_t eventType, uint32_t senderId)
        : mEventType(eventType)
        , mSenderId(senderId)
    { }

    template<typename EventType>
    bool IsType(EventType type) const
    {
        return mEventType == static_cast<uint32_t>(type);
    }

    bool IsFromSender(uint32_t senderId) const 
    {
        return mSenderId == senderId;
    }

private:
    uint32_t mEventType;
    uint32_t mSenderId;
};

//------------------------------------------------------------------------------
class EventQueue
{
    static EventQueue* sInstance;

public:
    EventQueue(const EventQueue&) = delete;
    EventQueue& operator=(const EventQueue&) = delete;
    EventQueue(EventQueue&&) = delete;
    EventQueue& operator=(EventQueue&&) = delete;

    static EventQueue* CreateInstance() 
    {
        assert(sInstance == nullptr);        
        sInstance = new EventQueue();
        return sInstance;
    }

    static EventQueue* Instance() 
    {
        assert(sInstance != nullptr);
        return sInstance;
    }

    void QueueEvent(std::unique_ptr<Event> event);
    void DispatchEvents(GameObject* object);
    void Clear();

private:
    EventQueue() = default;
    
    std::vector<std::unique_ptr<Event>> mQueue;
};
