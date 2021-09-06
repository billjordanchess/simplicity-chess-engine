//book.cpp
#include "globals.h"

extern int hash_start, hash_dest;

char lines[100][80];
move_data book_list[GEN_STACK];
int total2;

void LoadBook();
void InitBookboard();
int Random(const int x);

int start_bb[64] =
{
	3,1,2,4,5,2,1,3,
	0,0,0,0,0,0,0,0,
	6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,
	0,0,0,0,0,0,0,0,
	3,1,2,4,5,2,1,3
};

int start_bc[64] =
{
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,
	1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1
};

int bb[64], bc[64];

int Book()
{
	if (hply > 5)
		return 0;
	int r;
	hash_start = -1;
	r = Random(100);
	if (hply == 0)
	{
		if (r < 50)
		{
			hash_start = D2; hash_dest = D4;
		}
		else
		{
			hash_start = E2; hash_dest = E4;
		}
		return 1;
	}

	int start, dest, match, m = 0;
	int matches[100][2];
	int y, colour;

	for (int x = 0; x < total2; x++)
	{
		InitBookboard();
		for (y = 0; y < hply * 5; y += 5)
		{
			start = lines[x][y] - 'a';
			start += ((lines[x][y + 1] - '0') - 1) * 8;
			dest = lines[x][y + 2] - 'a';
			dest += ((lines[x][y + 3] - '0') - 1) * 8;
			bb[dest] = bb[start];
			bc[dest] = bc[start];
			bb[start] = 6;
			bc[start] = 6;
		}
		match = 1;
		for (int z = 0; z < 64; z++)
		{
			colour = 6;
			if (mask[z] & bit_units[0])
				colour = 0;
			if (mask[z] & bit_units[1])
				colour = 1;
			if ((b[z] != bb[z]) || (colour != bc[z]))
			{
				match = 0; break;
			}
		}
		if (match == 1)
		{
			start = lines[x][y] - 'a';
			start += ((lines[x][y + 1] - '0') - 1) * 8;
			dest = lines[x][y + 2] - 'a';
			dest += ((lines[x][y + 3] - '0') - 1) * 8;
			matches[m][0] = start;
			matches[m][1] = dest;
			m++;
		}

	}

	if (m == 0)
		return 0;
	else
	{
		r = Random(m);
		hash_start = matches[r][0];
		hash_dest = matches[r][1];
		return 1;
	}
}

