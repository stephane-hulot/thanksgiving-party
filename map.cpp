#include "map.h"
#include <iostream>
#include <cmath>
#include <algorithm>

Map::Map() : map(nullptr)
{
	map = new char[32*32+1];

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

void Map::update_ai(float player_x, float player_y)
{
	int px = ushort(player_x);
	int py = ushort(player_y);

	int dist[32*32+1];
	int origine = px+py*w;


	for(int i = 0; i < h; i++)
	{
		for(int j = 0; j < w; j++)
		{
			dist[j * w + i] = 1000;
		}
	}



	
	for(unsigned int i = 0; i < sprites.size(); i++)
	{
		sprites.at(i).sqr_dist = pow(px - sprites.at(i).x, 2) + pow(py - sprites.at(i).y, 2);
	}
	

	dist[origine] = 0;
	for (int iter=0; iter<20; iter++) {
		for (int j=0; j<h; j++) {
			for (int i=0; i<w; i++) {
				int idx = i+j*w;
				if (map[idx]!=' ') continue;
				for (int i=-1; i<=1; i++) {
					for (int j=-1; j<=1; j++) {
						int tidx = idx+i+j*w;
						if (i*j || !(i||j) || map[tidx]!=' ' || dist[tidx]<= dist[idx]) continue;
						dist[tidx] = dist[idx]+1;
					}
				}
			}
		}
	}

	/*for(int i = 0; i < h; i++)
	{
		for(int j = 0; j < w; j++)
		{
			if(dist[j * w + i] >= 10)
				std::cout<<'-';
			else
				std::cout<<dist[j * w + i];
		}
		std::cout<<std::endl;
	}*/

	for(unsigned int i = 0; i < sprites.size(); i++)
	{

		ushort x = ushort(sprites.at(i).x);
		ushort y = ushort(sprites.at(i).y);

		int d = dist[x+y*w];

		if(d > dist[x+(y-1)*w])
		{
			sprites.at(i).y -= 0.1;
		}
		else if(d > dist[x+(y+1)*w])
		{
			sprites.at(i).y += 0.1;
		}
		else if(d > dist[(x+1)+y*w])
		{
			sprites.at(i).x += 0.1;
		}
		else if(d > dist[(x-1)+y*w])
		{
			sprites.at(i).x -= 0.1;
		}


	}
}