#include "globals.h"

unsigned int CAPTURE = (1 << 0);
unsigned int CHECK = (1 << 9);
unsigned int INCHECK = (1 << 1);
unsigned int ATTACK = (1 << 2);
unsigned int CASTLE = (1 << 3);
unsigned int QUIET = (1 << 4);
unsigned int KILLER = (1 << 5);
unsigned int PASSED6 = (1 << 6);
unsigned int PASSED7 = (1 << 7);
unsigned int PROMOTE = (1 << 8);
unsigned int COUNTER = (1 << 10);
unsigned int DEFEND = (1 << 11);
unsigned int PAWNMOVE = (1 << 12);
unsigned int EP = (1 << 13);
unsigned int MATETHREAT = (1 << 14);

int move_count;

int c = CAPTURE_SCORE;
const int px[6] = { 0 + c,c + 100,c + 200,c + 300,c + 400,0 + c };
const int nx[6] = { c - 30,c + 70,c + 170,c + 270,c + 370,c + 0 };
const int bx[6] = { c - 30,c + 70,c + 170,c + 270,c + 370,c + 0 };
const int rx[6] = { c - 50,c + 50,c + 150,c + 250,c + 350,c + 0 };
const int qx[6] = { c - 90,c + 10,c + 110,c + 210,c + 310,c + 0 };
const int kx[6] = { c + 00,c + 100,c + 200,c + 300,c + 400,c + 0 };

void BishopMoves(const int);
void RookMoves(const int);
void QueenMoves(const int);

void GenEP();
/*
void AddCapture(const int from, const int to);
void AddCapture(const int from, const int to);
void AddCapture(const int from, const int to);
void AddCapture(const int from, const int to);
void AddCapture(const int from, const int to);
void AddCapture(const int from, const int to);
*/

void AddRecapture(const int from, const int to);

void AddPawn(const int from, const int to);
void AddKnight(const int from, const int to);
void AddBishop(const int from, const int to);
void AddRook(const int from, const int to);
void AddQueen(const int from, const int to);
void AddKing(const int from, const int to);
void AddCastle(const int from, const int to);

void AddCheck(const int from, const int to, const int p);

void AddEP(const int from, const int to);

void AddCapture(const int from, const int to, const int score);

move_data* g;
game* h = &game_list[hply];