void LoadBook()
{
	int c = 0;
	strcpy_s(lines[c++], "e2e4 e7e6 d2d4 d7d5 b1c3 g8f6 e4e5 f6d7 ");
	strcpy_s(lines[c++], "e2e4 e7e6 d2d4 d7d5 b1c3 g8f6 c1g5 f8e7 ");
	strcpy_s(lines[c++], "e2e4 e7e6 d2d4 d7d5 b1c3 g8f6 c1g5 f8b4 ");
	strcpy_s(lines[c++], "e2e4 e7e6 d2d4 d7d5 b1c3 f8b4 e4e5 c7c5 ");
	strcpy_s(lines[c++], "e2e4 e7e6 d2d4 d7d5 b1c3 d5e4 c3e4 b8d7 ");
	strcpy_s(lines[c++], "e2e4 e7e6 d2d4 d7d5 b1d2 g8f6 e4e5 f6d7 ");
	strcpy_s(lines[c++], "e2e4 e7e6 d2d4 d7d5 b1d2 c7c5 g1f3 b8c6 ");
	strcpy_s(lines[c++], "e2e4 e7e6 d2d4 d7d5 e4e5 c7c5 c2c3 b8c6 ");
	strcpy_s(lines[c++], "e2e4 c7c6 d2d4 d7d5 e4e5 c8f5 b1c3 e7e6 ");
	strcpy_s(lines[c++], "e2e4 c7c6 d2d4 d7d5 e4d5 c6d5 f1d3 b8c6 ");
	strcpy_s(lines[c++], "e2e4 c7c6 d2d4 d7d5 e4d5 c6d5 c2c4 b8c6 ");
	strcpy_s(lines[c++], "e2e4 c7c6 d2d4 d7d5 b1c3 d5e4 c3e4 c8f5 ");
	strcpy_s(lines[c++], "e2e4 c7c6 d2d4 d7d5 b1c3 d5e4 c3e4 b8d7 ");
	strcpy_s(lines[c++], "e2e4 c7c6 d2d4 d7d5 b1c3 d5e4 c3e4 g8f6 ");
	strcpy_s(lines[c++], "e2e4 c7c6 d2d4 d7d5 b1c3 g7g6 e4e5 f8g7 ");
	strcpy_s(lines[c++], "e2e4 c7c6 d2d4 d7d5 b1c3 g7g6 g1f3 f8g7 ");
	strcpy_s(lines[c++], "e2e4 c7c5 g1f3 b8c6 d2d4 c5d4 f3d4 g8f6 ");
	strcpy_s(lines[c++], "e2e4 c7c5 g1f3 b8c6 d2d4 c5d4 f3d4 g7g6 ");
	strcpy_s(lines[c++], "e2e4 c7c5 g1f3 b8c6 d2d4 c5d4 f3d4 d8c7 ");
	strcpy_s(lines[c++], "e2e4 c7c5 g1f3 b8c6 d2d4 c5d4 f3d4 e7e6 ");
	strcpy_s(lines[c++], "e2e4 c7c5 g1f3 d7d6 d2d4 c5d4 f3d4 g8f6 ");
	strcpy_s(lines[c++], "e2e4 c7c5 g1f3 e7e6 d2d4 c5d4 f3d4 a7a6 ");
	strcpy_s(lines[c++], "e2e4 c7c5 g1f3 g7g6 d2d4 c5d4 d1d4 g8f6 ");
	strcpy_s(lines[c++], "e2e4 c7c5 c2c3 g8f6 e4e5 f6d5 d2d4 c5d4 ");
	strcpy_s(lines[c++], "e2e4 c7c5 c2c3 d7d5 e4d5 d8d5 d2d4 b8c6 ");
	strcpy_s(lines[c++], "e2e4 c7c5 c2c3 b7b6 d2d4 c8b7 f1d3 g8f6 ");
	strcpy_s(lines[c++], "e2e4 c7c5 b1c3 b8c6 f2f4 g7g6 g1f3 f8g7 ");
	strcpy_s(lines[c++], "e2e4 e7e5 f1c4 g8f6 d2d3 c7c6 g1f3 d7d5 ");
	strcpy_s(lines[c++], "e2e4 e7e5 g1f3 b8c6 d2d4 e5d4 f3d4 g8f6 ");
	strcpy_s(lines[c++], "e2e4 e7e5 g1f3 b8c6 d2d4 e5d4 f3d4 f8c5 ");
	strcpy_s(lines[c++], "e2e4 e7e5 g1f3 b8c6 d2d4 e5d4 f3d4 d8h4 ");
	strcpy_s(lines[c++], "e2e4 e7e5 g1f3 b8c6 c2c3 g8f6 d2d4 f6e4 ");
	strcpy_s(lines[c++], "e2e4 e7e5 g1f3 b8c6 f1c4 g8f6 f3g5 d7d5 ");
	strcpy_s(lines[c++], "e2e4 e7e5 g1f3 b8c6 f1c4 f8c5 c2c3 g8f6 ");
	strcpy_s(lines[c++], "e2e4 e7e5 g1f3 b8c6 f1c4 f8e7 d2d4 d7d6 ");
	strcpy_s(lines[c++], "e2e4 e7e5 g1f3 b8c6 f1c4 d7d6 c2c3 f8e7 ");
	strcpy_s(lines[c++], "e2e4 e7e5 g1f3 b8c6 f1b5 f8c5 c2c3 f7f5 ");
	strcpy_s(lines[c++], "e2e4 e7e5 g1f3 b8c6 f1b5 f8c5 c2c3 g8f6 ");
	strcpy_s(lines[c++], "e2e4 e7e5 g1f3 b8c6 f1b5 f8c5 c2c3 d8f6 ");
	strcpy_s(lines[c++], "e2e4 e7e5 g1f3 b8c6 f1b5 g8f6 e1g1 f6e4 ");
	strcpy_s(lines[c++], "e2e4 e7e5 g1f3 b8c6 f1b5 a7a6 b5a4 b7b5 ");
	strcpy_s(lines[c++], "e2e4 e7e5 g1f3 b8c6 f1b5 a7a6 b5a4 d7d6 ");
	strcpy_s(lines[c++], "e2e4 e7e5 g1f3 b8c6 f1b5 a7a6 b5a4 g8e7 ");
	strcpy_s(lines[c++], "e2e4 e7e5 g1f3 b8c6 f1b5 a7a6 b5a4 g8f6 ");
	strcpy_s(lines[c++], "e2e4 e7e5 g1f3 b8c6 f1b5 a7a6 b5c6 d7c6 ");
	strcpy_s(lines[c++], "e2e4 e7e5 g1f3 b8c6 f1b5 d7d6 d2d4 c8d7 ");
	strcpy_s(lines[c++], "e2e4 e7e5 g1f3 b8c6 f1b5 g7g6 c2c3 f8g7 ");
	strcpy_s(lines[c++], "e2e4 e7e5 g1f3 b8c6 b1c3 g8f6 f1b5 f8b4 ");
	strcpy_s(lines[c++], "e2e4 e7e5 g1f3 g8f6 f3e5 d7d6 e5f3 f6e4 ");
	strcpy_s(lines[c++], "e2e4 e7e5 g1f3 d7d6 d2d4 b8d7 f1c4 c7c6 ");
	strcpy_s(lines[c++], "e2e4 e7e5 g1f3 d7d6 d2d4 g8f6 d4e5 f6e4 ");
	strcpy_s(lines[c++], "e2e4 e7e5 g1f3 d7d6 d2d4 g8f6 b1c3 b8d7 ");
	strcpy_s(lines[c++], "e2e4 e7e5 b1c3 g8f6 f2f4 d7d5 f4e5 f6e4 ");
	strcpy_s(lines[c++], "e2e4 e7e5 b1c3 g8f6 f1c4 b8c6 d2d3 f8b4 ");
	strcpy_s(lines[c++], "e2e4 d7d5 e4d5 d8d5 b1c3 d5a5 d2d4 g8f6 ");
	strcpy_s(lines[c++], "d2d4 d7d5 c2c4 e7e6 b1c3 g8f6 c1g5 f8e7 ");
	strcpy_s(lines[c++], "d2d4 d7d5 c2c4 c7c6 b1c3 g8f6 g1f3 e7e6 ");
	strcpy_s(lines[c++], "d2d4 d7d5 c2c4 c7c6 g1f3 g8f6 e2e3 b8d7 ");
	strcpy_s(lines[c++], "d2d4 d7d5 c2c4 d5c4 e2e3 g8f6 f1c4 e7e6 ");
	strcpy_s(lines[c++], "d2d4 d7d5 c2c4 c7c6 c4d5 c6d5 b1c3 g8f6 ");
	strcpy_s(lines[c++], "d2d4 d7d5 c2c4 e7e6 c4d5 e6d5 b1c3 g8f6 ");
	strcpy_s(lines[c++], "d2d4 g8f6 c2c4 c7c5 d4d5 g7g6 b1c3 d7d6 ");
	strcpy_s(lines[c++], "d2d4 g8f6 c2c4 e7e6 b1c3 f8b4 d1c2 c7c5 ");
	strcpy_s(lines[c++], "d2d4 g8f6 c2c4 e7e6 b1c3 f8b4 e2e3 b7b6 ");
	strcpy_s(lines[c++], "d2d4 g8f6 c2c4 e7e6 b1c3 f8b4 e2e3 c7c5");
	strcpy_s(lines[c++], "d2d4 g8f6 c2c4 e7e6 b1c3 f8b4 e2e3 b8c6 ");
	strcpy_s(lines[c++], "d2d4 g8f6 c2c4 e7e6 b1c3 c7c5 g1f3 c5d4 ");
	strcpy_s(lines[c++], "d2d4 g8f6 c2c4 e7e6 b1c3 c7c5 d4d5 e6d5 ");
	strcpy_s(lines[c++], "d2d4 g8f6 c2c4 e7e6 g1f3 f8b4 c1d2 d8e7 ");
	strcpy_s(lines[c++], "d2d4 g8f6 c2c4 e7e6 g1f3 b7b6 g2g3 c8b7 ");
	strcpy_s(lines[c++], "d2d4 g8f6 c2c4 e7e6 g1f3 b7b6 g2g3 c8a6 ");
	strcpy_s(lines[c++], "d2d4 g8f6 c2c4 e7e6 g1f3 c7c5 e2e3 d7d5 ");
	strcpy_s(lines[c++], "d2d4 g8f6 c2c4 g7g6 b1c3 d7d5 c4d5 f6d5 ");
	strcpy_s(lines[c++], "d2d4 g8f6 c2c4 g7g6 b1c3 d7d5 g1f3 f8g7 ");
	strcpy_s(lines[c++], "d2d4 g8f6 c2c4 g7g6 b1c3 d7d5 c1g5 f6e4 ");
	strcpy_s(lines[c++], "d2d4 g8f6 c2c4 g7g6 b1c3 f8g7 e2e4 d7d6 ");
	strcpy_s(lines[c++], "d2d4 g8f6 g1f3 g7g6 c1f4 f8g7 e2e3 d7d6 ");
	strcpy_s(lines[c++], "d2d4 f7f5 c2c4 g8f6 g1f3 e7e6 g2g3 d7d5 ");
	strcpy_s(lines[c++], "d2d4 f7f5 c2c4 g8f6 g1f3 e7e6 g2g3 d7d6 ");
	strcpy_s(lines[c++], "d2d4 d7d5 g1f3 g8f6 e2e3 g7g6 f1e2 f8g7 ");

	total2 = c;
}

void InitBookboard()
{
	for (int x = 0; x < 64; x++)
	{
		bb[x] = start_bb[x];
		bc[x] = start_bc[x];
	}
}

int Random(const int x)
{
	return rand() % x;
}