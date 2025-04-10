#include "globals.h"

extern int move_count;

move_data* m;

const int evade_pawn[5] = { 0,0,1,1,1 };
const int evade_bishop[5] = { 0,0,1,0,2 };
const int evade_rook[5] = { 0,0,-2,1,2 };
const int evade_queen[5] = { 0,0,-6,-4,-1 };

void EvadeKing(const int from, const int to);
void BlockCheck(const int from, const int to, const int score);

void GenEP();

void AddCapture(const int from, const int to, const int score);

void EvadeQuiet(const int checker);
void EvadeCapture(const int checker);

void Evasion(const int checker)
{
	move_count = first_move[ply + 1];

	BITBOARD b1;

	int k = pieces[side][K][0];
	int check_piece = b[checker];
	int i,x;
	
	if(b[checker]==P)
		GenEP();//??
	if (bit_left[xside][checker] & bit_pieces[side][P])
	{
		AddCapture(pawnleft[xside][checker], checker, px[check_piece]);
	}
	if (bit_right[xside][checker] & bit_pieces[side][P])
	{
		AddCapture(pawnright[xside][checker], checker, px[check_piece]);
	}

	if(bit_pieces[side][1] & bit_knightmoves[checker])
	{
		for( x = 0; x < total[side][N]; x++)
		{
			i = pieces[side][N][x];
			if (bit_knightmoves[checker] & mask[i])
				AddCapture(i, checker, nx[check_piece]);
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
					AddCapture(i, checker, bx[check_piece]);
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
					AddCapture(i, checker, rx[check_piece]);
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
					AddCapture(i, checker, qx[check_piece]);
				}
			}
		}
	}
	
	int sq;

	BITBOARD b2 = bit_kingmoves[pieces[xside][K][0]];
	if (check_piece == B)
		b2 |= bit_bishopmoves[checker];
	else if (check_piece == R)
		b2 |= bit_rookmoves[checker];
	else if (check_piece == Q)
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
			AddCapture(k, sq, kx[b[sq]]);
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
		BlockCheck(sq, pawnplus[side][sq], evade_pawn[check_piece]);
	}

	while (b2)
	{
		sq = NextBit(b2);
		b2 &= not_mask[sq];
		BlockCheck(pawndouble[xside][sq], sq, evade_pawn[check_piece]);
	}

	for (x = 0; x < total[side][N]; x++)
	{
		i = pieces[side][N][x];
		b1 = bit_knightmoves[i] & bit_between[checker][k];
		while (b1)
		{
			sq = NextBit(b1);
			b1 &= not_mask[sq];
			BlockCheck(i, sq, -2);
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
				BlockCheck(i, sq, evade_bishop[check_piece]);
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
				BlockCheck(i, sq, evade_rook[check_piece]);
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
				BlockCheck(i, sq, evade_queen[check_piece]);
		}
	}
	first_move[ply + 1] = move_count;
}

void BlockCheck(const int from, const int to, const int score)
{
	m = &move_list[move_count++];
	m->flags = INCHECK;
	m->from = from;
	m->to = to;
	m->score = score;
}

void EvadeKing(const int from, const int to)
{
	m = &move_list[move_count++];
	m->flags = INCHECK;
	m->from = from;
	m->to = to;
	m->score = 0;
}
//
void EvadeCapture(const int checker)
{
	move_count = first_move[ply + 1];

	BITBOARD b1;

	int k = pieces[side][K][0];
	int check_piece = b[checker];
	
	if(b[checker]==P)
		GenEP();
	if (bit_left[xside][checker] & bit_pieces[side][P])
	{
		AddCapture(pawnleft[xside][checker], checker, px[check_piece]);
	}
	if (bit_right[xside][checker] & bit_pieces[side][P])
	{
		AddCapture(pawnright[xside][checker], checker, px[check_piece]);
	}

	int i,x;
	if(bit_pieces[side][1] & bit_knightmoves[checker])
	{
		for( x = 0; x < total[side][N]; x++)
		{
			i = pieces[side][N][x];
			if (bit_knightmoves[checker] & mask[i])
				AddCapture(i, checker, nx[check_piece]);
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
					AddCapture(i, checker, bx[check_piece]);
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
					AddCapture(i, checker, rx[check_piece]);
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
					AddCapture(i, checker, qx[check_piece]);
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
			AddCapture(k, sq, kx[b[sq]]);
		}
	}
	first_move[ply + 1] = move_count;
}

void EvadeQuiet(const int checker)
{
	int i, j, x, sq;
	int k = pieces[side][K][0];
	int check_piece = b[checker];

	move_count = first_move[ply + 1];//

	for (j = 0; j < king_total[k]; j++)
	{
		sq = kingmoves[k][j];
		if (b[sq] != EMPTY)
		{
			continue;
		}
		EvadeKing(k, sq);
	}
	BITBOARD b1, b2;

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
		BlockCheck(sq, pawnplus[side][sq], evade_pawn[check_piece]);
	}

	while (b2)
	{
		sq = NextBit(b2);
		b2 &= not_mask[sq];
		BlockCheck(pawndouble[xside][sq], sq, evade_pawn[check_piece]);
	}

	for (x = 0; x < total[side][N]; x++)
	{
		i = pieces[side][N][x];
		b1 = bit_knightmoves[i] & bit_between[checker][k];
		while (b1)
		{
			sq = NextBit(b1);
			b1 &= not_mask[sq];
			BlockCheck(i, sq, -2);
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
				BlockCheck(i, sq, evade_bishop[check_piece]);
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
				BlockCheck(i, sq, evade_rook[check_piece]);
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
				BlockCheck(i, sq, evade_queen[check_piece]);
		}
	}
	first_move[ply + 1] = move_count;
}
