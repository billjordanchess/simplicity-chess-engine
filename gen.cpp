#include "globals.h"

unsigned int CAPTURE = (1 << 0);
unsigned int INCHECK = (1 << 1);
unsigned int ATTACK = (1 << 2);
unsigned int CASTLE = (1 << 3);
unsigned int QUIET = (1 << 4);
unsigned int KILLER = (1 << 5);
unsigned int PASSED6 = (1 << 6);
unsigned int PASSED7 = (1 << 7);
unsigned int PROMOTE = (1 << 8);
unsigned int CHECK = (1 << 9);
unsigned int MATETHREAT = (1 << 10);
unsigned int DEFEND = (1 << 11);
unsigned int PAWNMOVE = (1 << 12);
unsigned int COUNTER = (1 << 13);
unsigned int EP = (1 << 14);

int nextq[] = { 1,2,3,4,5,6,7,8,0 };
int nextr[] = { 2,4,6,8,0 };
int nextb[] = { 1,3,5,7,0 };

const BITBOARD m0 = ~mask_ranks[0][6];
const BITBOARD m1 = ~mask_ranks[1][6];

int mc;
int last;
int c = CAPTURE_SCORE;
const int px[6] = { 0 + c,10 + c,20 + c,30 + c,40 + c,0 + c };
const int nx[6] = { c - 3,c + 7,c + 17,c + 27,c + 37,c + 0 };
const int bx[6] = { c - 3,c + 7,c + 17,c + 27,c + 37,c + 0 };
const int rx[6] = { c - 5,c + 5,c + 15,c + 25,c + 35,c + 0 };
const int qx[6] = { c - 9,c + 1,c + 11,c + 21,c + 31,c + 0 };
const int kx[6] = { c + 0,c + 10,c + 20,c + 30,c + 40,c + 0 };

void BishopMoves(const int);
void RookMoves(const int);
void QueenMoves(const int);

void GenEP();

void CapPawn(const int from, const int to);
void CapKnight(const int from, const int to);
void CapBishop(const int from, const int to);
void CapRook(const int from, const int to);
void CapQueen(const int from, const int to);
void CapKing(const int from, const int to);

void Recap(const int from, const int to);

void PushPawn(const int from, const int to);
void PushKnight(const int from, const int to);
void PushBishop(const int from, const int to);
void PushRook(const int from, const int to);
void PushQueen(const int from, const int to);
void PushKing(const int from, const int to);
void AddCastle(const int from, const int to);

void CheckPawn(const int from, const int to);
void CheckKnight(const int from, const int to);
void CheckBishop(const int from, const int to);
void CheckRook(const int from, const int to);
void CheckQueen(const int from, const int to);

void AddEP(const int from, const int to);

BITBOARD SetTargets(const int s, const int v);

move_data* g;
game* h;

int GetChecker(const int s, const int xs, const int sq)
{
	if (bit_knightmoves[sq] & bit_pieces[s][1])
		return NextBit(bit_knightmoves[sq] & bit_pieces[s][1]);

	BITBOARD b1 = bit_bishopmoves[sq] & (bit_pieces[s][2] | bit_pieces[s][4]);
	while (b1)
	{
		int i = NextBit(b1);
		if (!(bit_between[i][sq] & bit_all))
			return i;
		b1 &= not_mask[i];
	}
	b1 = bit_rookmoves[sq] & (bit_pieces[s][3] | bit_pieces[s][4]);
	while (b1)
	{
		int i = NextBit(b1);
		if (!(bit_between[i][sq] & bit_all))
			return i;
		b1 &= not_mask[i];
	}
	if (bit_left[xs][sq] & bit_pieces[s][0])
	{
		return pawnleft[xs][sq];
	}
	if (bit_right[xs][sq] & bit_pieces[s][0])
	{
		return pawnright[xs][sq];
	}
	return -1;
}

