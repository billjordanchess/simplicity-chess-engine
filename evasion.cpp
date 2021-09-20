#include "globals.h"

extern int mc;

move_data* m;

const int evade_p[5] = { 0,0,1,1,1 };
const int eb[5] = { 0,0,1,0,2 };
const int er[5] = { 0,0,-2,1,2 };
const int eq[5] = { 0,0,-6,-4,-1 };

void GenEP();

void PushPawn(const int from, const int to);

void EvadePawn(const int from, const int to, const int n);
void EvadeKnight(const int from, const int to);
void EvadeBishop(const int from, const int to, const int n);
void EvadeRook(const int from, const int to, const int n);
void EvadeQueen(const int from, const int to, const int n);
void EvadeKing(const int from, const int to);

void CapPawn(const int from, const int to);
void CapKnight(const int from, const int to);
void CapBishop(const int from, const int to);
void CapRook(const int from, const int to);
void CapQueen(const int from, const int to);
void CapKing(const int from, const int to);

void Evasion()
{
	first_move[ply + 1] = first_move[ply];
	mc = first_move[ply];

	BITBOARD b1;

	int k = pieces[side][5][0];

	int n;

	if (game_list[hply - 1].flags & CHECK)
		n = game_list[hply - 1].to;
	else
		n = GetChecker(xside, side, k);

	GenEP();
	if (bit_left[xside][n] & bit_pieces[side][0])
	{
		CapPawn(pawnleft[xside][n], n);
	}
	if (bit_right[xside][n] & bit_pieces[side][0])
	{
		CapPawn(pawnright[xside][n], n);
	}

	int i;

	b1 = bit_pieces[side][1] & bit_knightmoves[n];
	while (b1)
	{
		i = NextBit(b1);
		b1 &= not_mask[i];
		CapKnight(i, n);
	}

	b1 = bit_pieces[side][2] & bit_bishopmoves[n];
	while (b1)
	{
		i = NextBit(b1);
		b1 &= not_mask[i];
		if (!(bit_between[i][n] & bit_all))
		{
			CapBishop(i, n);
		}
	}

	b1 = bit_pieces[side][3] & bit_rookmoves[n];
	while (b1)
	{
		i = NextBit(b1);
		b1 &= not_mask[i];
		if (!(bit_between[i][n] & bit_all))
		{
			CapRook(i, n);
		}
	}

	b1 = bit_pieces[side][4] & bit_queenmoves[n];
	while (b1)
	{
		i = NextBit(b1);
		b1 &= not_mask[i];
		if (!(bit_between[i][n] & bit_all))
		{
			CapQueen(i, n);
		}
	}

	int sq;

	BITBOARD b2 = bit_kingmoves[pieces[xside][5][0]];
	if (b[n] == 2)
		b2 |= bit_bishopmoves[n];
	else if (b[n] == 3)
		b2 |= bit_rookmoves[n];
	else if (b[n] == 4)
	{
		b2 |= bit_kingmoves[n];
		if (col[n] == col[k] || row[n] == row[k])
			b2 |= bit_rookmoves[n];
		else
			b2 |= bit_bishopmoves[n];
	}
	b1 = bit_kingmoves[k] & bit_units[xside] & ~b2;
	while (b1)
	{
		sq = NextBit(b1);
		b1 &= not_mask[sq];
		if (!(Attack(xside, sq)))
		{
			CapKing(k, sq);
		}
	}
	b1 = bit_kingmoves[k] & ~bit_all & ~b2;
	while (b1)
	{
		sq = NextBit(b1);
		b1 &= not_mask[sq];
		if (!(Attack(xside, sq)))//?? 
			EvadeKing(k, sq);
	}

	if (b[n] < 2 || difference[n][k] == 1)
	{
		first_move[ply + 1] = mc;
		return;
	}

	if (side == 0)
	{
		b1 = bit_pieces[0][0] & (bit_between[n][k] >> 8);
		b2 = bit_between[n][k] & mask_ranks[0][3] & (bit_pieces[0][0] << 16) & ~(bit_all << 8);
	}
	else
	{
		b1 = bit_pieces[1][0] & (bit_between[n][k] << 8);
		b2 = bit_between[n][k] & mask_ranks[1][3] & (bit_pieces[1][0] >> 16) & ~(bit_all >> 8);
	}
	while (b1)
	{
		sq = NextBit(b1);
		b1 &= not_mask[sq];
		PushPawn(sq, pawnplus[side][sq]);
		//EvadePawn(i,sq,n);
	}

	while (b2)
	{
		sq = NextBit(b2);
		b2 &= not_mask[sq];
		PushPawn(pawndouble[xside][sq], sq);
	}

	int x;
	for (x = 0; x < total[side][1]; x++)
	{
		i = pieces[side][1][x];
		b1 = bit_knightmoves[i] & bit_between[n][k];
		while (b1)
		{
			sq = NextBit(b1);
			b1 &= not_mask[sq];
			EvadeKnight(i, sq);
		}
	}

	for (x = 0; x < total[side][2]; x++)
	{
		i = pieces[side][2][x];
		b2 = bit_bishopmoves[i] & bit_between[n][k];
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
				EvadeBishop(i, sq, n);
		}
	}

	for (x = 0; x < total[side][3]; x++)
	{
		i = pieces[side][3][x];
		b2 = bit_rookmoves[i] & bit_between[n][k];
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
				EvadeRook(i, sq, n);
		}
	}

	for (x = 0; x < total[side][4]; x++)
	{
		i = pieces[side][4][x];
		b2 = bit_queenmoves[i] & bit_between[n][k];
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
				EvadeQueen(i, sq, n);
		}
	}
	first_move[ply + 1] = mc;
}

void EvadePawn(const int from, const int to, const int n)
{
	m = &move_list[mc++];
	m->flags = INCHECK;
	m->from = from;
	m->to = to;
	m->score = evade_p[b[n]];
}

void EvadeKnight(const int from, const int to)
{
	m = &move_list[mc++];
	m->flags = INCHECK;
	m->from = from;
	m->to = to;
	m->score = -2;
}

void EvadeBishop(const int from, const int to, const int n)
{
	m = &move_list[mc++];
	m->flags = INCHECK;
	m->from = from;
	m->to = to;
	m->score = eb[b[n]];
}

void EvadeRook(const int from, const int to, const int n)
{
	m = &move_list[mc++];
	m->flags = INCHECK;
	m->from = from;
	m->to = to;
	m->score = er[b[n]];
}

void EvadeQueen(const int from, const int to, const int n)
{
	m = &move_list[mc++];
	m->flags = INCHECK;
	m->from = from;
	m->to = to;
	m->score = eq[b[n]];
}

void EvadeKing(const int from, const int to)
{
	m = &move_list[mc++];
	m->flags = INCHECK;
	m->from = from;
	m->to = to;
	m->score = 0;
}