void GenCapsChecks(const int diff, const int depth)
{
	first_move[ply + 1] = first_move[ply];
	move_count = first_move[ply];

	int i;
	int sq;
	BITBOARD b1, b2;
	
	b1 = bit_pieces[side][P] & mask_ranks[side][6];
	bit_all = bit_units[0] | bit_units[1];

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
	
	BITBOARD bit_targets = 0;

	if (diff < QVAL)
	{
		bit_targets = bit_pieces[xside][Q];
		if (diff < RVAL)
		{
			bit_targets |= bit_pieces[xside][R];
			if (diff < BVAL)
			{
				bit_targets |= bit_pieces[xside][1] | bit_pieces[xside][B];
				if (diff < PVAL)
				{
					bit_targets |= bit_pieces[xside][0];
				}
			}
		}
	}

	if (depth == 0)
	{
		GenEP();
	}

	int king = pieces[xside][K][0];

	if (side == 0)
	{
		b1 = bit_pieces[0][P] & ((bit_targets & not_h_file) >> 7) & not_rank6;
		b2 = bit_pieces[0][P] & ((bit_targets & not_a_file) >> 9) & not_rank6;
	}
	else
	{
		b1 = bit_pieces[1][P] & ((bit_targets & not_h_file) << 9) & not_rank1;
		b2 = bit_pieces[1][P] & ((bit_targets & not_a_file) << 7) & not_rank1;
	}

	while (b1)
	{
		i = NextBit(b1);
		b1 &= not_mask[i];
		AddCapture(i, pawnleft[side][i], px[b[i]]);
	}
	while (b2)
	{
		i = NextBit(b2);
		b2 &= not_mask[i];
		AddCapture(i, pawnright[side][i], px[b[i]]);
	}

	b1 = bit_pawndefends[side][king] & bit_units[xside];
	while (b1)
	{
		i = NextBit(b1);
		b1 &= not_mask[i];
		b2 = bit_pawndefends[side][i] & bit_pieces[side][0];
		while(b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			AddCheck(sq, i, P);
		}
	}

	b1 = bit_knightmoves[king] & bit_pieces[side][0];
	while (b1)
	{
		i = NextBit(b1);
		b1 &= not_mask[i];
		if (rank[side][i] < rank[side][king] && 
			abs(col[i] - col[king]) == 1 && b[pawnplus[side][i]] == EMPTY)
		{
			AddCheck(i, pawnplus[side][i], P);
		}
	}
	BITBOARD not_bu = ~bit_targets & ~bit_units[side];

	int x;
	for (x = 0; x < total[side][N]; x++)
	{
		i = pieces[side][1][x];
		b2 = bit_knightmoves[i] & bit_targets;
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			AddCapture(i, sq, nx[b[sq]]);
		}
		b2 = bit_knightmoves[i] & bit_knightmoves[king] & ~bit_targets & ~bit_units[side];
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			AddCheck(i, sq, N);
		}
	}
	
	for (x = 0; x < total[side][B]; x++)
	{
		i = pieces[side][B][x];
		b2 = bit_bishopmoves[i] & bit_targets;
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
			{
				AddCapture(i, sq, bx[b[sq]]);
			}
		}
		sq = l_check[i][king];
		if (sq > -1 && mask[sq] & not_bu)
		{
			if (!((bit_between[i][sq] | bit_between[sq][king]) & bit_all))
			{
				AddCheck(i, sq, B);
			}
		}
		sq = r_check[i][king];
		if (sq > -1 && mask[sq] & not_bu)
		{
			if (!((bit_between[i][sq] | bit_between[sq][king]) & bit_all))
			{
				AddCheck(i, sq, B);
			}
		}
		if (bit_bishopmoves[i] & mask[king])
		{
			b2 = bit_bishopmoves[i] & ~bit_targets & bit_units[xside] & bit_bishopmoves[king];
			while (b2)
			{
				sq = NextBit(b2);
				b2 &= not_mask[sq];
				if (!(bit_between[i][sq] & bit_all))
				{
					if (!((bit_between[i][sq] | bit_between[sq][king]) & bit_all))
					{
						AddCheck(i, sq, B);
					}
				}
			}
		}
	}

	for (x = 0; x < total[side][R]; x++)
	{
		i = pieces[side][R][x];
		sq = h_check[i][king];
		if (sq > -1 && !(mask[sq] & bit_units[side]))
		{
			if (!((bit_between[i][sq] | bit_between[sq][king]) & bit_all))
			{
				AddCheck(i, sq, R);
			}
		}
		sq = v_check[i][king];
		if (sq > -1 && !(mask[sq] & bit_units[side]))
		{
			if (!((bit_between[i][sq] | bit_between[sq][king]) & bit_all))
			{
				AddCheck(i, sq, R);
			}
		}

		b2 = bit_rookmoves[i] & bit_targets;
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
			{
				AddCapture(i, sq, rx[b[sq]]);
			}
		}
		if (bit_rookmoves[i] & mask[king])
		{
			b2 = bit_rookmoves[i] & ~bit_targets & bit_units[xside] & bit_rookmoves[king];
			while (b2)
			{
				sq = NextBit(b2);
				b2 &= not_mask[sq];
				if (!(bit_between[i][sq] & bit_all))
				{
					if (!((bit_between[i][sq] | bit_between[sq][king]) & bit_all))
					{
						AddCheck(i, sq, R);
					}
				}
			}
		}
	}

	for (x = 0; x < total[side][Q]; x++)
	{
		i = pieces[side][Q][x];
		b2 = bit_queenmoves[i] & bit_targets;
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];

			if (!(bit_between[i][sq] & bit_all))
			{
				AddCapture(i, sq, qx[b[sq]]);
			}
		}
		int z1 = 1;
		sq = q_check[i][king][0];
		while (sq > -1)
		{
			if (mask[sq] & not_bu && 
				!((bit_between[i][sq] | bit_between[sq][king]) & bit_all))
			{
				AddCheck(i, sq, Q);
			}
			sq = q_check[i][king][z1++];
		}
	}

	b1 = bit_kingmoves[pieces[side][K][0]] & bit_targets;

	while (b1)
	{
		sq = NextBit(b1);
		b1 &= not_mask[sq];
		if(!Attack(xside,sq))//24
			AddCapture(pieces[side][K][0], sq, kx[b[sq]]);
	}
	first_move[ply + 1] = move_count;
}