void Gen(const int c)
{
	if(c>0)
	{
		Evasion();
		return;
	}
	first_move[ply + 1] = first_move[ply];
	mc = first_move[ply];

	GenEP();

	int i, j, n, sq;

	BITBOARD b1, b2, bp;

	b1 = bit_pieces[side][0] & mask_ranks[side][6];

	while (b1)
	{
		i = NextBit(b1);
		b1 &= not_mask[i];
		sq = pawnplus[side][i];
		if (bit_left[side][i] & bit_units[xside])
		{
			GenPromote(i, pawnleft[side][i]);
		}
		if (bit_right[side][i] & bit_units[xside])
		{
			GenPromote(i, pawnright[side][i]);
		}
		if (b[sq] == EMPTY)
		{
			GenPromote(i, sq);
		}
	}

	if (side == 0)
	{
		b1 = bit_pieces[0][0] & ((bit_units[1] & not_h_file) >> 7) & m0;
		b2 = bit_pieces[0][0] & ((bit_units[1] & not_a_file) >> 9) & m0;
		bp = bit_pieces[0][0] & ~(bit_all >> 8);
	}
	else
	{
		b1 = bit_pieces[1][0] & ((bit_units[0] & not_h_file) << 9) & m1;
		b2 = bit_pieces[1][0] & ((bit_units[0] & not_a_file) << 7) & m1;
		bp = bit_pieces[1][0] & ~(bit_all << 8);
	}

	while (b1)
	{
		i = NextBit(b1);
		b1 &= not_mask[i];
		CapPawn(i, pawnleft[side][i]);
	}
	while (b2)
	{
		i = NextBit(b2);
		b2 &= not_mask[i];
		CapPawn(i, pawnright[side][i]);
	}

	while (bp)
	{
		i = NextBit(bp);
		bp &= not_mask[i];
		PushPawn(i, pawnplus[side][i]);
		if (rank[side][i] == 1 && b[pawndouble[side][i]] == 6)
		{
			PushPawn(i, pawndouble[side][i]);
		}
	}

	if (side == 0) {
		if (castle & 1 && !(bit_between[H1][E1] & bit_all) && Attack(1, F1) == 0)
			AddCastle(E1, G1);
		if (castle & 2 && !(bit_between[A1][E1] & bit_all) &&
			Attack(1, D1) == 0)
			AddCastle(E1, C1);
	}
	else {
		if (castle & 4 && !(bit_between[E8][H8] & bit_all) && Attack(0, F8) == 0)
			AddCastle(E8, G8);
		if (castle & 8 && !(bit_between[E8][A8] & bit_all) && Attack(0, D8) == 0)
			AddCastle(E8, C8);
	}
	int x;
	for (x = 0; x < total[side][1]; x++)
	{
		i = pieces[side][1][x];
		for (j = 0; j < knight_total[i]; j++)
		{
			n = knightmoves[i][j];
			if (b[n] == 6)
				PushKnight(i, n);
			else if (mask[n] & bit_units[xside])
				CapKnight(i, n);
		}
	}

	for (x = 0; x < total[side][4]; x++)
	{
		i = pieces[side][4][x];
		QueenMoves(i);
	}
	for (x = 0; x < total[side][3]; x++)
	{
		i = pieces[side][3][x];
		RookMoves(i);
	}
	for (x = 0; x < total[side][2]; x++)
	{
		i = pieces[side][2][x];
		BishopMoves(i);
	}

	i = pieces[side][5][0];
	for (j = 0; j < king_total[i]; j++)
	{
		n = kingmoves[i][j];
		if (b[n] != EMPTY)
		{
			if (mask[n] & bit_units[xside])
			{
				CapKing(i, n);
			}
			continue;
		}
		PushKing(i, n);
	}
	first_move[ply + 1] = mc;
}

