#include "Clock.h"

/// <summary>
/// Constructor for the Clock class.
/// </summary>
Clock::Clock()
{
	m_startTime = SDL_GetTicks();
	m_creationTime = SDL_GetTicks();
}

/// <summary>
/// Destructor for the Clock class.
/// </summary>
Clock::~Clock()
{

}

/// <summary>
/// Get elapsed time.
/// </summary>
/// <returns>Uint64 - The time elapsed since the last call to restart().</returns>
Uint64 Clock::getElapsedTime() const
{
	return SDL_GetTicks() - m_startTime;
}

/// <summary>
/// Get total time.
/// </summary>
/// <returns>Uint64 - The time since the Clock object was created.</returns>
Uint64 Clock::getTotalTime() const
{
	return SDL_GetTicks() - m_creationTime;
}

/// <summary>
/// Restart time counter to zero.
/// </summary>
/// <returns>Uint64 - The time elapsed since the clock was restarted (or created if restart() hasn't been called yet).</returns>
Uint64 Clock::restart()
{
	Uint64 f_timeElapsed = SDL_GetTicks() - m_startTime;
	m_startTime = SDL_GetTicks();
	return f_timeElapsed;
}
