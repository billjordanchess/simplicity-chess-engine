#include "globals.h"

#define HALFKENDMOB 4

#define BVAL 300
#define RVAL 500
#define QVAL 900
#define DRAWPLUS 1
#define DRAWMINUS -1
#define ROOKBEHIND 15

#define DRAWN 77

#define QUEENING 650

int endgame_score[MAX_PLY][2];

int endmatrix[10][3][10][3];

int RookMoveCount(int);

int PassedPawnScore2(const int s, const int xs);

int PawnEndingScore(const int s, const int xs);

int SquarePawn(const int s, const int, const int p);

int OpposedPawns(const int s, const int);

int OppositeBishops();

int DrawnEnding(const int, const int);
int SinglePawnEnding(const int s, const int xs);

int MostAdvancedPawn(const int s, const int);
int LeastDifference(const int s, const int xs);

int EvalEndgame()
{
	int score[2];
	score[0] = pawn_mat[0] + piece_mat[0] + table_score[0] + king_endgame_score[pieces[0][5][0]];
	score[1] = pawn_mat[1] + piece_mat[1] + table_score[1] + king_endgame_score[pieces[1][5][0]];

	if (LookUpPawn())
	{
		score[0] += GetHashPawn0();
		score[1] += GetHashPawn1();
		passed_list[0] = GetHashPassed0();
		passed_list[1] = GetHashPassed1();
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
		bit_pawnattacks[0] = (bit_pieces[0][0] & not_a_file) << 7;
		bit_pawnattacks[0] |= (bit_pieces[0][0] & not_h_file) << 9;
		AddPawnAttackHash(0, bit_pawnattacks[0]);

		bit_pawnattacks[1] = (bit_pieces[1][0] & not_a_file) >> 9;
		bit_pawnattacks[1] |= (bit_pieces[1][0] & not_h_file) >> 7;
		AddPawnAttackHash(1, bit_pawnattacks[1]);
	}

	if (piece_mat[0] == 0 && piece_mat[1] == 0)
	{
		if (pawn_mat[0] == 0)
		{
			return SinglePawnEnding(0, 1);
		}
		else if (pawn_mat[1] == 0)
		{
			return SinglePawnEnding(1, 0);
		}
		score[0] += PawnEndingScore(0, 1);
		score[1] += PawnEndingScore(1, 0);
		return score[side] - score[xside];
	}

	if (pawn_mat[0] == 0 && pawn_mat[1] == 0)
	{
		if (game_list[hply - 1].capture < 6)
		{
			int result = endmatrix[piece_mat[side] / 100][total[side][1]][piece_mat[xside] / 100][total[xside][1]];
			if (result == DRAWN)
			{
				drawn = 1;
				return 0;
			}
			if (result != 0 && SafeKingMoves(side, xside) > 0 && (!(bit_pieces[side][5] & bit_units[xside])))
			{   //add defended
				//z();

				return result;
			}
		}
		score[0] = piece_mat[0] + table_score[0];
		score[1] = piece_mat[1] + table_score[1];
		score[0] += KingPawnLess[pieces[0][5][0]];
		score[1] += KingPawnLess[pieces[1][5][0]];
		if (abs(piece_mat[0] - piece_mat[1]) < 500)
		{
			score[0] -= (piece_mat[0] >> 1);
			score[1] -= (piece_mat[1] >> 1);
		}
		return score[side] - score[xside];
	}
	int score2[2];
	score2[0] = 0;
	score2[1] = 0;
	if (game_list[hply - 1].capture == 6 && ply > 1)
	{
		score[0] += endgame_score[ply - 1][0];
		score[1] += endgame_score[ply - 1][1];
	}
	else
	{
		//static score
		if (pawn_mat[0] > 0 && piece_mat[0] > 0 && piece_mat[0] + pawn_mat[0] > piece_mat[1] + pawn_mat[1])
		{
			score2[0] += (piece_mat[0] * 10) / (piece_mat[1] + 100);
			if (mask_abc & bit_pieces[0][0] && mask_def & bit_pieces[0][0])
			{
				score2[0] += 10;
			}
		}
		else if (pawn_mat[1] > 0 && piece_mat[1] > 0 && piece_mat[1] + pawn_mat[1] > piece_mat[0] + pawn_mat[0])
		{
			score2[1] += (piece_mat[1] * 10) / (piece_mat[0] + 100);
			if (mask_abc & bit_pieces[1][0] && mask_def & bit_pieces[1][0])
			{
				score2[1] += 10;
			}
		}
	}
	if (pawn_mat[0] == 0 && abs(piece_mat[0] - piece_mat[1]) < 500)
		score2[0] -= (piece_mat[0] >> 1);
	else if (pawn_mat[1] == 0 && abs(piece_mat[0] - piece_mat[1]) < 500)
		score2[1] -= (piece_mat[1] >> 1);

	if (total[0][2] == 1 && total[1][2] == 1)
	{
		if (OppositeBishops() > 0)
		{
			score2[0] -= (pawn_mat[0] >> 1);
			score2[1] -= (pawn_mat[1] >> 1);
		}
		endgame_score[ply][0] = score2[0];
		endgame_score[ply][1] = score2[1];
		score[0] += score2[0];
		score[1] += score2[1];
	}

	int x, sq, pawn;
	for (int s = 0; s < 2; s++)
	{	
		if (passed_list[s])
			score[s] += PassedPawnScore(s, !s);
		if (pawn_mat[s] == 0 && piece_mat[s] < 600 && pawn_mat[!s] == 100 &&
			(piece_mat[!s] < 600 || piece_mat[!s] == 900)
			&& DrawnEnding(s, !s) && ply>1)
		{
			drawn = 1;
			return 0;
		}
		if (pawn_mat[s] == 100)
		{
		    pawn = pawnplus[s][NextBit(bit_pieces[!s][0])];

			score[0] -= pawn_difference[pieces[0][5][0]][pawn];
			score[1] -= pawn_difference[pieces[1][5][0]][pawn];
		}
		else if (pawn_mat[s] == 0 && piece_mat[s]==300)//
		{
			if (score[s] >= 0)
				return 0;
		}
		for (x = 0; x < total[s][3]; x++)
		{
			sq = pieces[s][3][x]; 
			score[s] += RookMoveCount(sq);
		}
	}
	return score[side] - score[xside];
}

