#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL_mixer.h>
#include "sound.h"

Sound::Sound(Menu* me, Player* p, Map* ma) : theme(NULL), music_menu(NULL), music_pause(NULL), gameover(NULL), victory(NULL),
	gunshot(NULL), death_turkey(NULL), key(NULL), turkey(NULL), wall(NULL), hurt(NULL), menu(me), player(p), map(ma)
{
	
}

void Sound::init_sounds()
{
	std::cout<<"Loading sounds ..."<<std::endl;
	//Initialize SDL_mixer 
	Mix_Init(MIX_INIT_OGG);		
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);

	theme = load_sound("sounds/theme.ogg");
	music_menu  = load_sound("sounds/menu.ogg");
	music_pause = load_sound("sounds/pause.ogg");
	gameover = load_sound("sounds/gameover.ogg");
	victory = load_sound("sounds/victory.ogg");
	gunshot = load_sound("sounds/gunshot.ogg");
	death_turkey = load_sound("sounds/death_turkey.ogg");
	key = load_sound("sounds/key.ogg");
	turkey = load_sound("sounds/turkey.ogg");
	wall = load_sound("sounds/wall.ogg");
	hurt = load_sound("sounds/hurt.ogg");
}

void Sound::play_sounds()
{
	//MUSICS
	if (menu->current == None)//plays theme when not in menu
	{
		if(Mix_Playing(2) == 1) Mix_HaltChannel(2);
		else if(Mix_Playing(3) == 1) Mix_HaltChannel(3);

		if(Mix_Playing(1) == 0) play(theme, 1, -1);
		resume_music(1);
	}
	else if (menu->current == Main || menu->current == Help)//plays main menu music
	{
		if(Mix_Playing(2) == 0) play(music_menu, 2, -1);
	}
	else if (menu->current == Pause)//plays pause menu music
	{
		pause_music(1);
		if(Mix_Playing(3) == 0) play(music_pause, 3, -1);
	}
	else if (menu->current == GameOver)//plays game over music
	{
		Mix_HaltChannel(1);
		if(Mix_Playing(2) == 0) play(gameover, 2, -1);
	}
	else if (menu->current == Win)//plays game over music
	{
		Mix_HaltChannel(1);
		if(Mix_Playing(2) == 0) play(victory, 2, -1);
	}

	//SOUND EFFECTS
	if(player->display_flash) 
	{
		play(gunshot, 5, 0);
		player->display_flash = false;
	}

	if(player->turkey_destruct) 
	{
		play(death_turkey, 6, 0);
    	player->turkey_destruct = false;
	}

	if(player->wall_destruct) 
	{
		play(wall, 7, 0);
    	player->wall_destruct = false;
	}
}

Mix_Chunk* Sound::load_sound(std::string filename)
{
	Mix_Chunk* chunk;
	chunk = Mix_LoadWAV(filename.c_str());
	return chunk;
}

void Sound::pause_music(int channel)
{
	if(Mix_Playing(channel) == 1) Mix_Pause(channel);
}

void Sound::resume_music(int channel)
{
	if(Mix_Paused(channel) == 1) Mix_Resume(channel);
}

void Sound::play(Mix_Chunk* sound, int channel, int loops)
{
	Mix_PlayChannel(channel, sound, loops);
}

Sound::~Sound()
{
	//freeMusics
	Mix_FreeChunk(theme);
	Mix_FreeChunk(music_menu);
	Mix_FreeChunk(music_pause);
	Mix_FreeChunk(gameover);
	Mix_FreeChunk(victory);

	//freeSFX
	Mix_FreeChunk(gunshot);
	Mix_FreeChunk(death_turkey);
	Mix_FreeChunk(wall);
	Mix_FreeChunk(key);
	Mix_FreeChunk(turkey);
	Mix_FreeChunk(hurt);

	Mix_CloseAudio();
	Mix_Quit();
}