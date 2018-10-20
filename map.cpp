#include "map.h"
#include <iostream>
#include <cmath>
#include <algorithm>

Map::Map() : map(nullptr)
{
	map = new char[32*32+33];

	for(int i = 0; i < h; i++)
	{
		for(int j = 0; j < w; j++)
		{
			int id = i * w + j;
			if(tmp[id] == 'S')
			{
				map[id] = ' ';
				
				unsigned int index = sprites.size();
				sprites.push_back(Sprite());
				sprites.at(index).x = j + ((id * 50) % 100) / 100.0;
				sprites.at(index).y = i + ((id * 20) % 100) / 100.0;
				sprites.at(index).itex = 1;
				sprites.at(index).size = 500 + ((id * 50) % 100);
				

			}
			else
				map[id] = tmp[id];

			std::cout<<get_tile(j, i);
		}
		std::cout<<std::endl;
	}
	std::cout<<"Niveau chargé avec "<<sprites.size()<<" enemis"<<std::endl;
}

char Map::get_tile(unsigned short x, unsigned short y)
{
	if(x >= w || y >= h)
		return '0';

	return map[y * w + x];
}

void Map::set_tile(unsigned short x, unsigned short y, char tile)
{
	if(x > w || y > h)
		return;

	map[y * w + x] = tile; 
}

void Map::sort_sprites(float player_x, float player_y)
{
	for(unsigned int i = 0; i < sprites.size(); i++)
	{
		sprites.at(i).sqr_dist = pow(player_x - sprites.at(i).x, 2) + pow(player_y - sprites.at(i).y, 2);
	}
	std::sort(sprites.begin(), sprites.end());
}

std::vector<Sprite> Map::get_sprites()
{
	return sprites;
}

void Map::delete_sprite(unsigned short id)
{
	sprites.erase(sprites.begin() + id);
}