int SinglePawnEnding(const int s, const int xs)
{	
	int pawn = NextBit(bit_pieces[xs][0]);
	if (pawn_mat[xs] == 100)
	{
		int xking = pieces[xs][5][0];
		int king = pieces[s][5][0];
		if (b[pawnplus[s][king]] == 0 ||
			b[pawndouble[s][king]] == 0)
			if (rank[s][king] != 0)
			{
				drawn = 1;
				return 1;
			}
		if (bit_pieces[xs][5] & mask_rookfiles &&
			col[xking] == col[pawn] &&
			difference[king][xking] == 2 && rank[xs][king] <= rank[xs][xking] &&
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
			if (rank[xs][pawn] > 3 && difference[pawn][pieces[s][5][0]] > 1)
			{
				if (col[pawn] == 0 && pieces[xs][5][0] == squares[xs][B7])
				{
					return -600;
				}
				if (col[pawn] == 7 && pieces[xs][5][0] == squares[xs][G7])
				{
					return -600;
				}
			}
		}
		else
			if ((p2 == pieces[xs][5][0] || p2 - 1 == pieces[xs][5][0] || p2 + 1 == pieces[xs][5][0])
				&& difference[pawn][pieces[s][5][0]] > 1)
			{
				return -600;
			}
	}

	//doubled rook pawns
	if (!(~mask_cols[pawn] & bit_pieces[xs][0]))
	{
		int xking = pieces[xs][5][0];
		int king = pieces[s][5][0];
		if (!(mask_passed[xs][king] & bit_pieces[xs][0]) && !(mask_passed[xs][xking] & bit_pieces[xs][0]))
		{
			if ((b[pawnplus[s][king]] == PAWN ||
				b[pawndouble[s][king]] == PAWN))
			{
				drawn = 1;
				return 1;
			}
			if (bit_pieces[xs][5] & mask_rookfiles && rank[s][king] <= rank[s][xking] && difference[king][xking] < 3)
			{
				drawn = 1;
				return 1;
			}
		}
	}
	return 0;
}

