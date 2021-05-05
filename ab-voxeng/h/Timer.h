/*
	Created by Paul O'Callaghan
*/
#ifndef TIMER_H
#define TIMER_H

#include <iostream>
#include <chrono>
#include <vector>

class Timer
{
public:
	Timer(std::string s);
	~Timer();

	void Start();
	void Stop();

	void SetSlowest(std::chrono::microseconds time);
	void SetFastest(std::chrono::microseconds time);
	void SetAverage();
	std::string name = "";
private:

	std::chrono::time_point<std::chrono::system_clock> start;
	std::chrono::time_point<std::chrono::system_clock> end;

	std::chrono::microseconds fastest;
	std::chrono::microseconds slowest;
	long long average;

	std::vector<std::chrono::microseconds> times = std::vector<std::chrono::microseconds>();

	bool firstFastestSet = false;
	bool firstSlowestSet = false;
};

#endif // TIMER_H