void GenCapsChecks(const int diff, const int depth)
{
	first_move[ply + 1] = first_move[ply];
	mc = first_move[ply];
	int i;
	int sq;
	BITBOARD b1, b2;
	
	b1 = bit_pieces[side][0] & mask_ranks[side][6];

	while (b1)
	{
		i = NextBit(b1);
		b1 &= not_mask[i];
		sq = pawnplus[side][i];
		if (bit_left[side][i] & bit_units[xside])
		{
			GenPromote(i, pawnleft[side][i]);
		}
		if (bit_right[side][i] & bit_units[xside])
		{
			GenPromote(i, pawnright[side][i]);
		}
		if (b[sq] == EMPTY)
		{
			GenPromote(i, sq);
		}
	}
	
	BITBOARD bu = 0;

	if (diff < 900)
	{
		bu = bit_pieces[xside][4];
		if (diff < 500)
		{
			bu |= bit_pieces[xside][3];
			if (diff < 300)
			{
				bu |= bit_pieces[xside][1] | bit_pieces[xside][2];
				if (diff < 100)
				{
					bu |= bit_pieces[xside][0];
				}
			}
		}
	}
	if (bu == 0)
	{
		first_move[ply + 1] = mc;
		return;
	}

	int king = pieces[xside][5][0];

	if (depth == 0)
	{
		GenEP();
	}

	if (side == 0)
	{
		b1 = bit_pieces[0][0] & ((bit_units[1] & not_h_file) >> 7) & m0;
		b2 = bit_pieces[0][0] & ((bit_units[1] & not_a_file) >> 9) & m0;
	}
	else
	{
		b1 = bit_pieces[1][0] & ((bit_units[0] & not_h_file) << 9) & m1;
		b2 = bit_pieces[1][0] & ((bit_units[0] & not_a_file) << 7) & m1;
	}

	while (b1)
	{
		i = NextBit(b1);
		b1 &= not_mask[i];
		CapPawn(i, pawnleft[side][i]);
	}
	while (b2)
	{
		i = NextBit(b2);
		b2 &= not_mask[i];
		CapPawn(i, pawnright[side][i]);
	}

	b1 = bit_knightmoves[king] & bit_pieces[side][0];
	while (b1)
	{
		i = NextBit(b1);
		b1 &= not_mask[i];
		if (rank[side][i] < rank[side][king] && abs(col[i] - col[king]) == 1 && b[pawnplus[side][i]] == 6)
		{
			CheckPawn(i, pawnplus[side][i]);
		}
	}
	BITBOARD bb = ~bu & ~bit_units[side];

	int x;
	for (x = 0; x < total[side][1]; x++)
	{
		i = pieces[side][1][x];
		b2 = bit_knightmoves[i] & bu;
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			CapKnight(i, sq);
		}
		b2 = bit_knightmoves[i] & bit_knightmoves[king] & ~bit_all;
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			CheckKnight(i, sq);
		}
	}
	//start line pieces
	for (x = 0; x < total[side][2]; x++)
	{
		i = pieces[side][2][x];
		b2 = bit_bishopmoves[i] & bu;
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
			{
				CapBishop(i, sq);
			}
		}
		sq = l_check[i][king];
		if (sq > -1 && mask[sq] & bb)
		{
			if (!((bit_between[i][sq] | bit_between[sq][king]) & bit_all))
			{
				CheckBishop(i, sq);
			}
		}
		sq = r_check[i][king];
		if (sq > -1 && mask[sq] & bb)
		{
			if (!((bit_between[i][sq] | bit_between[sq][king]) & bit_all))
			{
				CheckBishop(i, sq);
			}
		}
	}

	for (x = 0; x < total[side][3]; x++)
	{
		i = pieces[side][3][x];
		b2 = bit_rookmoves[i] & bu;
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
			{
				CapRook(i, sq);
			}
		}

		sq = h_check[i][king];
		if (sq > -1 && mask[sq] & bb)
		{
			if (!((bit_between[i][sq] | bit_between[sq][king]) & bit_all))
			{
				CheckRook(i, sq);
			}
		}
		sq = v_check[i][king];
		if (sq > -1 && mask[sq] & bb)
		{
			if (!((bit_between[i][sq] | bit_between[sq][king]) & bit_all))
			{
				CheckRook(i, sq);
			}
		}
	}

	for (x = 0; x < total[side][4]; x++)
	{
		i = pieces[side][4][x];
		b2 = bit_queenmoves[i] & bu;
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];

			if (!(bit_between[i][sq] & bit_all))
			{
				CapQueen(i, sq);
			}
		}
		int z1 = 1;
		sq = q_check[i][king][0];
		while (sq > -1)
		{
			if (mask[sq] & bb && !((bit_between[i][sq] | bit_between[sq][king]) & bit_all))
			{
				CheckQueen(i, sq);
			}
			sq = q_check[i][king][z1++];
		}
	}

	b1 = bit_kingmoves[pieces[side][5][0]] & bu;

	while (b1)
	{
		sq = NextBit(b1);
		b1 &= not_mask[sq];
		CapKing(pieces[side][5][0], sq);
	}
	first_move[ply + 1] = mc;
}

