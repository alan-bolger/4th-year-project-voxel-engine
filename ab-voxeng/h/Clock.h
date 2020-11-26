#ifndef CLOCK_H
#define CLOCK_H

#include <SDL.h>

class Clock
{
public:
	Clock();
	~Clock();
	Uint64 getElapsedTime() const;
	Uint64 getTotalTime() const;
	Uint64 restart();

private:
	Uint64 m_creationTime;
	Uint64 m_startTime;
};

#endif // !CLOCK_H

