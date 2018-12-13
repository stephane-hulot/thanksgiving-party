#ifndef _LEADERBOARD_H_
#define _LEADERBOARD_H_
#include <fstream>
#include <SDL2/SDL.h>

const std::string scores_file("scores.txt");

class Leaderboard
{
	public:
		Leaderboard(); //create a leadeboard by reading the file
		~Leaderboard(); //free memory and save scores to disk

		//forbids copy constructor to avoid warning in c++11
		Leaderboard(const Leaderboard& l) = delete;
        Leaderboard& operator=(const Leaderboard& l) = delete;

		void add_score(int score); //adds a score to the array and saves to disk

		int* scores;

	private:
		void read_file(); //loads scores from file
		void write_file(); //saves score to disk
		void sort_scores(); //sorts the score array

};

#endif