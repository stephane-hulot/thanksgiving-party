#ifndef _MAP_H_
#define _MAP_H_

#include <SDL/SDL.h>
#include <vector>

struct Sprite
{
	float x, y = 0;
	ushort size = 600;
	ushort itex = 0;
	//squared distance from the player, it's used to sort sprites so no need to calculate the square root
	float sqr_dist = 0;

	bool operator < (const Sprite& s) const
	{
		//sprites further away are sorted before near sprites
		return sqr_dist > s.sqr_dist;
	}
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

	private:
		char* map;
		std::vector<Sprite> sprites;
		Uint32 get_pixel(SDL_Surface* source, ushort x, ushort y);
};

#endif