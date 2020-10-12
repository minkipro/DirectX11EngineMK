
#include "Timer.h"

Timer::Timer()
{
	_start = std::chrono::high_resolution_clock::now();
	_stop = std::chrono::high_resolution_clock::now();
}

double Timer::GetMilisecondsElapsed()
{
	if (_isrunning)
	{
		auto elapsed = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - _start);
		return elapsed.count();
	}
	else
	{
		auto elapsed = std::chrono::duration<double, std::milli>(_stop - _start);
		return elapsed.count();
	}
}

void Timer::Restart()
{
	_isrunning = true;
	_start = std::chrono::high_resolution_clock::now();
}

bool Timer::Stop()
{
	if (!_isrunning)
		return false;
	else
	{
		_stop = std::chrono::high_resolution_clock::now();
		_isrunning = false;
		return true;
	}
}

bool Timer::Start()
{
	if (_isrunning)
	{
		return false;
	}
	else
	{
		_start = std::chrono::high_resolution_clock::now();
		_isrunning = true;
		return true;
	}
}