void GenQuietCaptures(const int diff)
{
	first_move[ply + 1] = first_move[ply];
	move_count = first_move[ply];

	BITBOARD bit_targets = 0;

	/*/
	int limit = piece_value[game_list[hply-1].capture] - piece_value[game_list[hply-2].capture];
	if(limit>0 && !(game_list[hply-2].flags & INCHECK))
	{
		//printf("+");
		printf(" diff %d ",diff);
		printf(" limit %d ",limit);
	printf("\n rs");
	z();
	}
	/*/

	if (diff < QVAL)
	{
		bit_targets = bit_pieces[xside][Q];
		if (diff < RVAL)
		{
			bit_targets |= bit_pieces[xside][R];
			if (diff < BVAL)
			{
				bit_targets |= bit_pieces[xside][N] | bit_pieces[xside][B];
				if (diff < PVAL)
				{
					bit_targets |= bit_pieces[xside][P];
				}
			}
		}
	}
	if (bit_targets == 0)
	{
		first_move[ply + 1] = move_count;
		return;
	}
	int i;
	int sq;
	BITBOARD b1, b2;

	if (side == 0)
	{
		b1 = bit_pieces[0][P] & ((bit_targets & not_h_file) >> 7);
		b2 = bit_pieces[0][P] & ((bit_targets & not_a_file) >> 9);
	}
	else
	{
		b1 = bit_pieces[1][P] & ((bit_targets & not_h_file) << 9);
		b2 = bit_pieces[1][P] & ((bit_targets & not_a_file) << 7);
	}

	while (b1)
	{
		i = NextBit(b1);
		b1 &= not_mask[i];
		AddCapture(i, pawnleft[side][i], px[b[pawnleft[side][i]]]);
	}
	while (b2)
	{
		i = NextBit(b2);
		b2 &= not_mask[i];
		AddCapture(i, pawnright[side][i], px[b[pawnright[side][i]]]);
	}
	int x;
	for (x = 0; x < total[side][N]; x++)
	{
		i = pieces[side][1][x];
		b2 = bit_knightmoves[i] & bit_targets;
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			AddCapture(i, sq, px[b[sq]]);
		}
	}

	for (x = 0; x < total[side][B]; x++)
	{
		i = pieces[side][B][x];
		b2 = bit_bishopmoves[i] & bit_targets;
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
			{
				AddCapture(i, sq, bx[b[sq]]);
			}
		}
	}

	for (x = 0; x < total[side][R]; x++)
	{
		i = pieces[side][R][x];
		b2 = bit_rookmoves[i] & bit_targets;
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
			{
				AddCapture(i, sq, rx[b[sq]]);
			}
		}
	}

	for (x = 0; x < total[side][Q]; x++)
	{
		i = pieces[side][Q][x];
		b2 = bit_queenmoves[i] & bit_targets;
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
			{
				AddCapture(i, sq, qx[b[sq]]);
			}
		}
	}

	b1 = bit_kingmoves[pieces[side][K][0]] & bit_targets;

	while (b1)
	{
		sq = NextBit(b1);
		b1 &= not_mask[sq];
		if(!Attack(xside,sq))//24
			AddCapture(pieces[side][K][0], sq, kx[b[sq]]);
	}
	first_move[ply + 1] = move_count;
}

