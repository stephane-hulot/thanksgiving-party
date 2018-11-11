#ifndef _MAP_H_
#define _MAP_H_

#include <SDL2/SDL.h>
#include <vector>

struct Sprite
{
	float x, y = 0;
	ushort size = 600;
	ushort itex = 0;
	ushort type = 0; //0 = decoration, 1 = ennemy
	//squared distance from the player, it's used to sort sprites so no need to calculate the square root
	float sqr_dist = 0;

	bool operator < (const Sprite& s) const
	{
		//sprites further away are sorted before near sprites
		return sqr_dist > s.sqr_dist;
	}
};

struct Door
{
	ushort x, y = 0;
	float animationState = 1; //0 = fully opened, 0.5 = half opened, 1 = fully closed
};

class Map
{
	public:
        Map();
        ushort w = 32;
		ushort h = 32;
		
		char get_tile(ushort x, ushort y);
		void set_tile(ushort x, ushort y, char tile);
		void sort_sprites(float player_x, float player_y);
		std::vector<Sprite> get_sprites();
		void delete_sprite(ushort id);
		void update_ai(float player_x, float player_y);
		Door get_door(ushort x, ushort y);
		void update_doors(float player_x, float player_y);
		~Map();

	private:
		char* map;
		ushort* dist;
		std::vector<Sprite> sprites;
		std::vector<Door> doors;
		Uint32 get_pixel(SDL_Surface* source, ushort x, ushort y);
		void update_dist_map(ushort px, ushort py);
};

#endif