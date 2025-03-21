#include "globals.h"

void SetLinks()
{
	link Links[8];
	int next;

	int sq;
	int count = 0;
	BITBOARD b1;

	for (int x = 0; x < 64; x++)
	{
		b1 = 0; count = 0; next = 0;
		for (int k = 0; k < 14; k++)
			bishoplist[x][k].next = -1;
		for (int dir = 1; dir < 8; dir += 2)
		{
			sq = x;
			Links[next].first = count;
			while (true)
			{
				sq = kmoves[sq][dir];
				if (sq == -1)
					break;
				bishoplist[x][count].sq = sq;
				count++;
				if (count < 13)
					Links[next].next = count;
				else
					Links[next].next = -1;
				b1 |= mask[sq];
			}
			Links[next].last = count - 1;
			next++;
		}
		bishoplist[x][count].sq = -1;
		for (int num = 0; num < next; num++)
			for (int z = Links[num].first; z <= Links[num].last; z++)
			{
				bishoplist[x][z].next = Links[num].next;
			}
	}

	for (int x = 0; x < 64; x++)
	{
		b1 = 0; count = 0; next = 0;
		for (int k = 0; k < 15; k++)
			rooklist[x][k].next = -1;
		for (int dir = 0; dir < 8; dir += 2)
		{
			sq = x;
			Links[next].first = count;
			while (true)
			{
				sq = kmoves[sq][dir];
				if (sq == -1)
					break;
				rooklist[x][count].sq = sq;
				count++;
				if (count < 14)
					Links[next].next = count;
				else
					Links[next].next = -1;
				b1 |= mask[sq];
			}
			Links[next].last = count - 1;
			next++;
		}
		rooklist[x][count].sq = -1;
		for (int num = 0; num < next; num++)
			for (int z = Links[num].first; z <= Links[num].last; z++)
			{
				rooklist[x][z].next = Links[num].next;
			}
	}
	for (int x = 0; x < 64; x++)
	{
		b1 = 0; count = 0; next = 0;
		for (int k = 0; k < 28; k++)
			queenlist[x][k].next = -1;
		for (int dir = 0; dir < 8; dir++)
		{
			sq = x;
			Links[next].first = count;
			while (true)
			{
				sq = kmoves[sq][dir];
				if (sq == -1)
					break;
				queenlist[x][count].sq = sq;
				count++;
				if (count < 28)
					Links[next].next = count;
				else
					Links[next].next = -1;
				b1 |= mask[sq];
			}
			Links[next].last = count - 1;
			next++;
		}
		queenlist[x][count].sq = -1;
		for (int num = 0; num < next; num++)
			for (int z = Links[num].first; z <= Links[num].last; z++)
			{
				queenlist[x][z].next = Links[num].next;
			}
	}
}