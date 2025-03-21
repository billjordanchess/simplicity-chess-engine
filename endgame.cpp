#include "globals.h"

#define HALFKENDMOB 4

#define DRAWPLUS 1
#define DRAWMINUS -1
#define ROOKBEHIND 15

#define RANK_6 5
#define RANK_7 6

#define QUEENING 650

int endgame_score[MAX_PLY][2];

int endmatrix[10][3][10][3];

int RookMoveCount(const int x, const BITBOARD bp);

int PawnEndingScore(const int s, const int xs);
int SquarePawn(const int s, const int, const int p);
int OpposedPawns(const int s, const int);
int MostAdvancedPawn(const int s, const int);
int LeastDifference(const int s, const int xs);

int OppositeBishops();

int DrawnEnding(const int, const int);
int SinglePawnEnding(const int s, const int xs);

int EvalPawnless();

int EvalEndgame()
{
	int score[2];
	score[0] = centi_pawns[pawn_mat[0]] + centi_pieces[piece_mat[0]] + table_score[0] + king_endgame_score[pieces[0][K][0]];
	score[1] = centi_pawns[pawn_mat[1]] + centi_pieces[piece_mat[1]] + table_score[1] + king_endgame_score[pieces[1][K][0]];
	drawn = 0;//19/1/23

	if (LookUpPawn())
	{
		score[0] += GetHashPawn(0);
		score[1] += GetHashPawn(1);
		passed_list[0] = GetHashPassed(0);
		passed_list[1] = GetHashPassed(1);
		bit_pawnattacks[0] = GetHashPawnAttacks(0);
		bit_pawnattacks[1] = GetHashPawnAttacks(1);
	}
	else
	{
		passed_list[0] = 0;
		passed_list[1] = 0;
		int s1 = EvalPawns(0, 1);
		int s2 = EvalPawns(1, 0);
		score[0] += s1;
		score[1] += s2;
		AddPawnHash(s1, s2, passed_list[0], passed_list[1]);
		bit_pawnattacks[0] = (bit_pieces[0][P] & not_a_file) << 7;
		bit_pawnattacks[0] |= (bit_pieces[0][P] & not_h_file) << 9;
		AddPawnAttackHash(0, bit_pawnattacks[0]);

		bit_pawnattacks[1] = (bit_pieces[1][P] & not_a_file) >> 9;
		bit_pawnattacks[1] |= (bit_pieces[1][P] & not_h_file) >> 7;
		AddPawnAttackHash(1, bit_pawnattacks[1]);
	}

	if (piece_mat[xside] == 0)
	{
		if (piece_mat[side] == 0)
		{
			int sc = 0;
			if (pawn_mat[0] == 0)
			{
				sc = SinglePawnEnding(0, 1);
			}
			else if (pawn_mat[1] == 0)
			{
				sc = SinglePawnEnding(1, 0);
			}
			if (drawn == 1)
				return 1;
			if (sc != 0)
				return sc;

			score[0] += PawnEndingScore(0, 1);
			score[1] += PawnEndingScore(1, 0);
			score[0] += centi_pawns[pawn_mat[0]] / 2;//24
			score[1] += centi_pawns[pawn_mat[1]] / 2;//24
			return score[side] - score[xside];
		}
	}
	if (bit_pieces[side][Q] && (piece_mat[xside] < startmat[xside] || piece_mat[side] > startmat[side])
		&& !(bit_pieces[xside][0] & (mask_ranks[xside][5] | mask_ranks[xside][6])))
	{
		return 9050;
	}
	if (bit_pieces[side][R] && piece_mat[xside] < startmat[xside] && pawn_mat[side] >= pawn_mat[xside] &&
		!(bit_pieces[xside][0] & (mask_ranks[xside][Q] | mask_ranks[xside][5] | mask_ranks[xside][6])))
	{
		return 9001;
	}
	//*
	//static score
	score[0] += centi_pawns[pawn_mat[0]] / 4;//24
	score[1] += centi_pawns[pawn_mat[1]] / 4;//24

	if (pawn_mat[0] > 0 && piece_mat[0] > 0 && piece_mat[0] + pawn_mat[0] > piece_mat[1] + pawn_mat[1])
	{
		score[0] += (piece_mat[0] * 10) / (piece_mat[1] + 1);
		if (mask_abc & bit_pieces[0][P] && mask_def & bit_pieces[0][P])
		{
			score[0] += 10;
		}
	}
	else if (pawn_mat[1] > 0 && piece_mat[1] > 0 && piece_mat[1] + pawn_mat[1] > piece_mat[0] + pawn_mat[0])
	{
		score[1] += (piece_mat[1] * 10) / (piece_mat[0] + 1);
		if (mask_abc & bit_pieces[1][P] && mask_def & bit_pieces[1][P])
		{
			score[1] += 10;
		}
	}
	//*/
	if (OppositeBishops() > 0)
	{
		score[0] -= (centi_pawns[pawn_mat[0]] >> 1);
		score[1] -= (centi_pawns[pawn_mat[1]] >> 1);
	}
	int x, sq;
	for (int s = 0; s < 2; s++)
	{
		if (pawn_mat[s] == 0 && piece_mat[s] < BBVAL && pawn_mat[!s] == PVAL &&
			(piece_mat[!s] < BBVAL || piece_mat[!s] == QVAL)
			&& DrawnEnding(s, !s) && ply>1)
		{
			drawn = 1;
			return 0;
		}
		if (passed_list[s])
			score[s] += PassedPawnScore(s, !s);
		if (pawn_mat[s] == 0 && piece_mat[s] == BVAL && score[s] > score[!s])//
		{
			if (score[s] >= 0)
				return 0;
		}
		for (x = 0; x < total[s][R]; x++)
		{
			sq = pieces[s][R][x];
			score[s] += RookMoveCount(sq, bit_pawnattacks[!s]);
		}
	}
	return score[side] - score[xside];
}

