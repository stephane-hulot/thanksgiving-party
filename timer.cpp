#include <iostream>
#include "timer.h"

Timer::Timer() : start_time(0), time(0)
{

}

void Timer::start()
{
	start_time = SDL_GetTicks();
}

void Timer::stop()
{
	time = SDL_GetTicks() - start_time;
}

uint Timer::get_time()
{
	uint res = 0;
	if(time == 0) //timer not stopped yet
		res = SDL_GetTicks() - start_time;
	else
		res = time;
	return res;
}

std::string Timer::get_time_string()
{
	uint t = get_time();
	uint min = t / 60000;
	uint sec = (t / 1000) % 60;
	uint ms = t % 1000;
	std::string res = (min < 10 ? "0" : "") + std::to_string(min) + ":";
	res += (sec < 10 ? "0" : "") + std::to_string(sec);
	res += "." + std::to_string(ms);
	return res;
}

/*
Timer& Timer::operator=(Timer t)
{
    return *this;
}
*/