void GenCaptures()
{
	move_count = first_move[ply + 1];

	GenEP();

	int i;
	int sq;
	BITBOARD b1, b2;
	BITBOARD bit_targets = bit_units[xside];

	b1 = bit_pieces[side][P] & mask_ranks[side][6];

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
		b1 = bit_pieces[0][P] & ((bit_targets & not_h_file) >> 7) & not_rank6;
		b2 = bit_pieces[0][P] & ((bit_targets & not_a_file) >> 9) & not_rank6;
	}
	else
	{
		b1 = bit_pieces[1][P] & ((bit_targets & not_h_file) << 9) & not_rank1;
		b2 = bit_pieces[1][P] & ((bit_targets & not_a_file) << 7) & not_rank1;
	}
	while (b1)
	{
		i = NextBit(b1);
		b1 &= not_mask[i];
		sq = pawnleft[side][i];
		AddCapture(i, sq, px[b[sq]]);		
	}
	while (b2)
	{
		i = NextBit(b2);
		b2 &= not_mask[i];
		sq = pawnright[side][i];
		AddCapture(i, sq, px[b[sq]]);
	}
	int x;
	for (x = 0; x < total[side][N]; x++)
	{
		i = pieces[side][1][x];
		b2 = bit_knightmoves[i] & bit_targets;
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			AddCapture(i, sq, nx[b[sq]]);
		}
	}

	for (x = 0; x < total[side][B]; x++)
	{
		i = pieces[side][B][x];
		b2 = bit_bishopmoves[i] & bit_targets;
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
			{
				AddCapture(i, sq, bx[b[sq]]);
			}
		}
	}

	for (x = 0; x < total[side][R]; x++)
	{
		i = pieces[side][R][x];
		b2 = bit_rookmoves[i] & bit_targets;
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
			{
				AddCapture(i, sq, rx[b[sq]]);
			}
		}
	}

	for (x = 0; x < total[side][Q]; x++)
	{
		i = pieces[side][Q][x];
		b2 = bit_queenmoves[i] & bit_targets;
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
			{
				AddCapture(i, sq, qx[b[sq]]);
			}
		}
	}

	b1 = bit_kingmoves[pieces[side][K][0]] & bit_targets;

	while (b1)
	{
		sq = NextBit(b1);
		b1 &= not_mask[sq];
		if(!Attack(xside,sq))
			AddCapture(pieces[side][K][0], sq, kx[b[sq]]);
	}
	first_move[ply + 1] = move_count;
}

