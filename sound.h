#ifndef _SOUND_H_
#define _SOUND_H_

#include <SDL2/SDL_mixer.h>
#include "menu.h"
#include "player.h"
#include "map.h"
#include <SDL2/SDL.h>


class Sound
{
	public:
		Sound(Menu* me, Player* p, Map* ma);
		void init_sounds();
		void play_sounds();
		~Sound();

		//forbids copy constructor to avoid warning in c++11
		Sound(const Sound& s) = delete;
        Sound& operator=(const Sound& s) = delete;
		
	private:

		//SOUNDS----------
		std::string themestring = "sounds/theme.ogg";
		std::string gunshotstring = "sounds/gunshot.ogg";
		std::string menustring = "sounds/menu.ogg";
		std::string pausestring = "sounds/pause.ogg";
		std::string overstring = "sounds/gameover.ogg";
		std::string winstring = "sounds/victory.ogg";
		std::string wallstring = "sounds/wall.ogg";
		std::string deathstring = "sounds/death_turkey.ogg";
		std::string keystring = "sounds/key.ogg";
		std::string turkstring = "sounds/turkey.ogg";

		char* theme = (char*)themestring.c_str();
		char* gunshot = (char*)gunshotstring.c_str();
		char* music_menu = (char*)menustring.c_str();
		char* music_pause = (char*)pausestring.c_str();
		char* gameover = (char*)overstring.c_str();
		char* victory = (char*)winstring.c_str();
		char* wall = (char*)wallstring.c_str();
		char* death_turk = (char*)deathstring.c_str();
		char* key = (char*)keystring.c_str();
		char* turkey = (char*)turkstring.c_str();
		//----------------

		Mix_Chunk* get_sound(char filename[]);
        void pause_music(int channel);
        void resume_music(int channel);
		void play(char filename[], int channel, int loops);

		Menu* menu;
		Player* player;
		Map* map;
};

#endif