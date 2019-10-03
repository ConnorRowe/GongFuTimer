#pragma once

#include <iostream>
#include <chrono>
#include <ctime>
#include <cmath>

class Timer
{
private:
	std::chrono::time_point<std::chrono::system_clock>	StartTime;
	std::chrono::time_point<std::chrono::system_clock>	EndTime;

public:
	bool												isRunning = false;
	void start();
	void stop();
	void reset();
	double elapsedMiliseconds();
	double elapsedSeconds();

};