int GenRecaptures(const int diff, const int to)
{
	first_move[ply + 1] = first_move[ply];

	if (piece_value[b[to]] < diff)
	{
		return NO_MOVES;
	}
	if (b[to] == K)
	{
		return NO_MOVES;
	}

	h = &game_list[hply - 1];

	if (bit_pawncaptures[xside][to] & bit_pieces[side][0])
	{
		if (bit_pieces[side][0] & bit_left[xside][to] && 
			!isPinned(xside, pieces[side][K][0], pawnleft[xside][to]))
		{
			if ((piece_value[h->capture] + PVAL < piece_value[b[to]]) && 
				!(h->flags & INCHECK))
			{
				return RECAPTURE_STOP;
			}
			AddRecapture(pawnleft[xside][to], to);
			return RECAPTURE_GO;
		}
		if (bit_pieces[side][0] & bit_right[xside][to] && 
			!isPinned(xside, pieces[side][K][0], pawnright[xside][to]))
		{
			if (piece_value[h->capture] + PVAL < piece_value[b[to]] && !(h->flags & INCHECK))
			{
				return RECAPTURE_STOP;
			}
			AddRecapture(pawnright[xside][to], to);
			return RECAPTURE_GO;
		}
	}

	int i;
	if(bit_knightmoves[to] & bit_pieces[side][1])
	for (int x = 0; x < total[side][N]; x++)
	{
		i = pieces[side][1][x];
		if (bit_knightmoves[to] & mask[i])
		{
			if (!isPinned(xside, pieces[side][K][0], i))
			{
				if (piece_value[h->capture] + BVAL < piece_value[b[to]] && 
					!(h->flags & INCHECK))
				{
					return RECAPTURE_STOP;
				}
				AddRecapture(i, to);
				return RECAPTURE_GO;
			}
		}
	}

	if(bit_bishopmoves[to] & bit_pieces[side][B])
	for (int x = 0; x < total[side][B]; x++)
	{
		i = pieces[side][B][x];
		if (bit_bishopmoves[to] & mask[i])
		{
			if (!(bit_between[to][i] & bit_all) && 
				!(isPinnedLinePiece(xside, pieces[side][K][0], i, to)))
			{
				if (piece_value[h->capture] + BVAL < piece_value[b[to]] && 
					!(h->flags & INCHECK))
				{
					return RECAPTURE_STOP;
				}
				AddRecapture(i, to);
				return RECAPTURE_GO;
			}
		}
	}

	if(bit_rookmoves[to] & bit_pieces[side][R])
	for (int x = 0; x < total[side][R]; x++)
	{
		i = pieces[side][R][x];
		if (bit_rookmoves[to] & mask[i])
		{
			if (!(bit_between[to][i] & bit_all) && 
				!(isPinnedLinePiece(xside, pieces[side][K][0], i, to)))
			{
				if (piece_value[h->capture] + RVAL < piece_value[b[to]] && 
					!(h->flags & INCHECK))
				{
					return RECAPTURE_STOP;
				}
				AddRecapture(i, to);
				return RECAPTURE_GO;
			}
		}
	}

	if( bit_queenmoves[to] & bit_pieces[side][Q])
	for (int x = 0; x < total[side][Q]; x++)
	{
		i = pieces[side][Q][x];
		if (bit_queenmoves[to] & mask[i])
		{
			if (!(bit_between[to][i] & bit_all) && 
				!(isPinnedLinePiece(xside, pieces[side][K][0], i, to)))
			{
				AddRecapture(i, to);
				return RECAPTURE_GO;
			}
		}
	}

	if (bit_kingmoves[pieces[side][K][0]] & mask[to] && 
		Attack(xside, to) == 0)
	{
		AddRecapture(pieces[side][K][0], to);
		return RECAPTURE_GO;
	}
	return NO_MOVES;
}

int SafeKingMoves(const int s, const int xs)
{
	int sq;
	int	i = pieces[s][K][0];
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

	//if (b[ep] == P && abs(game_list[hply - 1].from - ep) == 16)
	if (bit_pieces[xside][P] & mask[ep] && abs(game_list[hply - 1].from - ep) == 16 && rank[side][ep]==4)
	{
		if (col[ep] > 0 && mask[ep - 1] & bit_pieces[side][P])
			AddEP(ep - 1, pawnplus[side][ep]);
		if (col[ep] < 7 && mask[ep + 1] & bit_pieces[side][P])
			AddEP(ep + 1, pawnplus[side][ep]);
	}
}

void AddEP(const int from, const int to)
{
	g = &move_list[move_count++];
	g->flags = EP;
	g->from = from;
	g->to = to;
	g->score = px[0];
}

void AddCastle(const int from, const int to)
{
	g = &move_list[move_count++];
	g->flags = CASTLE;
	g->from = from;
	g->to = to;
	g->score = history[from][to];
}

void AddCapture(const int from, const int to, const int score)
{
	g = &move_list[move_count++];
	g->flags = CAPTURE;
	g->from = from;
	g->to = to;
	g->score = score;
}

void AddRecapture(const int from, const int to)
{
	g = &move_list[first_move[ply + 1]++];
	g->from = from;
	g->to = to;
}

