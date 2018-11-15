#include <iostream>
#include <cmath>
#include <algorithm>
#include "map.h"

Map::Map() : map(NULL), dist(NULL), sprites(std::vector<Sprite>()), doors(std::vector<Door>())
{
	SDL_Surface* map_tex = SDL_LoadBMP("map.bmp");

    if(!map_tex)
    {
        std::cerr << "Couldn't load texture file " << SDL_GetError() << std::endl;
        return;
    }

    if(map_tex->format->BytesPerPixel != 3)
    {
        std::cerr << "Map must be using 8bpp format." << std::endl;
        return;
    }

    w = map_tex->w;
    h = map_tex->h;

    map = new char[w*h];
    dist = new ushort[w*h];

	for(ushort y = 0; y < h; y++)
	{
		for(ushort x = 0; x < w; x++)
		{
			int id = y * w + x;
			Uint32 pixel = get_pixel(map_tex, x, y);

			if(pixel == 0) //black 0,0,0
				map[id] = '0';
			else if(pixel == 65280) //green 0,255,0
				map[id] = '1';
			else if(pixel == 255) //blue 0,0,255
				map[id] = '2';
			else if(pixel == 16776960) //yellow 255,255,0
			{
				map[id] = '3';

				unsigned int index = doors.size();
				doors.push_back(Door());
				doors.at(index).x = x;
				doors.at(index).y = y;
				doors.at(index).animationState = 1;
			}
			else if(pixel == 16711680) //red 255,0,0
			{
				map[id] = ' ';
				
				unsigned int index = sprites.size();
				sprites.push_back(Sprite());
				sprites.at(index).x = x + (std::rand() % 100) / 100.0;
				sprites.at(index).y = y + (std::rand() % 100) / 100.0;
				sprites.at(index).itex = 1;
				sprites.at(index).type = 1; //enemy type
				sprites.at(index).size = 500 + ((id * 50) % 100);
			}
			else if(pixel == 65535) //cyan 0,255,255
			{
				map[id] = ' ';

				for(int i = 0; i < 6; i++)
				{
					unsigned int index = sprites.size();
					sprites.push_back(Sprite());
					sprites.at(index).x = x + (std::rand() % 100) / 100.0;
					sprites.at(index).y = y + (std::rand() % 100) / 100.0;
					sprites.at(index).itex = 2;
					sprites.at(index).type = 0;
					sprites.at(index).size = 200 + ((id * 50) % 100);
				}
			}
			else
				map[id] = ' ';
			
			std::cout<<get_tile(x, y);
		}
		std::cout<<std::endl;
	}
	std::cout<<"Niveau chargé avec "<<sprites.size()<<" sprites"<<std::endl;
	SDL_FreeSurface(map_tex);

	update_dist_map(2, 2);
}

Map::Map(const Map& m) : map(NULL), dist(NULL), sprites(std::vector<Sprite>()), doors(std::vector<Door>())
{
	map = m.map;
	w = m.w;
	h = m.h;
	dist = new ushort[w*h];
}

Map& Map::operator=(Map m)
{
	map = m.map;
	w = m.w;
	h = m.h;
	dist = new ushort[w*h];
    return *this;
}

char Map::get_tile(ushort x, ushort y)
{
	if(x >= w || y >= h)
		return '0';

	return map[y * w + x];
}

void Map::set_tile(ushort x, ushort y, char tile)
{
	if(x > w || y > h)
		return;

	map[y * w + x] = tile; 
}

Door Map::get_door(ushort x, ushort y)
{
	unsigned int i = 0;
	for(i = 0; i < doors.size() - 1; i++)
	{
		if(doors.at(i).x == x && doors.at(i).y == y)
			break;
	}
	return doors.at(i);
}

void Map::update_doors(float player_x, float player_y, float dt)
{
	for(unsigned int i = doors.size(); i > 0; i--)
	{
		float sqr_dist = pow(player_x - doors.at(i-1).x, 2) + pow(player_y - doors.at(i-1).y, 2);
		if(sqr_dist < 10)
		{
			doors.at(i-1).animationState -= dt;

			if(doors.at(i-1).animationState < 0.01)
			{
				set_tile(doors.at(i-1).x, doors.at(i-1).y, ' ');
				doors.erase(doors.begin() + i-1);
			}
		}
	}
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

void Map::delete_sprite(ushort id)
{
	sprites.erase(sprites.begin() + id);
}

void Map::update_ai(float player_x, float player_y)
{
	//player position
	int px = ushort(player_x);
	int py = ushort(player_y);
	if(dist[px+py*w] != 0)
		update_dist_map(px, py);

	//enemy speed
	float speed = 0.03;

	for(unsigned int i = 0; i < sprites.size(); i++)
	{
		if(sprites.at(i).type != 1) continue;

		ushort x = ushort(sprites.at(i).x);
		ushort y = ushort(sprites.at(i).y);

		int d = dist[x + y * w];

		if(d > dist[x+(y-1)*w])
		{
			sprites.at(i).y -= speed;
			sprites.at(i).x += (x - sprites.at(i).x + 0.5) * speed;
		}
		else if(d > dist[x+(y+1)*w])
		{
			sprites.at(i).y += speed;
			sprites.at(i).x += (x - sprites.at(i).x + 0.5) * speed;
		}
		else if(d > dist[(x+1)+y*w])
		{
			sprites.at(i).x += speed;
			sprites.at(i).y += (y - sprites.at(i).y + 0.5) * speed;
		}
		else if(d > dist[(x-1)+y*w])
		{
			sprites.at(i).x -= speed;
			sprites.at(i).y += (y - sprites.at(i).y + 0.5) * speed;
		}
	}
}

void Map::update_dist_map(ushort px, ushort py)
{
	//initialize the distance map
	for(int i = 0; i < h; i++)
	{
		for(int j = 0; j < w; j++)
		{
			dist[j * w + i] = 1000;
		}
	}

	//all for neighbors
	int nx[4] = {0, -1, 1, 0};
	int ny[4] = {-1, 0, 0, 1};

	dist[px+py*w] = 0; //sets player's distance to 0
	for (int iter = 0; iter < 10; iter++)
	{
		for (int y = 0; y < h; y++)
		{
			for (int x = 0; x < w; x++)
			{
				int idx = x + y * w;
				if(map[idx] != ' ') continue;
				//for each neighbor
				for(int i = 0; i < 4; i++)
				{
					int tidx = idx + nx[i] + ny[i] * w;
					if(map[tidx] == ' ' && dist[tidx] > dist[idx])
						dist[tidx] = dist[idx] + 1;
				}		
			}
		}
	}

	/*
	for(int i = 0; i < h; i++)
	{
		for(int j = 0; j < w; j++)
		{
			int idx = i + j * w;
			std::cout<<(char)(dist[idx] > 9 ? '-' : dist[idx] + '0');
		}
		std::cout<<std::endl;
	}
	*/
}

//Gets a pixel from the texture file
Uint32 Map::get_pixel(SDL_Surface* source, ushort x, ushort y)
{
    if(x >= h || y >= w) 
        return 0;

    Uint8 *p = (Uint8 *)source->pixels + (y - 1) * source->pitch + (x + h) * source->format->BytesPerPixel;
    return p[0] | p[1] << 8 | p[2] << 16; 
}

Map::~Map()
{
	delete dist;
	delete map;
    std::cout<<"Map deleted"<<std::endl;
}