int SinglePawnEnding(const int s, const int xs)
{
	if (pawn_mat[xs] == PVAL)
	{
		int pawn = NextBit(bit_pieces[xs][P]);//moved from first line 17/1/23
		int xking = pieces[xs][K][0];
		int king = pieces[s][K][0];
		if (b[pawnplus[s][king]] == 0 ||
			b[pawndouble[s][king]] == 0)
			if (rank[s][king] != 0 || bit_pieces[xs][P] & mask_rookfiles)
			{
				drawn = 1;
				return 1;
			}
		if (bit_pieces[xs][K] & mask_rookfiles &&
			col[xking] == col[pawn] &&
			difference[king][xking] == 2 && rank[xs][king] >= rank[xs][xking] &&
			rank[xs][xking] > rank[xs][pawn])
		{
			drawn = 1;
			return 1;
		}
		if ((col[pawn] == 0 || col[pawn] == 7)
			&& abs(col[king] - col[pawn]) < 2 &&
			rank[s][king] < rank[s][pawn])
		{
			drawn = 1;
			return 1;
		}
		int p2 = pawndouble[xs][pawn];

		if (col[pawn] == 0 || col[pawn] == 7)//could be in main loop
		{
			if (rank[xs][pawn] > 3 && difference[pawn][pieces[s][K][0]] > 1)
			{
				if (col[pawn] == 0 && pieces[xs][K][0] == squares[xs][B7])
				{
					return -600;
				}
				if (col[pawn] == 7 && pieces[xs][K][0] == squares[xs][G7])
				{
					return -600;
				}
			}
		}
		if ((p2 == pieces[xs][K][0] || p2 - 1 == pieces[xs][K][0] || p2 + 1 == pieces[xs][K][0])
			&& difference[pawn][pieces[s][K][0]] > 1)
		{
			return -600;
		}
		if (s == side && !(mask_squarepawn[s][pawn] & bit_pieces[xs][K]))
			return -600;
	}
	else
	{
		//doubled rook pawns
		if (!(~mask_files[0] & bit_pieces[xs][P]) && !(~mask_files[7] & bit_pieces[xs][P]))
		{
			int xking = pieces[xs][K][0];
			int king = pieces[s][K][0];
			if (!(mask_passed[xs][king] & bit_pieces[xs][P]) && !(mask_passed[xs][xking] & bit_pieces[xs][P]))
			{
				if ((b[pawnplus[s][king]] == PAWN ||
					b[pawndouble[s][king]] == PAWN))
				{
					drawn = 1;
					return 1;
				}
				if (bit_pieces[xs][K] & mask_rookfiles && rank[s][king] <= rank[s][xking] && difference[king][xking] < 3)
				{
					drawn = 1;
					return 1;
				}
			}
		}
	}
	return 0;
}