void AddPawn(const int from, const int to)
{
	g = &move_list[move_count++];
	g->flags = 0;
	g->from = from;
	g->to = to;
	g->score = history[from][to];

	if (bit_pawncaptures[side][to] & bit_pieces[xside][K])
	{
		g->flags |= CHECK;
		g->score = check_history[b[from]][to];
		if (g->score > 0)
			g->score += CHECK_SCORE;
		g->score += 50;
	}
	else
		if(mask[to] & mask_ranks[side][6])
		{
			g->score += CAPTURE_SCORE;
			g->flags |= PASSED7;
		}
}

void AddKnight(const int from, const int to)
{
	g = &move_list[move_count++];
	g->flags = 0;
	g->from = from;
	g->to = to;
	g->score = history[from][to];
	if (bit_knightmoves[to] & bit_pieces[xside][K])
	{
		g->flags |= CHECK;
		g->score = check_history[b[from]][to];
		g->score += CHECK_SCORE;
	}
}

void AddBishop(const int from, const int to)
{
	g = &move_list[move_count++];
	g->flags = 0;
	g->from = from;
	g->to = to;
	g->score = history[from][to];
	if (bit_bishopmoves[to] & bit_pieces[xside][K] && 
		!(bit_between[to][pieces[xside][K][0]] & bit_all))
	{
		g->flags |= CHECK;
		g->score = check_history[b[from]][to];
		g->score += CHECK_SCORE;
	}
}

void AddRook(const int from, const int to)
{
	g = &move_list[move_count++];
	g->flags = 0;
	g->from = from;
	g->to = to;
	g->score = history[from][to];
	if (bit_rookmoves[to] & bit_pieces[xside][K] && 
		!(bit_between[to][pieces[xside][K][0]] & bit_all))
	{
		g->flags |= CHECK;
		g->score = check_history[b[from]][to];
		g->score += CHECK_SCORE;
	}
}

void AddQueen(const int from, const int to)
{
	g = &move_list[move_count++];
	g->flags = 0;
	g->from = from;
	g->to = to;
	g->score = history[from][to];
	if (bit_queenmoves[to] & bit_pieces[xside][K] && 
		!(bit_between[to][pieces[xside][K][0]] & bit_all))
	{
		g->flags |= CHECK;
		g->score = check_history[b[from]][to];
		if (difference[pieces[xside][Q][0]][to] == 2)
			g->score += CHECK_SCORE;
		else if (bit_kingmoves[pieces[xside][K][0]] & mask[to])
		{
			if (colors[pieces[xside][K][0]] == colors[to])
				g->score += 50 + CHECK_SCORE;
			else
				g->score += 150 + CHECK_SCORE;
		}
	}
}

void AddKing(const int from, const int to)
{
	g = &move_list[move_count++];
	g->flags = 0;
	g->from = from;
	g->to = to;
	g->score = history[from][to];
}

void AddCheck(const int from, const int to, const int p)
{
	g = &move_list[move_count++];
	g->flags = CHECK;
	if (b[to] != EMPTY) g->flags |= CAPTURE;
	g->from = from;
	g->to = to;
	g->score = check_history[p][to];
}

void GenPromote(const int from, const int to)
{
	if (ply < 1 && currentmax < 2)
	{
		for (int i = KNIGHT; i <= QUEEN; i++)
		{
			g = &move_list[move_count++];
			g->flags = PROMOTE;
			if (b[to] != EMPTY) g->flags |= CAPTURE;
			g->from = from;
			g->to = to;
			g->score = CAPTURE_SCORE + (i * 10);

			if (i == 4)//other types could be added
				if (bit_queenmoves[to] & bit_pieces[xside][K] &&
					!(bit_between[to][pieces[xside][K][0]] & (bit_all ^ mask[from])))
				{
					g->flags |= CHECK;
					g->score += 50;
				}

		}
	}
	else
	{
		g = &move_list[move_count++];
		g->flags = PROMOTE;
		if (b[to] != EMPTY) 
			g->flags |= CAPTURE;
		g->from = from;
		g->to = to;
		g->score = CAPTURE_SCORE + 40;
		if (bit_queenmoves[to] & bit_pieces[xside][K] &&
			!(bit_between[to][pieces[xside][K][0]] & (bit_all ^ mask[from])))
		{
			g->flags |= CHECK;
			g->score += 50;
		}
	}
}