void GenCaps(const int diff)
{
	first_move[ply + 1] = first_move[ply];
	mc = first_move[ply];

	BITBOARD bu = 0;

	if (diff < 900)
	{
		bu = bit_pieces[xside][4];
		if (diff < 500)
		{
			bu |= bit_pieces[xside][3];
			if (diff < 300)
			{
				bu |= bit_pieces[xside][1] | bit_pieces[xside][2];
				if (diff < 100)
				{
					bu |= bit_pieces[xside][0];
				}
			}
		}
	}
	if (bu == 0)
	{
		first_move[ply + 1] = mc;
		return;
	}
	int i;
	int sq;
	BITBOARD b1, b2;

	if (side == 0)
	{
		b1 = bit_pieces[0][0] & ((bit_units[1] & not_h_file) >> 7);
		b2 = bit_pieces[0][0] & ((bit_units[1] & not_a_file) >> 9);
	}
	else
	{
		b1 = bit_pieces[1][0] & ((bit_units[0] & not_h_file) << 9);
		b2 = bit_pieces[1][0] & ((bit_units[0] & not_a_file) << 7);
	}

	while (b1)
	{
		i = NextBit(b1);
		b1 &= not_mask[i];
		CapPawn(i, pawnleft[side][i]);
	}
	while (b2)
	{
		i = NextBit(b2);
		b2 &= not_mask[i];
		CapPawn(i, pawnright[side][i]);
	}
	int x;
	for (x = 0; x < total[side][1]; x++)
	{
		i = pieces[side][1][x];
		b2 = bit_knightmoves[i] & bu;
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			CapKnight(i, sq);
		}
	}

	for (x = 0; x < total[side][2]; x++)
	{
		i = pieces[side][2][x];
		b2 = bit_bishopmoves[i] & bu;
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
			{
				CapBishop(i, sq);
			}
		}
	}

	for (x = 0; x < total[side][3]; x++)
	{
		i = pieces[side][3][x];
		b2 = bit_rookmoves[i] & bu;
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
			{
				CapRook(i, sq);
			}
		}
	}

	for (x = 0; x < total[side][4]; x++)
	{
		i = pieces[side][4][x];
		b2 = bit_queenmoves[i] & bu;
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
			{
				CapQueen(i, sq);
			}
		}
	}

	b1 = bit_kingmoves[pieces[side][5][0]] & bu;

	while (b1)
	{
		sq = NextBit(b1);
		b1 &= not_mask[sq];
		CapKing(pieces[side][5][0], sq);
	}
	first_move[ply + 1] = mc;
}

