#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL_mixer.h>
#include "sound.h"

Sound::Sound(Menu* me, Player* p, Map* ma) : themestring("sounds/theme.ogg"), gunshotstring("sounds/gunshot.ogg"), menustring("sounds/menu.ogg"), pausestring("sounds/pause.ogg"), overstring("sounds/gameover.ogg"), winstring("sounds/victory.ogg"), wallstring("sounds/wall.ogg"), deathstring("sounds/death_turkey.ogg"), keystring("sounds/key.ogg"), turkstring("sounds/turkey.ogg"), theme((char*)themestring.c_str()), 
	gunshot((char*)gunshotstring.c_str()), music_menu((char*)menustring.c_str()), music_pause((char*)pausestring.c_str()), gameover((char*)overstring.c_str()), victory((char*)winstring.c_str()), wall((char*)wallstring.c_str()), death_turk((char*)deathstring.c_str()), key((char*)keystring.c_str()), turkey((char*)turkstring.c_str()), menu(me), player(p), map(ma)
{
	
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
		play(gunshot, 6, 0);
		player->display_flash = false;
	}

	if(player->turkey_destruct) 
	{
		play(death_turk, 7, 0);
    	player->turkey_destruct = false;
	}

}

void Sound::init_sounds()
{
	//Initialize SDL_mixer 
	Mix_Init(MIX_INIT_OGG);//			
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
}

Mix_Chunk* Sound::get_sound(char filename[])
{
	Mix_Chunk* chunk;
	chunk = Mix_LoadWAV(filename);

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

void Sound::play(char filename[], int channel, int loops)
{
	Mix_PlayChannel(channel, get_sound(filename), loops);
}

Sound::~Sound()
{
	// quit SDL_mixer
	//freeMusics
	Mix_FreeChunk(get_sound(theme));
	Mix_FreeChunk(get_sound(music_menu));
	Mix_FreeChunk(get_sound(music_pause));
	Mix_FreeChunk(get_sound(gameover));
	Mix_FreeChunk(get_sound(victory));

	//freeSFX
	Mix_FreeChunk(get_sound(gunshot));
	Mix_FreeChunk(get_sound(death_turk));
	Mix_FreeChunk(get_sound(wall));
	Mix_FreeChunk(get_sound(key));
	Mix_FreeChunk(get_sound(turkey));

	Mix_CloseAudio();
	Mix_Quit();
}