int DrawnEnding(const int s, const int xs)
{
	int pawn = 0;
	int xking = pieces[xs][K][0];
	int king = pieces[s][K][0];

	if (pawn_mat[s] == PVAL && piece_mat[s] == 0 && pawn_mat[xs] == 0)
	{
		pawn = NextBit(bit_pieces[s][P]);

		//knight v p
		if (total[xs][N] == 1)
		{
			int pp = NextBit(bit_pieces[xs][P]);
			if (!(mask[pp] & mask_corner) && col[pp] == col[pawn] &&
				rank[xs][pp] < rank[xs][pawn])
			{
				return 1;
			}
			return 0;
		}
		//bishop v p
		if (total[xs][B] == 1)
		{
			if ((bit_bishopmoves[pawnplus[s][pawn]] & bit_pieces[xs][B] || mask[pawnplus[s][pawn]] & bit_units[xs]) &&
				!(bit_kingmoves[king] & bit_pieces[xs][B]))
			{
				return 1;
			}

			return 0;
		}
		//rook v p
		if (total[xs][R] == 1)
		{
			int dest = lastsquare[s][pawn];
			int dist = rank[xs][pawn];
			if (difference[pieces[s][K][0]][pawn] == 1 && pieces[s][K][0] != dest &&
				difference[pieces[s][K][0]][dest] == 1 &&
				difference[pieces[xs][K][0]][dest] >= dist + difference[pieces[s][K][0]][dest] &&
				!(bit_kingmoves[pieces[s][K][0]] & bit_pieces[xs][R]))
			{
				return 1;
			}
			return 0;
		}
		//queen v p
		if (piece_mat[xs] == QVAL)
		{
			if ((col[pawn] == 0 || col[pawn] == 7) && rank[s][pawn] == 6 &&
				difference[king][pawnplus[s][pawn]] < 2 && !(bit_pieces[xs][Q] & bit_kingmoves[king]) &&
				(abs(col[xking] - col[pawn]) > 4 || rank[xs][xking] > 4))
			{
				return 1;
			}
			if (((col[pawn] == 2 && (col[king] == 0 || col[king] == 1))
				|| (col[pawn] == 5 && (col[king] == 6 || col[king] == 7))) &&
				rank[s][pawn] == RANK_7 &&
				difference[king][pawnplus[s][pawn]] < 2 && !(bit_pieces[xs][Q] & bit_kingmoves[king]) &&
				(abs(col[xking] - col[pawn]) > 4 || rank[xs][xking] > 4))
			{
				return 1;
			}
			return 0;
		}
		return 0;
	}

	//phildor position
	if (piece_mat[xs] == RVAL && piece_mat[s] == RVAL &&
		pawn_mat[xs] == PVAL && pawn_mat[s] == 0)
	{
		int pawn = NextBit(bit_pieces[xs][P]);
		if (mask_passed[xs][pawn] & bit_pieces[s][K] &&
			!(bit_rookmoves[pieces[s][K][0]] & bit_pieces[xs][R]) &&
			!(bit_rookmoves[pieces[xs][K][0]] & bit_pieces[s][R]) &&
			!(bit_kingmoves[pieces[s][K][0]] & bit_pieces[xs][R]) &&
			!(bit_kingmoves[pieces[xs][K][0]] & bit_pieces[s][R]) &&
			rank[xs][pawn] < 5 && mask_rows[row[pawnplus[xs][pawn]]] & bit_pieces[s][R])
			return 1;
		return 0;
	}

	//  rook pawn of wrong colour  
	if (total[xs][B] == 1 &&
		!(bit_pieces[xs][P] & not_mask_rookfiles) &&
		piece_mat[s] <= BVAL)
	{
		int pawn = NextBit(bit_pieces[xs][P]);
		if (difference[king][lastsquare[xs][pawn]] < 2)
			if (colors[pieces[xs][B][0]] != colors[lastsquare[xs][pawn]])
			{
				return 1;
			}
	}
	return 0;
}

int PassedPawnScore(const int s, const int xs)
{
	int score = 0;
	int x;
	BITBOARD b1 = passed_list[s] & (mask_squareking[xs][pieces[xs][K][0]]);

	while (b1)
	{
		x = NextBit(b1);
		b1 &= not_mask[x];

		score += passed[s][x];//20/2/22

		if (rank[s][x] == RANK_7)// && s==xside)
		{
			if (bit_pawncaptures[xs][x] & passed_list[s])
			{
				if (total[xs][N] + total[xs][R] <= 1 && difference[pieces[xs][K][0]][x] > 2)
				{
					score += 140;
				}
			}
		}
		if (rank[s][x] == RANK_6)
		{
			if (bit_adjacent[x] & passed_list[s])
			{
				if (total[xs][N] + total[xs][R] <= 1 && difference[pieces[xs][K][0]][x] > 2)
					score += 70;
			}
		}
		if (piece_mat[xs] == 0 && !(mask_path[s][x] & bit_all))
		{
			score += SquarePawn(s, xs, x);
		}
		else if (total[xs][N] == 1)
		{
			score -= pawn_difference[pieces[s][K][0]][x] + pawn_difference[pieces[xs][K][0]][x];
		}
	}
	return score;
}

