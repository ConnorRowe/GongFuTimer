//
// Timer.cpp
// Implementation of the Timer class.
//


#include "pch.h"
#include "Timer.h"

void Timer::start()
{
	StartTime = std::chrono::system_clock::now();
	isRunning = true;
}

void Timer::stop()
{
	EndTime = std::chrono::system_clock::now();
	isRunning = false;
}

double Timer::elapsedMiliseconds()
{
	std::chrono::time_point<std::chrono::system_clock> endTime;

	if (isRunning)
	{
		endTime = std::chrono::system_clock::now();
	}
	else
	{
		endTime = EndTime;
	}

	return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - StartTime).count();
}

double Timer::elapsedSeconds()
{
	return elapsedMiliseconds() / 1000.0;
}

void Timer::reset()
{
	start();
	stop();
}