int gen_recaptures(const int diff, const int dest)
{
	first_move[ply + 1] = first_move[ply];

	if (piece_value[b[dest]] < diff)
	{
		return 0;
	}
	if (b[dest] == 5)
	{
		return 0;
	}

	int i;
	h = &game_list[hply - 1];

	if (bit_pawncaptures[xside][dest] & bit_pieces[side][0])
	{
		if (bit_pieces[side][0] & bit_left[xside][dest] && !LineAttack1(xside, pieces[side][5][0], pawnleft[xside][dest]))
		{
			if ((piece_value[h->capture] + 100 < piece_value[b[dest]] || rank[side][dest] == 0) && !(h->flags & INCHECK))
			{
				return 2;
			}
			Recap(pawnleft[xside][dest], dest);
			return 1;
		}
		if (bit_pieces[side][0] & bit_right[xside][dest] && !LineAttack1(xside, pieces[side][5][0], pawnright[xside][dest]))
		{
			if (piece_value[h->capture] + 100 < piece_value[b[dest]] && !(h->flags & INCHECK))
			{
				return 2;
			}
			Recap(pawnright[xside][dest], dest);
			return 1;
		}
	}

	BITBOARD b1 = bit_knightmoves[dest] & bit_pieces[side][1];
	while (b1)
	{
		i = NextBit(b1);
		b1 &= not_mask[i];
		if (!LineAttack1(xside, pieces[side][5][0], i))
		{
			if (piece_value[h->capture] + 300 < piece_value[b[dest]] && !(h->flags & INCHECK))
			{
				return 2;
			}
			Recap(i, dest);
			return 1;
		}
	}

	b1 = bit_bishopmoves[dest] & bit_pieces[side][2];
	while (b1)
	{
		i = NextBit(b1);
		b1 &= not_mask[i];
		if (!(bit_between[dest][i] & bit_all) && !(LineAttack2(xside, pieces[side][5][0], i, dest)))
		{
			if (piece_value[h->capture] + 300 < piece_value[b[dest]] && !(h->flags & INCHECK))
			{
				return 2;
			}
			Recap(i, dest);
			return 1;
		}
	}

	b1 = bit_rookmoves[dest] & bit_pieces[side][3];
	while (b1)
	{
		i = NextBit(b1);
		b1 &= not_mask[i];
		if (!(bit_between[dest][i] & bit_all) && !(LineAttack2(xside, pieces[side][5][0], i, dest)))
		{
			if (piece_value[h->capture] + 500 < piece_value[b[dest]] && !(h->flags & INCHECK))
			{
				return 2;
			}
			Recap(i, dest);
			return 1;
		}
	}

	b1 = bit_queenmoves[dest] & bit_pieces[side][4];
	while (b1)
	{
		i = NextBit(b1);
		b1 &= not_mask[i];
		if (!(bit_between[dest][i] & bit_all) && !(LineAttack2(xside, pieces[side][5][0], i, dest)))
		{
			if (piece_value[h->capture] + 900 < piece_value[b[dest]] && !(h->flags & INCHECK))
			{
				return 2;
			}
			Recap(i, dest);
			return 1;
		}
	}

	if (bit_kingmoves[pieces[side][5][0]] & mask[dest] && Attack(xside, dest) == 0)
	{
		Recap(pieces[side][5][0], dest);
		return 1;
	}
	return 0;
}

int SafeKingMoves(const int s, const int xs)
{
	int sq;
	int	i = pieces[s][5][0];
	BITBOARD b1 = bit_kingmoves[i] & ~bit_units[s];
	while (b1)
	{
		sq = NextBit(b1);
		b1 &= not_mask[sq];
		if (!(Attack(xs, sq))) 
			return 1;
	}
	return 0;
}

void GenEP()
{
	int ep = game_list[hply - 1].to;

	if (b[ep] == 0 && abs(game_list[hply - 1].from - ep) == 16)
	{
		if (first_move[ply - 1] == first_move[ply])
			return;
		if (col[ep] > 0 && mask[ep - 1] & bit_pieces[side][P])
			AddEP(ep - 1, pawnplus[side][ep]);
		if (col[ep] < 7 && mask[ep + 1] & bit_pieces[side][P])
			AddEP(ep + 1, pawnplus[side][ep]);
	}
}

void AddEP(const int from, const int to)
{
	g = &move_list[mc++];
	g->flags = 0;
	g->from = from;
	g->to = to;
	g->score = px[0];
}

void AddCastle(const int from, const int to)
{
	g = &move_list[mc++];
	g->flags = CASTLE;
	g->from = from;
	g->to = to;
	g->score = history[from][to] + 50;
}

void CapPawn(const int from, const int to)
{
	g = &move_list[mc++];
	g->flags = CAPTURE;
	g->from = from;
	g->to = to;
	g->score = px[b[to]];
}

void CapKnight(const int from, const int to)
{
	g = &move_list[mc++];
	g->flags = CAPTURE;
	g->from = from;
	g->to = to;
	g->score = nx[b[to]];
}

void CapBishop(const int from, const int to)
{
	g = &move_list[mc++];
	g->flags = CAPTURE;
	g->from = from;
	g->to = to;
	g->score = bx[b[to]];
}

