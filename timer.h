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

        //overloaded function
        std::string get_time_string(); //returns the current time to string
        std::string get_time_string(uint t); //returns specified time to string

        //forbids copy constructor to avoid warning in c++11
        Timer(const Timer& t) = delete;
        Timer& operator=(const Timer& t) = delete;
    private:
    	uint start_time;
    	uint time;
};

#endif