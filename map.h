#ifndef _MAP_H_
#define _MAP_H_

const int mapw = 16;
const int maph = 16;
const char map[maph*mapw+1] = "\
0000222222220000\
1              0\
1      11111   0\
1     0        0\
0     0  1110000\
0     3        0\
0     000      0\
0   0   11100  0\
0   0   0      0\
0   0      00000\
0              0\
2       1      0\
0       0   0  0\
0 0000000      0\
0              0\
0002222222200000";

class Map
{
	public:
        Map();

};

#endif