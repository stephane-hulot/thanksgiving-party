#ifndef _MAP_H_
#define _MAP_H_

#include <SDL2/SDL.h>
#include <vector>

struct Sprite
{
	float x, y = 0;
	unsigned short size = 600;
	unsigned short itex = 0;
	unsigned short type = 0; //0 = decoration, 1 = ennemy, 2 = key, 3 = temp
	//squared distance from the player, it's used to sort sprites so no need to calculate the square root
	float sqr_dist = 0;
	Uint32 start_time = 0;

	bool operator < (const Sprite& s) const
	{
		//sprites further away are sorted before near sprites
		return sqr_dist > s.sqr_dist;
	}

	Sprite() : x(0), y(0), size(600), itex(0), type(0), sqr_dist(0), start_time(SDL_GetTicks()) {}
};

struct Door
{
	unsigned short x, y = 0;
	float animationState = 1; //0 = fully opened, 0.5 = half opened, 1 = fully closed

	Door() : x(0), y(0), animationState(1) {}
};

class Map
{
	public:
        Map();
        unsigned short w = 32;
		unsigned short h = 32;

		float speed = 0.03; //enemy speed
		int damage = 0;
		
		char get_tile(unsigned short x, unsigned short y);
		void set_tile(unsigned short x, unsigned short y, char tile);
		void sort_sprites(float player_x, float player_y);
		std::vector<Sprite> get_sprites();
		void delete_sprite(unsigned short id);
		void update_ai(float player_x, float player_y);
		Door get_door(unsigned short x, unsigned short y);
		bool update_doors(float player_x, float player_y, float dt);
		void animate_sprites();
		int damage_player();
		bool pickup_keys();
		void add_temp_sprite(ushort itex, float x, float y, ushort size);
		~Map();

		//copy constructor to avoid warning in c++11
        Map(const Map& m) : Map() {map = m.map;};
        Map& operator=(Map m);

	private:
		char* map;
		unsigned short* dist;
		std::vector<Sprite> sprites;
		std::vector<Door> doors;
		Uint32 get_pixel(SDL_Surface* source, unsigned short x, unsigned short y);
		void update_dist_map(unsigned short px, unsigned short py);
};

#endif