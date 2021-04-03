#pragma once
#include <ctime>
#include <chrono>

#define SECOND 1000000000

#ifdef _WIN32
    #define timegm _mkgmtime
#endif

typedef std::chrono::high_resolution_clock clock_;

class Timer
{
public:
	Timer();
	Timer(std::chrono::time_point<clock_>& tp);
	void SetStart(std::chrono::time_point<clock_>& tp);
	void ResetTimer();
	int64_t NanoCount() const;
	int64_t MicroCount() const;
	double MilliCount() const;
	double SecsCount() const;
private:
	std::chrono::time_point<clock_> start;
};

namespace Clock {

    using namespace std::chrono;

    static inline double Timestamp()
    {
        static struct tm epoch = {0,0,0,1,0,70,0,0,0};
        return difftime(time(NULL), timegm(&epoch));
    }
};