int OppositeBishops()
{
	if (total[0][B] == 1 && total[1][B] == 1)
	{
		if (colors[pieces[0][B][0]] != colors[pieces[1][B][0]])
			return 1;
	}
	return 0;
}

int PawnEndingScore(const int s, const int xs)
{
	int x;
	int a;
	int score = 0;
	BITBOARD b1 = passed_list[s];

	while (b1)
	{
		x = NextBit(b1);
		b1 &= not_mask[x];
		if ((s == side && !(mask_squarepawn[s][x] & bit_pieces[xs][K]) ||
			(s == xside && !(mask_squarepawn[s][x + pawnplus[xs][x]] & bit_pieces[xs][K]))))
		{
			score += SquarePawn(s, xs, x);
		}
		else if (!(mask_path[s][x] & ~bit_kingmoves[pieces[s][K][0]]) &&
			!(bit_kingmoves[pieces[xs][K][0]] & mask[x] && !(bit_kingmoves[pieces[s][K][0]] & mask[x])))
		{
			score += SquarePawn(s, xs, x);
		}
		else
			if (!(mask_path[s][pieces[s][K][0]] & bit_pawnattacks[xs]) && rank[s][pieces[s][K][0]] >= rank[s][pieces[xs][K][0]] &&
				difference[pieces[s][K][0]][x] == 1 && col[pieces[s][K][0]] != col[x])
			{
				score += SquarePawn(s, xs, x);
			}
			else if (!(mask_rookfiles & mask[x]))
			{
				if (mask_path[s][x] & bit_pieces[s][K] && rank[s][pieces[s][K][0]] == rank[s][pieces[xs][K][0]])
				{
					score += SquarePawn(s, xs, x);
				}
				else if ((pawndouble[s][x] == pieces[s][K][0] || pawndouble[s][x] == pieces[s][K][0] - 1 ||
					pawndouble[s][x] == pieces[s][K][0] + 1) && difference[pieces[xs][K][0]][x] > 1)
				{
					score += SquarePawn(s, xs, x);
				}
				else if (rank[s][x] == 4)
				{
					if (rank[s][pieces[s][K][0]] == 5 && difference[pieces[s][K][0]][x] == 1)
					{
						score += SquarePawn(s, xs, x);
					}
				}
				else if (rank[s][x] == RANK_7)
				{
					if (rank[s][pieces[s][K][0]] > 5 && difference[pieces[s][K][0]][x] == 1)
					{
						score += SquarePawn(s, xs, x);
					}
				}
			}
		//supported passed pawn
		if (rank[s][x] > 2)
		{
			if (bit_left[xs][x] & bit_pieces[s][P])
			{
				a = pawnleft[xs][x];
				if ((mask_passed[s][a] & not_mask[x - 1] & bit_pieces[xs][P]) == 0)
				{
					score += 50;
				}
				else
				{
					if (bit_right[xs][x] & bit_pieces[s][P])
					{
						a = pawnright[xs][x];
						if ((mask_passed[s][a] & not_mask[x + 1] & bit_pieces[xs][P]) == 0)
						{
							score += 50;
						}
					}
				}

			}
		}
		//outside passed pawn
		if (col[x] == 0 || (col[x] == 1 && (mask_files[0] & bit_pieces[xs][P]) == 0) &&
			(bit_pieces[s][P] & mask_def))
		{
			score += 20;
		}
		if (col[x] == 7 || (col[x] == 6 && (mask_files[7] & bit_pieces[xs][P]) == 0) &&
			(bit_pieces[s][P] & mask_abc))
		{
			score += 20;
		}
	}
	//King attack pawn
	if (bit_kingmoves[pieces[s][K][0]] & bit_pieces[xs][P] & ~(bit_pawnattacks[xs]))
	{
		score += 25;
	}
	return score;
}

int SquarePawn(const int s, const int xs, const int p)
{
	if (!pawn_mat[xs])
	{
		return QUEENING - ply;
	}
	int path = rank[xs][p];

	if (mask_path[s][p] & bit_pieces[s][K])
	{
		path++;
	}

	int path2;
	if (OpposedPawns(xs, s) == 1 || BlockedPawns(s, xs) == 1)
	{
		path2 = LeastDifference(s, xs);
	}
	else
		path2 = MostAdvancedPawn(xs, s);

	if (s == xside)
		path2--;

	if (path < path2)
	{
		return QUEENING - ply;
	}
	//queen with check
	if (path == path2)
	{
		int target = lastsquare[s][p];
		if (mask_ranks[s][7] & bit_pieces[xs][K] && !(bit_between[target][pieces[xs][K][0]] & bit_all))
		{
			return QUEENING - ply;
		}
	}
	return 0;
}

