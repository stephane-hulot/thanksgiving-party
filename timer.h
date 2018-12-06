#ifndef _TIMER_H_
#define _TIMER_H_

#include <SDL2/SDL.h>

class Timer
{
	public:
        Timer();
        void start();
        void stop();
        uint get_time();
        std::string get_time_string();

        //copy constructor to avoid warning in c++11
        //Timer(const Timer& t) : Timer() {};
        //Timer& operator=(const Timer& t);
    private:
    	uint start_time;
    	uint time;
};

#endif