void GenNonCaptures() 
{
	move_count = first_move[ply + 1];

	int i, j, sq;

	BITBOARD bit_pawns;

	if (side == 0)
	{
		bit_pawns = bit_pieces[0][P] & not_rank6 & ~(bit_all >> 8);
	}
	else
	{
		bit_pawns = bit_pieces[1][P] & not_rank1 & ~(bit_all << 8);
	}

	while (bit_pawns)
	{
		i = NextBit(bit_pawns);
		bit_pawns &= not_mask[i];
		AddPawn(i, pawnplus[side][i]);
		if (rank[side][i] == 1 && b[pawndouble[side][i]] == EMPTY)
		{
			AddPawn(i, pawndouble[side][i]);
		}
	}

	if (side == 0) {
		if (castle & 1 && !(bit_between[H1][E1] & bit_all) && Attack(1, F1) == 0)
			AddCastle(E1, G1);
		if (castle & 2 && !(bit_between[A1][E1] & bit_all) && Attack(1, D1) == 0)
			AddCastle(E1, C1);
	}
	else {
		if (castle & 4 && !(bit_between[E8][H8] & bit_all) && Attack(0, F8) == 0)
			AddCastle(E8, G8);
		if (castle & 8 && !(bit_between[E8][A8] & bit_all) && Attack(0, D8) == 0)
			AddCastle(E8, C8);
	}
	int x;
	for (x = 0; x < total[side][N]; x++)
	{
		i = pieces[side][1][x];
		for (j = 0; j < knight_total[i]; j++)
		{
			sq = knightmoves[i][j];
			if (b[sq] == EMPTY)
				AddKnight(i, sq);
		}
	}

	for (x = 0; x < total[side][Q]; x++)
	{
		i = pieces[side][Q][x];
		QueenMoves(i);
	}
	for (x = 0; x < total[side][R]; x++)
	{
		i = pieces[side][R][x];
		RookMoves(i);
	}
	for (x = 0; x < total[side][B]; x++)
	{
		i = pieces[side][B][x];
		BishopMoves(i);
	}

	i = pieces[side][K][0];
	for (j = 0; j < king_total[i]; j++)
	{
		sq = kingmoves[i][j];
		if (b[sq] != EMPTY)
		{
			continue;
		}
		AddKing(i, sq);
	}
	first_move[ply + 1] = move_count;
}

void RookMoves(const int x)
{
    int next = 0;
    int sq = rooklist[x][next].sq;
    do
    {
        if (mask[sq] & bit_all)
        {
            next = rooklist[x][next].next;
			if(next==-1)break;
        }
        else
        {
            next++;
            AddRook(x, sq);
        }
        sq = rooklist[x][next].sq;
    } while (sq > -1);
}

void BishopMoves(const int x)
{
    int next = 0;
    int sq = bishoplist[x][next].sq;
    do
    {
        if (mask[sq] & bit_all)
        {
            next = bishoplist[x][next].next;
			if(next==-1)break;
        }
        else
        {
            next++;
            AddBishop(x, sq);
        }
        sq = bishoplist[x][next].sq;
    } while (sq > -1);
}

void QueenMoves(const int x)
{
    int next = 0;
    int sq = queenlist[x][next].sq;
    do
    {
        if (mask[sq] & bit_all)
        {
            next = queenlist[x][next].next;
			if(next==-1)break;
        }
        else
        {
            next++;
            AddQueen(x, sq);
        }
        sq = queenlist[x][next].sq;
    } while (sq > -1);
}


