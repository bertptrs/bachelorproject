#ifndef TIMER_H
#define TIMER_H

#include <chrono>

using namespace std::chrono;

template<typename ClockType = high_resolution_clock, typename DurationType = typename ClockType::time_point::duration>
class Timer
{
	private:
		typename ClockType::time_point lastTime;

	public:
		// Create the timer with the current time.
		Timer()
		{
			set();
		}

		// Set the timer to the current time.
		void set()
		{
			lastTime = ClockType::now();
		}

		DurationType get() const
		{
			return ClockType::now() - lastTime;
		}
};

#endif
