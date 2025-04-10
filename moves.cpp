#include "globals.h"

list1 queenlist[64][28];
list1 rooklist[64][15];
list1 bishoplist[64][14];

int linemoves[64][8];
int knightmoves[64][8];
int kingmoves[64][8];

void SetKnightMoves();
void SetKingMoves();
void SetLineMoves();

void SetBishopLinks();
void SetRookLinks();
void SetQueenLinks();

void SetKnightMoves()
{
	int count = 0;

	for (int x = 0; x < 64; x++)
	{
		count = 0;
		if (row[x] < 6 && col[x] < 7)
			knightmoves[x][count++] = x + 17;
		if (row[x] < 7 && col[x] < 6)
			knightmoves[x][count++] = x + 10;
		if (row[x] < 6 && col[x]>0)
			knightmoves[x][count++] = x + 15;
		if (row[x] < 7 && col[x]>1)
			knightmoves[x][count++] = x + 6;
		if (row[x] > 1 && col[x] < 7)
			knightmoves[x][count++] = x - 15;
		if (row[x] > 0 && col[x] < 6)
			knightmoves[x][count++] = x - 6;
		if (row[x] > 1 && col[x] > 0)
			knightmoves[x][count++] = x - 17;
		if (row[x] > 0 && col[x] > 1)
			knightmoves[x][count++] = x - 10;
	}
}

void SetKingMoves()
{
	int count = 0;

	for (int x = 0; x < 64; x++)
	{
		count = 0;
		if (col[x] > 0)
			kingmoves[x][count++] = x - 1;
		if (col[x] < 7)
			kingmoves[x][count++] = x + 1;
		if (row[x] > 0)
			kingmoves[x][count++] = x - 8;
		if (row[x] < 7)
			kingmoves[x][count++] = x + 8;
		if (col[x] < 7 && row[x] < 7)
			kingmoves[x][count++] = x + 9;
		if (col[x] > 0 && row[x] < 7)
			kingmoves[x][count++] = x + 7;
		if (col[x] > 0 && row[x] > 0)
			kingmoves[x][count++] = x - 9;
		if (col[x] < 7 && row[x] > 0)
			kingmoves[x][count++] = x - 7;
	}
}

void SetLineMoves()
{
	int count = 0;
		for (int x = 0; x < 64; x++)
	{
		count = 0;
		for (int dir = 0; dir < 8; dir++)
		{
			linemoves[x][dir] = -1;
		}

		if (col[x] > 0) linemoves[x][WEST] = x - 1;
		if (col[x] < 7) linemoves[x][EAST] = x + 1;
		if (row[x] > 0) linemoves[x][SOUTH] = x - 8;
		if (row[x] < 7) linemoves[x][NORTH] = x + 8;
		if (col[x] < 7 && row[x] < 7) linemoves[x][NE] = x + 9;
		if (col[x] > 0 && row[x] < 7) linemoves[x][NW] = x + 7;
		if (col[x] > 0 && row[x] > 0) linemoves[x][SW] = x - 9;
		if (col[x] < 7 && row[x] > 0) linemoves[x][SE] = x - 7;
	}
}

void SetLinks()
{
    SetBishopLinks();
	SetRookLinks();
    SetQueenLinks();
}

void SetBishopLinks()
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
				sq = linemoves[sq][dir];
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
}

void SetRookLinks()
{
	link Links[8];
	int next;

	int sq;
	int count = 0;
	BITBOARD b1;
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
				sq = linemoves[sq][dir];
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
}

void SetQueenLinks()
{
	link Links[8];
	int next;

	int sq;
	int count = 0;
	BITBOARD b1;
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
				sq = linemoves[sq][dir];
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