void CapRook(const int from, const int to)
{
	g = &move_list[mc++];
	g->flags = CAPTURE;
	g->from = from;
	g->to = to;
	g->score = rx[b[to]];
}

void CapQueen(const int from, const int to)
{
	g = &move_list[mc++];
	g->flags = CAPTURE;
	g->from = from;
	g->to = to;
	g->score = qx[b[to]];
}

void CapKing(const int from, const int to)
{
	g = &move_list[mc++];
	g->flags = CAPTURE;
	g->from = from;
	g->to = to;
	g->score = kx[b[to]];
}

void Recap(const int from, const int to)
{
	g = &move_list[first_move[ply + 1]++];
	g->from = from;
	g->to = to;
}

void PushPawn(const int from, const int to)
{
	g = &move_list[mc++];
	g->flags = 0;
	g->from = from;
	g->to = to;
	g->score = history[from][to];

	if (bit_pawncaptures[side][to] & bit_pieces[xside][5])
	{
		g->flags |= CHECK;
		g->score = check_history[b[from]][to];
		if (g->score > 0)
			g->score += CAPTURE_SCORE;
		g->score += 50;
	}
	g->score += PieceScore[side][0][to] - PieceScore[side][0][from];
}

void PushKnight(const int from, const int to)
{
	g = &move_list[mc++];
	g->flags = 0;
	g->from = from;
	g->to = to;
	g->score = history[from][to];
	if (bit_knightmoves[to] & bit_pieces[xside][5])
	{
		g->flags |= CHECK;
		g->score = check_history[b[from]][to];
		g->score += CAPTURE_SCORE;
	}
}

void PushBishop(const int from, const int to)
{
	g = &move_list[mc++];
	g->flags = 0;
	g->from = from;
	g->to = to;
	g->score = history[from][to];
	if (bit_bishopmoves[to] & bit_pieces[xside][5] && !(bit_between[to][pieces[xside][5][0]] & bit_all))
	{
		g->flags |= CHECK;
		g->score = check_history[b[from]][to];
	}
}

void PushRook(const int from, const int to)
{
	g = &move_list[mc++];
	g->flags = 0;
	g->from = from;
	g->to = to;
	g->score = history[from][to];
	if (bit_rookmoves[to] & bit_pieces[xside][5] && !(bit_between[to][pieces[xside][5][0]] & bit_all))
	{
		g->flags |= CHECK;
		g->score = check_history[b[from]][to];
		g->score += CAPTURE_SCORE;
	}
}

void PushQueen(const int from, const int to)
{
	g = &move_list[mc++];
	g->flags = 0;
	g->from = from;
	g->to = to;
	g->score = history[from][to];
	if (bit_queenmoves[to] & bit_pieces[xside][5] && !(bit_between[to][pieces[xside][5][0]] & bit_all))
	{
		g->flags |= CHECK;
		g->score = check_history[b[from]][to];
		if (difference[pieces[xside][4][0]][to] == 2)
			g->score += CAPTURE_SCORE;
		else if (bit_kingmoves[pieces[xside][5][0]] & mask[to])
		{
			if (colors[pieces[xside][5][0]] == colors[to])
				g->score += 50 + CAPTURE_SCORE;
			else
				g->score += 150 + CAPTURE_SCORE;
		}
	}
}

void PushKing(const int from, const int to)
{
	g = &move_list[mc++];
	g->flags = 0;
	g->from = from;
	g->to = to;
	g->score = history[from][to];
}

void CheckPawn(const int from, const int to)
{
	g = &move_list[mc++];
	g->flags = CHECK;
	if (b[to] < 6) g->flags |= CAPTURE;
	g->from = from;
	g->to = to;
	g->score = check_history[0][to];
}

void CheckKnight(const int from, const int to)
{
	g = &move_list[mc++];
	g->flags = CHECK;
	if (b[to] < 6) g->flags |= CAPTURE;
	g->from = from;
	g->to = to;
	g->score = check_history[1][to];
}

