#include "Timer.h"

Timer::Timer() : start(clock_::now()) {}
Timer::Timer(std::chrono::time_point<clock_>& tp) : start(tp) {}

void Timer::ResetTimer()
{
	start = clock_::now();
}

void Timer::SetStart(std::chrono::time_point<clock_>& tp)
{
	start = tp;
}

int64_t Timer::NanoCount() const
{
	return std::chrono::duration_cast<std::chrono::nanoseconds>(clock_::now() - start).count();
}

int64_t Timer::MicroCount() const
{
	return std::chrono::duration_cast<std::chrono::microseconds>(clock_::now() - start).count();
}

double Timer::MilliCount() const
{
	return (double)MicroCount() * 1e-3;
}

double Timer::SecsCount() const
{
	return (double)MicroCount() * 1e-6;
}
