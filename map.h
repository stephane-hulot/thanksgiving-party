#ifndef _MAP_H_
#define _MAP_H_

#include <vector>

const char tmp[32*32+1] = "\
00002222222200000000222222220000\
1              01       0      0\
1     200111   01     000      0\
1                     0        0\
0  S     11100000     0   110000\
0  SS          0      000      0\
0     111      0               0\
0   0   11100  00   SSS 11100  0\
0   0   0           SSS 0      0\
0   0      000000   SSS    00000\
0              00              0\
2     S 1               1      0\
0       0   0    00000000      0\
0   00000       000    000    00\
0              00              0\
0  S           00   S          0\
0          S   00              0\
1    111       01       0      0\
1              01     000   S  0\
1                     0        0\
0     S  11100000     0  1110000\
0                  S           0\
0     000      0      000      0\
0   0   11100  00   0   11100  0\
0   0   0           0   0      0\
0   0      000000   0      00000\
0              00    S         0\
2   S   1               1      0\
0       0   0    00000000      0\
0   00000       000  S 000    00\
0          S   00              0\
00322222222000000032222222200000";

struct Sprite
{
	float x, y = 0;
	unsigned short size = 600;
	unsigned short itex = 0;
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
        unsigned short w = 32;
		unsigned short h = 32;
		
		char get_tile(unsigned short x, unsigned short y);
		void set_tile(unsigned short x, unsigned short y, char tile);
		void sort_sprites(float player_x, float player_y);
		std::vector<Sprite> get_sprites();
		void delete_sprite(unsigned short id);

	private:
		char* map;
		std::vector<Sprite> sprites;
};

#endif