int DrawnEnding(const int s, const int xs)
{
	int pawn = 0;
	int xking = pieces[xs][5][0];
	int king = pieces[s][5][0];

	if (pawn_mat[s] == 100 && piece_mat[s] == 0 && pawn_mat[xs] == 0)
	{
		pawn = NextBit(bit_pieces[s][0]);

		//knight v p
		if (total[xs][1] == 1)
		{
			int pp = NextBit(bit_pieces[xs][0]);
			if (!(mask[pp] & mask_corner) && col[pp] == col[pawn] &&
				rank[xs][pp] < rank[xs][pawn])
			{
				return 1;
			}
			return 0;
		}
		//bishop v p
		if (total[xs][2] == 1)
		{
			if ((bit_bishopmoves[pawnplus[s][pawn]] & bit_pieces[xs][2] || mask[pawnplus[s][pawn]] & bit_units[xs]) &&
				!(bit_kingmoves[king] & bit_pieces[xs][2]))
			{
				return 1;
			}

			return 0;
		}
		//rook v p
		if (piece_mat[xs] == 500)
		{
			int dest = lastsquare[s][pawn];
			int dist = rank[xs][pawn];
			if (difference[pieces[s][5][0]][pawn] == 1 && pieces[s][5][0] != dest &&
				difference[pieces[s][5][0]][dest] == 1 &&
				difference[pieces[xs][5][0]][dest] >= dist + difference[pieces[s][5][0]][dest] &&
				!(bit_kingmoves[pieces[s][5][0]] & bit_pieces[xs][3]))//pawn attacks rook
			{
				return 1;
			}
			return 0;
		}
		//queen v p
		if (piece_mat[xs] == QVAL)
		{
			if ((col[pawn] == 0 || col[pawn] == 7) && rank[s][pawn] == 6 &&
				difference[king][pawnplus[s][pawn]] < 2 && !(bit_pieces[xs][4] & bit_kingmoves[king]) &&
				(abs(col[xking] - col[pawn]) > 4 || rank[xs][xking] > 4))
			{
				return 1;
			}
			if (((col[pawn] == 2 && (col[king] == 0 || col[king] == 1))
				|| (col[pawn] == 5 && (col[king] == 6 || col[king] == 7))) &&
				rank[s][pawn] == 6 &&
				difference[king][pawnplus[s][pawn]] < 2 && !(bit_pieces[xs][4] & bit_kingmoves[king]) &&
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
		pawn_mat[xs] == 100 && pawn_mat[s] == 0)
	{
		int pawn = NextBit(bit_pieces[xs][0]);
		if (mask_passed[xs][pawn] & bit_pieces[s][5] &&
			!(bit_rookmoves[pieces[s][5][0]] & bit_pieces[xs][3]) &&
			!(bit_rookmoves[pieces[xs][5][0]] & bit_pieces[s][3]) &&
			!(bit_kingmoves[pieces[s][5][0]] & bit_pieces[xs][3]) &&
			!(bit_kingmoves[pieces[xs][5][0]] & bit_pieces[s][3]) &&
			rank[xs][pawn] < 5 && mask_rows[row[pawnplus[xs][pawn]]] & bit_pieces[s][3])
			return 1; 
		return 0;
	}

	//  rook pawn of wrong colour  
	if (total[xs][2] == 1 && 
		!(bit_pieces[xs][0] & not_mask_rookfiles) &&
		piece_mat[s] <= BVAL)
	{
		int pawn = NextBit(bit_pieces[xs][0]);
		if (difference[king][lastsquare[xs][pawn]] < 2)
			if (colors[pieces[xs][2][0]] != colors[lastsquare[xs][pawn]])
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
	BITBOARD b1 = passed_list[s] & (mask_squareking[xs][pieces[xs][5][0]]);

	while (b1)
	{
		x = NextBit(b1);
		b1 &= not_mask[x];

		if (rank[s][x] == 6)
		{
			if (Attack(xs, pawnplus[s][x]) == 0 && LineAttack(xs, x) == 0 && !(mask_ranks[xs][6] & bit_pieces[xs][0]) &&
				b[pawnplus[s][x]] == 6)//add r+q behind + pinned
				return 800 - ply;
			if (bit_pawncaptures[xs][x] & passed_list[s])
			{
				if (piece_mat[xs] == 500 && difference[pieces[xs][5][0]][x] > 2)
					 score += 140;
			}
		}

		score += passed[s][x] / 2;
		if (piece_mat[xs] == 0 && !(mask_path[s][x] & bit_all))
		{
			score += SquarePawn(s, xs, x);
		}
		else if (total[xs][1] == 1)
		{
			score += passed[s][x] / 2;
		}
	}
	return score;
}

int PassedPawnScore2(const int s, const int xs)
{
	int score = 0;
	int x;

	BITBOARD b1 = passed_list[s] & (mask_squareking[xs][pieces[xs][5][0]]);

	if (b1 && piece_mat[xs] == 0)
	{
		//if ((b1 & b1 - 1) > 0 passed_list[s] & (mask_squareking[xs][NextBit(b1))
		while (b1)
		{
			x = NextBit(b1);
			b1 &= not_mask[x];
			score += SquarePawn(s, xs, x);
			if (rank[s][x] == 6 && Attack(s, pawnplus[s][x]))
			{   
				score += 800 - ply;
			}
		}
		return score;
	}

	while (b1)
	{
		x = NextBit(b1);
		b1 &= not_mask[x];
		score += passed[s][x] >> 1;
		if (total[xs][1] == 1)
		{
			score += passed[s][x] >> 1;
		}
		if (piece_mat[xs] == 500)
		{
			if (rank[s][x] == 5)
			{
				if (passed_list[s] & bit_adjacent[x])
				{
					if (difference[pieces[xs][5][0]][x] > 2)
						score += 70;
				}
				continue;
			}
			if (rank[s][x] == 6)
			{
				if (bit_pawncaptures[xs][x] & passed_list[s])
				{
					if (difference[pieces[xs][5][0]][x] > 2)
						score += 140;
				}
			}
		}
		if (rank[s][x] == 6)
		{
			if (Attack(xs, x) == 0 && Attack(xs, pawnplus[s][x]) == 0)
			{
				if (!(mask_ranks[xs][6] & bit_pieces[xs][0]) &&
					b[pawnplus[s][x]] == 6 && piece_mat[xs] > 0 && piece_mat[xs] < 600)
				{
					if (total[xs][2] == 1)
					{
						int bish = pieces[xs][2][0];
						if (colors[x] == colors[bish])
						{
							score += 800 - ply;
						}
						continue;
					}
					if (total[xs][1] == 1)
					{
						if (colors[x] == colors[pieces[xs][1][0]])
						{
							score += 800 - ply;
						}
						continue;
					}
					if (Attack(s, pawnplus[s][x]))
					{   
						score += piece_mat[xs] - 120;
					}
				}
			}
		}
	}
	return score;
}
int OppositeBishops()
{
	if (bit_color[1] & bit_pieces[0][2] && bit_color[0] & bit_pieces[1][2])
		return 1;
	if (bit_color[0] & bit_pieces[0][2] && bit_color[1] & bit_pieces[1][2])
		return 1;

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

		if (!(mask_squarepawn[s][x] & bit_pieces[xs][5]))
		{
			score += SquarePawn(s, xs, x);
		}

		else if (!(mask_path[s][x] & ~bit_kingmoves[pieces[s][5][0]]) &&
			!(bit_kingmoves[pieces[xs][5][0]] & mask[x] && !(bit_kingmoves[pieces[s][5][0]] & mask[x])))
		{
			score += SquarePawn(s, xs, x);
		}
		else if (!(mask_path[s][pieces[s][5][0]] & bit_pawnattacks[xs]) && rank[s][pieces[s][5][0]] >= rank[s][pieces[xs][5][0]] &&
			difference[pieces[s][5][0]][x] == 1 && col[pieces[s][5][0]] != col[x])
		{
			score += SquarePawn(s, xs, x);
		}
		else if (!(mask_rookfiles & mask[x]))
		{
			if (mask_path[s][x] & bit_pieces[s][5] && rank[s][pieces[s][5][0]] == rank[s][pieces[xs][5][0]])
			{
				score += SquarePawn(s, xs, x);
			}
			else if ((pawndouble[s][x] == pieces[s][5][0] || pawndouble[s][x] == pieces[s][5][0] - 1 ||
				pawndouble[s][x] == pieces[s][5][0] + 1) && difference[pieces[xs][5][0]][x] > 1)
			{
				score += SquarePawn(s, xs, x);
			}
			else if (rank[s][x] == 4)
			{
				if (rank[s][pieces[s][5][0]] == 5 && difference[pieces[s][5][0]][x] == 1)
				{
					score += SquarePawn(s, xs, x);
				}
			}
			else if (rank[s][x] == 6)
			{
				if (rank[s][pieces[s][5][0]] > 5 && difference[pieces[s][5][0]][x] == 1)
				{
					score += SquarePawn(s, xs, x);
				}
			}
		}
		//supported passed pawn
		if (rank[s][x] > 2)
		{
			if (bit_left[xs][x] & bit_pieces[s][0])
			{
				a = pawnleft[xs][x];
				if ((mask_passed[s][a] & not_mask[x - 1] & bit_pieces[xs][0]) == 0)
				{
					score += 50;
				}
				else
				{
					if (bit_right[xs][x] & bit_pieces[s][0])
					{
						a = pawnright[xs][x];
						if ((mask_passed[s][a] & not_mask[x + 1] & bit_pieces[xs][0]) == 0)
						{
							score += 50;
						}
					}
				}

			}
		}
		//outside passed pawn
		if (col[x] == 0 || (col[x] == 1 && (mask_files[0] & bit_pieces[xs][0]) == 0) &&
			(bit_pieces[s][0] & mask_def))
		{
			score += 20;   
		}
		if (col[x] == 7 || (col[x] == 6 && (mask_files[7] & bit_pieces[xs][0]) == 0) &&
			(bit_pieces[s][0] & mask_abc))
		{
			score += 20;    
		}
	}
	//King attack pawn
	if (bit_kingmoves[pieces[side][5][0]] & bit_pieces[xs][0] & ~(bit_pawnattacks[xs]))
		score += 20;
	return score;
}

int SquarePawn(const int s, const int xs, const int p)
{
	if (!pawn_mat[xs])
	{
		return QUEENING - ply;
	}
	int path = rank[xs][p];

	if (mask_path[s][p] & bit_pieces[s][5])
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
	if (path == path2)//queen with check
	{
		int target = lastsquare[s][p];
		if (mask_ranks[s][7] & bit_pieces[xs][5] && !(bit_between[target][pieces[xs][5][0]] & bit_all))
		{
			return QUEENING - ply;
		}
	}
	return 0;
}

int LeastDifference(const int s, const int xs)
{
	BITBOARD b1 = bit_pieces[s][0] & ~passed_list[s];
	int d = 100;
	int y = 0;
	int x;

	while (b1)
	{
		x = NextBit(b1);
		b1 &= not_mask[x];
		y = difference[pieces[xs][5][0]][x] + 2 + rank[s][x];
		if (y < d)
			d = y;
	}
	return d;
}

int MostAdvancedPawn(const int s, const int xs)
{
	BITBOARD b1 = bit_pieces[s][0];
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
		if (mask_path[s][x] & bit_pieces[s][5])
			current++;
		b2 = mask_path[s][x] & bit_pieces[xs][0];
		if (b2)
		{			
			sq = NextBit(b2);
			current += difference[pieces[s][5][0]][sq] + 1;
		}
		if (current < advanced && !(mask_squarepawn[s][x] & bit_pieces[xs][5]))
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
	if (s == 0 && ((bit_pieces[s][0] << 8) & bit_pieces[xs][0]) == bit_pieces[xs][0])
	{
		return 1;
	}
	if (s == 1 && ((bit_pieces[s][0] >> 8) & bit_pieces[xs][0]) == bit_pieces[xs][0])
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
	int x;

	for (x = 0; x < 8; x++)
	{
		if (bit_pieces[s][0] & mask_files[x] && !(bit_pieces[xs][0] & mask_files[x]))
			return 0;
	}
	int count = 0;
	for (x = 0; x < 8; x++)
	{
		if (!((bit_pieces[s][0] | bit_pieces[xs][0]) & mask_files[x]))
		{
			count = 0;
			continue;
		}
		if ((bit_pieces[s][0] << 8 | bit_pieces[s][0] >> 8) & mask_files[x] & bit_pieces[xs][0] ||
			(!(bit_pieces[s][0] & mask_files[x]) && bit_pieces[xs][0] & mask_files[x]))
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