void CheckBishop(const int from, const int to)
{
	g = &move_list[mc++];
	g->flags = CHECK;
	if (b[to] < 6) g->flags |= CAPTURE;
	g->from = from;
	g->to = to;
	g->score = check_history[2][to];
}

void CheckRook(const int from, const int to)
{
	g = &move_list[mc++];
	g->flags = CHECK;
	if (b[to] < 6) g->flags |= CAPTURE;
	g->from = from;
	g->to = to;
	g->score = check_history[3][to];
}

void CheckQueen(const int from, const int to)
{
	g = &move_list[mc++];
	g->flags = CHECK;
	if (b[to] < 6) g->flags |= CAPTURE;
	g->from = from;
	g->to = to;
	g->score = check_history[4][to];
}

void GenPromote(const int from, const int to)
{
	if (ply < 1 && currentmax < 2)
	{
		for (int i = KNIGHT; i <= QUEEN; ++i)
		{
			g = &move_list[mc++];
			g->flags = PROMOTE;
			if (b[to] < 6) g->flags |= CAPTURE;
			g->from = from;
			g->to = to;
			//g->promote = i;
			g->score = CAPTURE_SCORE + (i * 10);

			if (i == 4)//other typs could be added
				if (bit_queenmoves[to] & bit_pieces[xside][5] &&
					!(bit_between[to][pieces[xside][5][0]] & (bit_all ^ mask[from])))
				{
					g->flags |= CHECK;
					g->score += 50;
				}

		}
	}
	else
	{
		g = &move_list[mc++];
		g->flags = PROMOTE;
		if (b[to] < 6) g->flags |= CAPTURE;
		g->from = from;
		g->to = to;
		g->score = CAPTURE_SCORE + 40;
		if (bit_queenmoves[to] & bit_pieces[xside][5] &&
			!(bit_between[to][pieces[xside][5][0]] & (bit_all ^ mask[from])))
		{
			g->flags |= CHECK;
			g->score += 50;
		}
	}
}

void GenNon()
{
	first_move[ply + 1] = first_move[ply];
	mc = first_move[ply];

	int i, j, n;

	BITBOARD bp;

	if (side == 0)
	{
		bp = bit_pieces[0][0] & ~(bit_all >> 8);
	}
	else
	{
		bp = bit_pieces[1][0] & ~(bit_all << 8);
	}

	while (bp)
	{
		i = NextBit(bp);
		bp &= not_mask[i];
		PushPawn(i, pawnplus[side][i]);
		if (rank[side][i] == 1 && b[pawndouble[side][i]] == 6)
		{
			PushPawn(i, pawndouble[side][i]);
		}
	}

	if (side == 0) {
		if (castle & 1 && !(bit_between[H1][E1] & bit_all) && Attack(1, F1) == 0)
			AddCastle(E1, G1);
		if (castle & 2 && !(bit_between[A1][E1] & bit_all) &&
			Attack(1, D1) == 0)
			AddCastle(E1, C1);
	}
	else {
		if (castle & 4 && !(bit_between[E8][H8] & bit_all) && Attack(0, F8) == 0)
			AddCastle(E8, G8);
		if (castle & 8 && !(bit_between[E8][A8] & bit_all) && Attack(0, D8) == 0)
			AddCastle(E8, C8);
	}
	int x;
	for (x = 0; x < total[side][1]; x++)
	{
		i = pieces[side][1][x];
		for (j = 0; j < knight_total[i]; j++)
		{
			n = knightmoves[i][j];
			if (b[n] == 6)
				PushKnight(i, n);
		}
	}

	for (x = 0; x < total[side][4]; x++)
	{
		i = pieces[side][4][x];
		QueenMoves(i);
	}
	for (x = 0; x < total[side][3]; x++)
	{
		i = pieces[side][3][x];
		RookMoves(i);
	}
	for (x = 0; x < total[side][2]; x++)
	{
		i = pieces[side][2][x];
		BishopMoves(i);
	}

	i = pieces[side][5][0];
	for (j = 0; j < king_total[i]; j++)
	{
		n = kingmoves[i][j];
		if (b[n] != EMPTY)
		{
			continue;
		}
		PushKing(i, n);
	}
	first_move[ply + 1] = mc;
}

