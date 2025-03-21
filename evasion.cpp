#include "globals.h"

extern int move_count;

move_data* m;

const int evade_pawn[5] = { 0,0,1,1,1 };
const int evade_bishop[5] = { 0,0,1,0,2 };
const int evade_rook[5] = { 0,0,-2,1,2 };
const int evade_queen[5] = { 0,0,-6,-4,-1 };

void GenEP();

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

void Evasion(const int checker)
{
	move_count = first_move[ply + 1];

	BITBOARD b1;

	int k = pieces[side][K][0];
	
	if(b[checker]==P)
		GenEP();
	if (bit_left[xside][checker] & bit_pieces[side][P])
	{
		CapPawn(pawnleft[xside][checker], checker);
	}
	if (bit_right[xside][checker] & bit_pieces[side][P])
	{
		CapPawn(pawnright[xside][checker], checker);
	}

	int i,x;
	if(bit_pieces[side][1] & bit_knightmoves[checker])
	{
		for( x = 0; x < total[side][N]; x++)
		{
			i = pieces[side][N][x];
			if (bit_knightmoves[checker] & mask[i])
				CapKnight(i, checker);
		}
	}

	if (bit_pieces[side][B] & bit_bishopmoves[checker])
	{
		for (x = 0; x < total[side][B]; x++)
		{
			i = pieces[side][B][x];
			if (bit_bishopmoves[checker] & mask[i])
			{
				if (!(bit_between[i][checker] & bit_all))
				{
					CapBishop(i, checker);
				}
			}
		}
	}

	if (bit_pieces[side][R] & bit_rookmoves[checker])
	{
		for (x = 0; x < total[side][R]; x++)
		{
			i = pieces[side][R][x];
			if (bit_rookmoves[checker] & mask[i])
			{
				if (!(bit_between[i][checker] & bit_all))
				{
					CapRook(i, checker);
				}
			}
		}
	}

	if (bit_pieces[side][Q] & bit_queenmoves[checker])
	{
		for (x = 0; x < total[side][Q]; x++)
		{
			i = pieces[side][Q][x];
			if (bit_queenmoves[checker] & mask[i])
			{
				if (!(bit_between[i][checker] & bit_all))
				{
					CapQueen(i, checker);
				}
			}
		}
	}
	
	int sq;

	BITBOARD b2 = bit_kingmoves[pieces[xside][K][0]];
	if (b[checker] == B)
		b2 |= bit_bishopmoves[checker];
	else if (b[checker] == R)
		b2 |= bit_rookmoves[checker];
	else if (b[checker] == Q)
	{
		b2 |= bit_kingmoves[checker];
		if (col[checker] == col[k] || row[checker] == row[k])
			b2 |= bit_rookmoves[checker];
		else
			b2 |= bit_bishopmoves[checker];
	}//bit_all &= not_mask[k]
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
		if (!(Attack(xside, sq)))
			EvadeKing(k, sq);
	}

	if(!(bit_between[checker][k]))
	{
		first_move[ply + 1] = move_count;
		return;
	}

	if (side == 0)
	{
		b1 = bit_pieces[0][P] & (bit_between[checker][k] >> 8);
		b2 = bit_between[checker][k] & mask_ranks[0][3] & (bit_pieces[0][P] << 16) & ~(bit_all << 8);
	}
	else
	{
		b1 = bit_pieces[1][P] & (bit_between[checker][k] << 8);
		b2 = bit_between[checker][k] & mask_ranks[1][3] & (bit_pieces[1][P] >> 16) & ~(bit_all >> 8);
	}
	while (b1)
	{
		sq = NextBit(b1);
		b1 &= not_mask[sq];
		EvadePawn(sq, pawnplus[side][sq], checker);
	}

	while (b2)
	{
		sq = NextBit(b2);
		b2 &= not_mask[sq];
		EvadePawn(pawndouble[xside][sq], sq, checker);
	}

	for (x = 0; x < total[side][N]; x++)
	{
		i = pieces[side][N][x];
		b1 = bit_knightmoves[i] & bit_between[checker][k];
		while (b1)
		{
			sq = NextBit(b1);
			b1 &= not_mask[sq];
			EvadeKnight(i, sq);
		}
	}

	for (x = 0; x < total[side][B]; x++)
	{
		i = pieces[side][B][x];
		b2 = bit_bishopmoves[i] & bit_between[checker][k];
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
				EvadeBishop(i, sq, checker);
		}
	}

	for (x = 0; x < total[side][R]; x++)
	{
		i = pieces[side][R][x];
		b2 = bit_rookmoves[i] & bit_between[checker][k];
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
				EvadeRook(i, sq, checker);
		}
	}

	for (x = 0; x < total[side][Q]; x++)
	{
		i = pieces[side][Q][x];
		b2 = bit_queenmoves[i] & bit_between[checker][k];
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
				EvadeQueen(i, sq, checker);
		}
	}
	first_move[ply + 1] = move_count;
}

void EvadePawn(const int from, const int to, const int sq)
{
	m = &move_list[move_count++];
	m->flags = INCHECK;
	m->from = from;
	m->to = to;
	m->score = evade_pawn[b[sq]];
}

void EvadeKnight(const int from, const int to)
{
	m = &move_list[move_count++];
	m->flags = INCHECK;
	m->from = from;
	m->to = to;
	m->score = -2;
}

void EvadeBishop(const int from, const int to, const int sq)
{
	m = &move_list[move_count++];
	m->flags = INCHECK;
	m->from = from;
	m->to = to;
	m->score = evade_bishop[b[sq]];
}

void EvadeRook(const int from, const int to, const int sq)
{
	m = &move_list[move_count++];
	m->flags = INCHECK;
	m->from = from;
	m->to = to;
	m->score = evade_rook[b[sq]];
}

void EvadeQueen(const int from, const int to, const int sq)
{
	m = &move_list[move_count++];
	m->flags = INCHECK;
	m->from = from;
	m->to = to;
	m->score = evade_queen[b[sq]];
}

void EvadeKing(const int from, const int to)
{
	m = &move_list[move_count++];
	m->flags = INCHECK;
	m->from = from;
	m->to = to;
	m->score = 0;
}

