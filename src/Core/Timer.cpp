// Includes
//------------------------------------------------------------------------------
#include "Timer.h"

//------------------------------------------------------------------------------
Timer::Timer(const sf::Time& duration)
	: mDuration(duration)
{ }

//------------------------------------------------------------------------------
void Timer::Update(const sf::Time& timestamp)
{
	if (mActive)
	{
		mElapsedTime += timestamp.asSeconds();
		if (IsFinished())
		{
			mActive = false;
			mElapsedTime = mDuration.asSeconds();
		}
	}
}

//------------------------------------------------------------------------------
void Timer::Start()
{
	mActive = true;
}

//------------------------------------------------------------------------------
void Timer::Finish()
{
	mElapsedTime = mDuration.asSeconds();
}

//------------------------------------------------------------------------------
void Timer::Reset(bool play)
{
	mElapsedTime = 0;
	mActive = play;
}

//------------------------------------------------------------------------------
bool Timer::IsFinished()
{
	return mElapsedTime >= mDuration.asSeconds();
}

//------------------------------------------------------------------------------
float Timer::PercentComplete()
{
	return mElapsedTime / mDuration.asSeconds();
}

//------------------------------------------------------------------------------
void Timer::SetDuration(const sf::Time& duration) 
{ 
	mDuration = duration; 
}