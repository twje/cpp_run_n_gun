#include "EventQueue.h"
#include "GameObject.h"

// Static definitions
//------------------------------------------------------------------------------
EventQueue* EventQueue::sInstance = nullptr;

//------------------------------------------------------------------------------
void EventQueue::QueueEvent(std::unique_ptr<Event> event)
{
    mQueue.push_back(std::move(event));
}

//------------------------------------------------------------------------------
void EventQueue::DispatchEvents(GameObject* object)
{
    for (auto& event : mQueue) 
    {
        object->HandleEvent(event.get());
    }
}

//------------------------------------------------------------------------------
void EventQueue::Clear()
{
    mQueue.clear();
}