#ifndef _SOUND_H_
#define _SOUND_H_

#include <SDL2/SDL_mixer.h>
#include "menu.h"
#include "player.h"
#include <SDL2/SDL.h>


class Sound
{
	public:
		Sound(Menu* me, Player* p);
		void init_sounds();
		void play_sounds();
		void set_volume(int vol);
		~Sound();

		//forbids copy constructor to avoid warning in c++11
		Sound(const Sound& s) = delete;
        Sound& operator=(const Sound& s) = delete;
		
	private:
		Mix_Chunk* theme;
		Mix_Chunk* music_menu;
		Mix_Chunk* music_pause;
		Mix_Chunk* gameover;
		Mix_Chunk* victory;
		Mix_Chunk* gunshot;
		Mix_Chunk* death_turkey;
		Mix_Chunk* key;
		Mix_Chunk* turkey;
		Mix_Chunk* wall;
		Mix_Chunk* hurt;

		Mix_Chunk* load_sound(std::string filename);
        void pause_music(int channel);
        void resume_music(int channel);
		void play(Mix_Chunk* sound, int channel, int loops);

		Menu* menu;
		Player* player;
};

#endif