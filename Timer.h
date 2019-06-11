#pragma once
#include <ctime>
#include <chrono>

#define SECOND 1000000000

typedef std::chrono::high_resolution_clock clock_;

class Timer
{
public:
	Timer();
	void ResetTimer();
	int64_t NanoCount() const;
	int64_t MicroCount() const;
	double MilliCount() const;
private:
	std::chrono::time_point<clock_> start;
};