int LeastDifference(const int s, const int xs)
{
	BITBOARD b1 = bit_pieces[s][P] & ~passed_list[s];
	int d = 100;
	int y = 0;
	int x;

	while (b1)
	{
		x = NextBit(b1);
		b1 &= not_mask[x];
		y = difference[pieces[xs][K][0]][x] + 2 + rank[s][x];
		if (y < d)
			d = y;
	}
	return d;
}

int MostAdvancedPawn(const int s, const int xs)
{
	BITBOARD b1 = bit_pieces[s][P];
	BITBOARD b2;
	int sq;
	int advanced = 8;
	int x, current;
	while (b1)
	{
		x = NextBit(b1);
		b1 &= not_mask[x];
		current = rank[xs][x];
		if (current == 6)
			current = 5;//double pawn move;
		if (mask_path[s][x] & bit_pieces[s][K])
			current++;
		b2 = mask_path[s][x] & bit_pieces[xs][P];
		if (b2)
		{
			sq = NextBit(b2);
			current += difference[pieces[s][K][0]][sq] + 1;
		}
		if (current < advanced && !(mask_squarepawn[s][x] & bit_pieces[xs][K]))
			advanced = current;
	}
	return advanced;
}

int BlockedPawns(const int s, const int xs)
{
	if ((bit_pawnattacks[s] & bit_units[xs]) != 0)
	{
		return 0;
	}
	if (s == 0 && ((bit_pieces[s][P] << 8) & bit_pieces[xs][P]) == bit_pieces[xs][P])
	{
		return 1;
	}
	if (s == 1 && ((bit_pieces[s][P] >> 8) & bit_pieces[xs][P]) == bit_pieces[xs][P])
	{
		return 1;
	}
	return 0;
}

int OpposedPawns(const int s, const int xs)
{
	if ((bit_pawnattacks[s] & bit_units[xs]) != 0)
	{
		return 0;
	}

	for (int x = 0; x < 8; x++)
	{
		if (bit_pieces[s][P] & mask_files[x] && !(bit_pieces[xs][P] & mask_files[x]))
			return 0;
	}
	int count = 0;
	for (int x = 0; x < 8; x++)
	{
		if (!((bit_pieces[s][P] | bit_pieces[xs][P]) & mask_files[x]))
		{
			count = 0;
			continue;
		}
		if ((bit_pieces[s][P] << 8 | bit_pieces[s][P] >> 8) & mask_files[x] & bit_pieces[xs][P] ||
			(!(bit_pieces[s][P] & mask_files[x]) && bit_pieces[xs][P] & mask_files[x]))
		{
			count = 0;
			continue;
		}
		count++;
		if (count > 2)
			return 0;
	}
	if (count < 3)
		return 1;
	return 0;
}

int EvalPawnless()
{
	if (startmat[0] != piece_mat[0] || startmat[1] != piece_mat[1])
	{
		int result = endmatrix[piece_mat[side]][total[side][N]][piece_mat[xside]][total[xside][N]];
		if (result == DRAWN)
		{
			drawn = 1;
			return 0;
		}
		if (result != 0 && SafeKingMoves(side, xside) > 0 && (!(bit_kingmoves[pieces[side][K][0]] & bit_units[xside])))
		{
			return result;
		}
	}
	int score[2];
	score[0] = centi_pieces[piece_mat[0]] + table_score[0];
	score[1] = centi_pieces[piece_mat[1]] + table_score[1];
	score[0] += KingPawnLess[pieces[0][K][0]];
	score[1] += KingPawnLess[pieces[1][K][0]];
	if (abs(piece_mat[0] - piece_mat[1]) < 5)
	{
		score[0] -= (centi_pieces[piece_mat[0]] >> 1);
		score[1] -= (centi_pieces[piece_mat[1]] >> 1);
	}
	if (piece_mat[side] > piece_mat[xside])
		score[side] += kingking[pieces[side][K][0]][pieces[xside][K][0]];
	else if (piece_mat[side] < piece_mat[xside])
		score[xside] += kingking[pieces[xside][K][0]][pieces[xside][K][0]];
	return score[side] - score[xside];
}


