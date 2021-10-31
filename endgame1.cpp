#include "globals.h"

#define HALFKENDMOB 4

#define BVAL 300
#define RVAL 500
#define QVAL 900
#define DRAWPLUS 1
#define DRAWMINUS -1
#define ROOKBEHIND 15

#define QUEENING 650//680

int endgame_score[MAX_PLY][2];

int endmatrix[10][3][10][3];

int RookMoveCount(int);

int EndgameTacticalScore2(const int s, const int xs);

int AnyAttacks(const int s, const int xs);

int LostRelative(const int s, const int xs);
int LostPawnEnding(const int s, const int xs);

int PawnEndingScore(const int s, const int xs);

void Alg(int a, int b);
void ShowAll2();

int CanStopOne(const int s, const int xs);

int SquarePawn(const int s, const int, const int p);

int OpposedPawns(const int s, const int);
int OpposedPawns2(const int s, const int xs);

int EndgameScore(const int, const int);

int PawnEndings(const int, const int);
int OppositeBishops();

int DrawnEnding(const int, const int);
int DrawnEnding2(const int s, const int xs);
int DrawnPawnEnding(const int s, const int xs);
int LostEnding(const int, const int);

int MostAdvancedPawn(const int s, const int);
int LeastDifference(const int s, const int xs);

int EvalEndgame()
{
	int score[2];
	score[0] = pawn_mat[0] + piece_mat[0] + table_score[0] + king_endgame_score[pieces[0][5][0]];
	score[1] = pawn_mat[1] + piece_mat[1] + table_score[1] + king_endgame_score[pieces[1][5][0]];

	//both
	if (LookUpPawn())
	{
		score[0] += GetHashPawn0();
		score[1] += GetHashPawn1();
		passed_list[0] = GetHashPassed0();
		passed_list[1] = GetHashPassed1();
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
	}
	if (piece_mat[0] == 0 && piece_mat[1] == 0)
	{
		if (DrawnPawnEnding(0, 1) > 0 || DrawnPawnEnding(1, 0) > 0)
		{
			drawn = 1;
			return 0;
		}

		score[side] += PawnEndings(side, xside);
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
		if (pawn_mat[0] == 0 && pawn_mat[1] == 0)
		{
			if (game_list[hply - 1].capture < 6)
			{
				int result = endmatrix[piece_mat[side] / 100][total[side][1]][piece_mat[xside] / 100][total[xside][1]];
				if (result == 77)
				{
					drawn = 1;
					return 0;//21/3/13
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

		if (pawn_mat[1] > 0 && piece_mat[1] > 0 && piece_mat[0] + pawn_mat[0] > piece_mat[1] + pawn_mat[1])
		{
			score2[0] += (piece_mat[0] * 10) / piece_mat[1];
			if (mask_abc & bit_pieces[0][0] && mask_def & bit_pieces[0][0])
			{
				score2[0] += 10;
			}
		}
		else if (pawn_mat[0] > 0 && piece_mat[0] > 0 && piece_mat[0] + pawn_mat[0] < piece_mat[1] + pawn_mat[1])
		{
			score2[1] += (piece_mat[1] * 10) / piece_mat[0];
			if (mask_abc & bit_pieces[1][0] && mask_def & bit_pieces[1][0])
			{
				score2[1] += 10;
			}
		}
	}
	if (pawn_mat[0] == 0 && abs(piece_mat[0] - piece_mat[1]) < 500)
		score2[0] -= (piece_mat[0] >> 1);
	if (pawn_mat[1] == 0 && abs(piece_mat[0] - piece_mat[1]) < 500)
		score2[1] -= (piece_mat[1] >> 1);

	if (total[0][2] == 1 && total[1][2] == 1)
	{
		if (OppositeBishops() > 0)
		{
			score2[0] -= (pawn_mat[0] >> 1);
			score2[1] -= (pawn_mat[1] >> 1);
		}
		//*/
		endgame_score[ply][0] = score2[0];
		endgame_score[ply][1] = score2[1];
		score[0] += score2[0];
		score[1] += score2[1];
	}
	//dynamic score
	if (passed_list[side])
		score[side] += EndgameTacticalScore(side, xside);

	if (passed_list[xside])
		score[xside] += EndgameTacticalScore2(xside, side);

	if (pawn_mat[1] == 100)
	{
		int pawn = pawnplus[1][NextBit(bit_pieces[1][0])];

		score[0] -= pawn_difference[pieces[0][5][0]][pawn];
		score[1] -= pawn_difference[pieces[0][5][0]][pawn];
	}
	if (pawn_mat[0] == 100) //.65
	{
		int pawn = pawnplus[0][NextBit(bit_pieces[1][0])];

		score[0] -= pawn_difference[pieces[0][5][0]][pawn];
		score[1] -= pawn_difference[pieces[1][5][0]][pawn];
	}

	if (pawn_mat[0] == 0 && piece_mat[0] < 600 && pawn_mat[1] == 100 &&
		(piece_mat[1] < 600 || piece_mat[1] == 900)
		&& DrawnEnding(0, 1) && ply>1)
	{
		//z();
		drawn = 1;
		return 0;//21/3/13
	}
	else
		if (pawn_mat[1] == 0 && piece_mat[1] < 600 && pawn_mat[0] == 100 &&
			(piece_mat[0] < 600 || piece_mat[0] == 900)
			&& DrawnEnding(1, 0) && ply>1)
		{
			//z();
			drawn = 1;
			return 0;//21/3/13
		}

	int x, sq;
	for (x = 0; x < total[0][3]; x++)
	{
		sq = pieces[0][3][x];
		score[0] += RookMoveCount(sq);
	}
	for (x = 0; x < total[1][3]; x++)
	{
		sq = pieces[1][3][x];
		score[1] += RookMoveCount(sq);
	}
	//end dynamic

	//*/19/1/14

	//*/

	//piece + pawn versus piece .65


	return score[side] - score[xside];
}

int Ratio(int s1, int s2)
{
	int percent = (s1 * 500) / s2;
	if (s1 > s2)
		percent += 500;
	return percent;
}

int DrawnPawnEnding(const int s, const int xs)
{
	//z();
	if (pawn_mat[s] == 0 && pawn_mat[xs] == 100)//?? && ply>1)
	{
		int xking = pieces[xs][5][0];
		int king = pieces[s][5][0];
		if (b[pawnplus[s][king]] == 0 ||
			b[pawndouble[s][king]] == 0)
			if (rank[s][king] != 0)
			{
				return 1;
			}
		int pawn = NextBit(bit_pieces[xs][0]);
		if (bit_pieces[xs][5] & mask_rookfiles &&
			col[xking] == col[pawn] &&
			difference[king][xking] == 2 && rank[xs][king] <= rank[xs][xking] &&
			rank[xs][xking] > rank[xs][pawn])
		{
			return 1;
		}
		if ((col[pawn] == 0 || col[pawn] == 7)
			&& abs(col[king] - col[pawn]) < 2 &&
			rank[s][king] < rank[s][pawn])
		{
			return 1;
		}
	}

	//doubled rook pawns
	if (pawn_mat[s] == 0 && !(not_mask_rookfiles & bit_pieces[xs][0]))//to add - not on diff files
	{
		int xking = pieces[xs][5][0];
		int king = pieces[s][5][0];
		if (!(mask_passed[xs][king] & bit_pieces[xs][0]) && !(mask_passed[xs][xking] & bit_pieces[xs][0]))
		{
			if ((b[pawnplus[s][king]] == PAWN ||
				b[pawndouble[s][king]] == PAWN))
			{
				//z();
				return 1;
			}
			if (bit_pieces[xs][5] & mask_rookfiles && rank[s][king] <= rank[s][xking] && difference[king][xking] < 3)
			{
				//z();
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
				return 1;//.69
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
		return 0;//why here? 16/11/12
	}

	//phildor position
	if (piece_mat[xs] == RVAL && piece_mat[s] == RVAL &&
		pawn_mat[xs] == 100 && pawn_mat[s] == 0 && !AnyAttacks(xs, s))
	{
		int pawn = NextBit(bit_pieces[xs][0]);
		if (mask_passed[xs][pawn] & bit_pieces[s][5] &&
			!(bit_rookmoves[pieces[s][5][0]] & bit_pieces[xs][3]) &&
			!(bit_rookmoves[pieces[xs][5][0]] & bit_pieces[s][3]) &&
			!(bit_kingmoves[pieces[s][5][0]] & bit_pieces[xs][3]) &&
			!(bit_kingmoves[pieces[xs][5][0]] & bit_pieces[s][3]) &&
			rank[xs][pawn] < 5 && mask_rows[row[pawnplus[xs][pawn]]] & bit_pieces[s][3])
			return 1; //to test
		return 0;
	}

	//  rook pawn of wrong colour  -changed to 4 squares .63
	if (total[xs][2] == 1 && rank[s][king] < 3 &&
		!(bit_pieces[xs][0] & not_mask_rookfiles) &&
		piece_mat[s] <= BVAL)
	{
		int pawn = NextBit(bit_pieces[xs][0]);
		//if(!(bit_pieces[xs][0] & mask_cols[pawn]))
		//	return 0;
		int bish = pieces[xs][2][0];

		if (difference[king][lastsquare[xs][pawn]] < 2)
			if (colors[bish] != colors[lastsquare[xs][pawn]])
			{
				return 1;
			}
	}
	return 0;
}

int DrawnEnding2(const int s, const int xs)
{
	int xking = pieces[xs][5][0];
	int king = pieces[s][5][0];
	//bishop versus bishop
	if (total[s][2] == 1 && total[xs][2] == 1)
	{
		if (CanStopOne(s, xs) > 0 && CanStopOne(xs, s) > 0)
		{
			return 1;
		}
		if (pawn_mat[xs] == 200 && pawn_mat[s] == 0)
		{
			if (bit_pieces[xs][0] & mask_rookfiles && rank[s][king] < 3 &&
				!(bit_kingmoves[xking] & bit_pieces[s][2]))//bbit_attacks and b
			{
				int bish = pieces[xs][2][0];
				BITBOARD bb;
				if (s == 0)
					bb = bit_bishopmoves[bish] << 8;
				else
					bb = bit_bishopmoves[bish] >> 8;
				//bb = pawnplus[s][bish];
				if (bb & bit_pieces[xs][0] & not_mask_rookfiles)
				{
					if (bit_pieces[s][0] & mask_files[0])
					{
						if (difference[king][squares[xs][A8]] < 2)
							if (colors[bish] != xs)
								//if(bit_color[ colors[squares[xs][A8]] ] & bit_pieces[xs][2])
							{
								return 1;
							}
					}
					else if (bit_pieces[xs][0] & mask_files[7])
					{
						if (difference[king][squares[xs][H8]] < 2)
							if (colors[bish] == xs)
							{
								return 1;
							}
					}
				}
			}
			return 0;
		}
	}

	if (piece_mat[xs] == 900 && pawn_mat[xs] == 0 && piece_mat[s] == 500 && pawn_mat[s] == 100)
	{
		int pawn = pieces[s][0][0];
		if (bit_pawncaptures[s][pawn] & bit_pieces[s][3] &&
			bit_kingmoves[pieces[s][5][0]] & bit_pieces[s][0])
		{
			if (bit_pieces[s][0] & mask_ranks[s][1] && !(Attack(xs, pieces[s][5][0])))
			{
				return 1;
			}
		}
	}

	return 0;
}

int CanStopOne(const int s, const int xs)
{
	if (!bit_pieces[xs][0]) return 1;
	if (pawn_mat[xs] > 100) return 0;
	if (bit_kingmoves[pieces[s][5][0]] & bit_pieces[xs][2])
		return 0;
	int pawn = NextBit(bit_pieces[xs][0]);
	int bish = pieces[s][2][0];
	if (xs == 0 && bit_bishopmoves[bish] & (bit_pieces[xs][0] << 8) &&
		!(bit_between[bish][pawn + 8] & bit_all))
		return 1;
	if (xs == 1 && bit_bishopmoves[bish] & (bit_pieces[xs][0] >> 8) &&
		!(bit_between[bish][pawn - 8] & bit_all))
		return 1;

	return 0;
}

int LostRelative(const int s, const int xs)
{
	//return 0;//??
	if (piece_mat[xs] >= 900 && piece_mat[s] == 300)
	{
		if (bit_pieces[s][2] && !AnyAttacks(s, xs))
		{
			//and king safe from checks
			if (((mask_ranks[s][5] | mask_ranks[s][6]) & bit_pieces[s][0]) == 0)
			{
				//printf(" queen versus bishop ");
				//z();
				return 9910;
			}
		}

	}
	if (piece_mat[s] == 0 && !AnyAttacks(s, xs))
	{
		if (piece_mat[xs] >= 900 && bit_pieces[xs][4] && (start_piece_mat[s] + start_pawn_mat[s] > 0 || piece_mat[xs] > start_piece_mat[xs]))
		{
			if (pawn_mat[s] > 100 && (mask_ranks[s][6] & bit_pieces[s][0]) == 0)
			{
				return 9910;
			}
			if (pawn_mat[s] == 100 &&   //could add 2 pawns etc
				!(bit_pieces[s][0] & (mask[squares[s][A7]] | mask[squares[s][C7]] | mask[squares[s][F7]] | mask[squares[s][H7]])))
			{
				return 9910;
			}
		}
		//*/

		if (bit_pieces[xs][3] && start_piece_mat[s] > 0 &&
			((mask_ranks[s][5] | mask_ranks[s][6]) & bit_pieces[s][0]) == 0)
		{
			if (OpposedPawns(s, xs) == 1)
				return 9910;
			if (passed_list[s] == 0)
			{
				return 9910;

			}
		}
		if (pawn_mat[s] == 0 && (start_piece_mat[s] > 0 || start_pawn_mat[s] > 0))
		{
			if (piece_mat[xs] >= 500)
			{
				return 9910;
			}
		}
	}
	return 0;
}

int LostEnding(const int s, const int xs)
{
	if (!(piece_mat[s] == start_piece_mat[s] && pawn_mat[s] == start_pawn_mat[s] &&
		piece_mat[xs] <= start_piece_mat[xs]))
	{
		if (LostRelative(s, xs))
		{
			if (s == xside && piece_mat[s] == 0 && BlockedPawns2(s, xs) == 1)
			{
				if (SafeKingMoves(s, xs) == 0)
				{
					return 0;
				}
			}
			return 9100;
		}
	}
	//if(piece_mat[s]<600 && s==1)

	//q v n, q v b, q v r
	if (piece_mat[xs] >= 900)
	{
		if (pawn_mat[s] == 0 && piece_mat[s] < 600 &&
			!AnyAttacks(s, xs) &&//
			(start_piece_mat[s] > piece_mat[s] || start_pawn_mat[xs] > 0))
		{
			if (s == xside && piece_mat[s] == 0 && BlockedPawns2(s, xs) == 1)
			{
				if (SafeKingMoves(s, xs) == 0)
				{
					return 0;
				}
			}
			return 9100;
		}
		return 0;
	}

	//piece + p v king
	if (piece_mat[s] == 0 && pawn_mat[s] == 0 && pawn_mat[xs] == 100)
	{
		int pawn = NextBit(bit_pieces[xs][0]);

		if (total[xs][1] == 1)
		{
			if (pawn != squares[xs][A7] && pawn != squares[xs][H7])
				if (Attack(xs, pawn))
				{
					if (s == xside && piece_mat[s] == 0 && BlockedPawns2(s, xs) == 1)
					{
						if (SafeKingMoves(s, xs) == 0)
						{
							return 0;
						}
					}
					return 9100;
				}
			return 0;
		}
		if (total[xs][2] == 1)
		{
			//could add rook pawn of right colour
			if (bit_bishopmoves[pawn] & bit_pieces[xs][2] && (col[pawn] > 0 && col[pawn] < 7))
			{
				if (Attack(xs, pawn) && Attack(xs, pieces[xs][2][0]) == 0)
				{
					if (s == xside && piece_mat[s] == 0 && BlockedPawns2(s, xs) == 1)
					{
						if (SafeKingMoves(s, xs) == 0)
						{
							return 0;
						}
					}
					return 9100;
				}
			}
			return 0;
		}
		//r v p
		if (piece_mat[xs] == RVAL)
		{
			int pawn = NextBit(bit_pieces[s][0]);
			int r = pieces[xs][3][0];
			if (mask_path[s][pawn] & bit_pieces[xs][5] && Attack(s, r) == 0)
			{
				if (s == xside && piece_mat[s] == 0 && BlockedPawns2(s, xs) == 1)
				{
					if (SafeKingMoves(s, xs) == 0)
					{
						return 0;
					}
				}
				return 9100;
			}
		}
	}
	return 0;
}

int AnyAttacks(const int s, const int xs)
{
	BITBOARD  b1 = bit_units[xs];
	int sq;
	while (b1)
	{
		sq = NextBit(b1);
		b1 &= not_mask[sq];
		if (Attack(s, sq))
			return 1;
	}
	return 0;
}

int EndgameTacticalScore(const int s, const int xs)
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
			//Alg(x,x);
			//z();
			if (Attack(xs, pawnplus[s][x]) == 0 && LineAttack(xs, x) == 0 && !(mask_ranks[xs][6] & bit_pieces[xs][0]) &&
				b[pawnplus[s][x]] == 6)//add r+q behind + pinned
				return 800;
			//printf("+");
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
		if (rank[s][x] == 5)
		{
			if (passed_list[s] & bit_adjacent[x])
			{
				if (piece_mat[xs] == 500 && difference[pieces[xs][5][0]][x] > 2)
					score += 70;
			}
			continue;
		}

	}

	return score;
}

int EndgameTacticalScore2(const int s, const int xs)
{
	int score = 0;
	int x;

	BITBOARD b1 = passed_list[s] & (mask_squareking[xs][pieces[xs][5][0]]);
	//PrintBitBoard(b1);
	//PrintBitBoard(mask_squareking[xs][pieces[xs][5][0]]);
	//ShowAll2();


	while (b1)
	{
		x = NextBit(b1);
		b1 &= not_mask[x];
		//outside kingsquare, even with pieces
		score += passed[s][x] >> 1;
		if (piece_mat[xs] == 0)
		{
			score += SquarePawn(s, xs, x);
		}
		else if (total[xs][1] == 1)
		{
			score += passed[s][x] >> 1;
		}
		if (rank[s][x] == 5)
		{
			if (passed_list[s] & bit_adjacent[x])
			{
				//if(difference[pieces[xs][5][0]][x]<=2)
				//	z();
				if (piece_mat[xs] == 500 && difference[pieces[xs][5][0]][x] > 2)
					score += 70;
			}
			continue;
		}
		if (rank[s][x] == 6)
		{
			if (bit_pawncaptures[xs][x] & passed_list[s])
			{
				if (piece_mat[xs] == 500 && difference[pieces[xs][5][0]][x] > 2)
					score += 140;
			}
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
							score += 800;
						}
						continue;
					}
					if (total[xs][1] == 1)
					{
						int kn = pieces[xs][1][0];
						if (colors[x] == colors[kn])
						{
							score += 800;
						}
						continue;
					}
					if (Attack(s, pawnplus[s][x]))
					{   /*
						printf(" nothing cannot defend ");
						Alg1(x);
						z();
						//*/
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
	/*
	if((bit_colors[colors] & bit_pieces[1][2])
	  return 0;
	else

	 return 1;

	if(colour[pieces[0][2][0]] != colour[pieces[1][2][0]]);
		return 1;
	 */


	if (bit_color[1] & bit_pieces[0][2] && bit_color[0] & bit_pieces[1][2])
		return 1;
	if (bit_color[0] & bit_pieces[0][2] && bit_color[1] & bit_pieces[1][2])
		return 1;

	return 0;
}

int PawnEndings(const int s, const int xs)
{
	//k+p v k
	if (pawn_mat[s] == 0 && pawn_mat[xs] == 100)//could add more
	{
		int pawn = NextBit(bit_pieces[xs][0]);
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
	SetPawnFen();
	return PawnEndingScore(s, xs) - PawnEndingScore(xs, s);
}
//
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

		if (!(mask_squarepawn[s][x] & bit_pieces[xs][5]))//side switched 14/8/13
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
					//z();
				}
			}
		}
		//supported passed .69
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
			score += 20;    //1.58
		}
		if (col[x] == 7 || (col[x] == 6 && (mask_files[7] & bit_pieces[xs][0]) == 0) &&
			(bit_pieces[s][0] & mask_abc))
		{
			score += 20;    //1.58
		}
	}
	//King attack pawn
	if (bit_kingattacks[s] & bit_pieces[xs][0] & ~(bit_pawnattacks[xs]))
		score += 20;//27/2/20
	//
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
	/*
	int res = OpposedPawns2(xs,s);
	if(OpposedPawns(xs,s) != OpposedPawns2(xs,s))
	{
		printf(" res %d ",res);
		z();
		OpposedPawns2(xs,s);
	}
	*/
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
		/*
		 printf("\n side %d ",side);
		  printf("\n s %d ",s);
		 printf("\n adv %d ",MostAdvancedPawn(xs.s));
		  Alg(p,p);
		  printf("\n path %d ",path);
		  printf("\n path2 %d ",path2);
		  ShowAll2();
		//*/
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
		if (current == 6)current = 5;//double pawn move;
		if (mask_path[s][x] & bit_pieces[s][5])
			current++;
		if (mask_path[s][x] & bit_pieces[xs][0])
		{
			b2 = mask_path[s][x] & bit_pieces[xs][0];
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

int BlockedPawns2(const int s, const int xs)
{
	if (pawn_mat[s] == 0 || (bit_pawnattacks[s] & bit_units[xs]) != 0)
	{
		return 0;
	}

	BITBOARD b2;

	if (s == 0)
		b2 = bit_pieces[0][0] & (~bit_all) >> 8;
	else
		b2 = bit_pieces[1][0] & (~bit_all) << 8;
	if (b2 > 0)
		return 0;
	/*/
	b1 = bit_pieces[s][0];
	while(b1)
	{
	  x = NextBit(b1);
	  b1 &= not_mask[x];
	  if(b[pawnplus[s][x]]==6)
	  {
		  if(b2==0)
		  {
		PrintBitBoard(bit_pieces[s][0] & (~bit_all)>>8);
		PrintBitBoard(bit_pieces[s][0] & ((~bit_units[s])>>8));
			  z();
		  }
		  return 0;
	  }
	}
	 if(b2!=0)
		  {
		PrintBitBoard(bit_pieces[s][0] & (~bit_all)>>8);
		PrintBitBoard(bit_pieces[s][0] & ((~bit_units[xs])>>8));
			  z();
		  }
	//*/
	return 1;
}

int OpposedPawns2(const int s, const int xs)
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
	for (x = 0; x < 6; x++)
	{
		if (!(bit_pieces[s][0] & mask_files[x]) || !(bit_pieces[s][0] & mask_files[x + 1]) || !(bit_pieces[s][0] & mask_files[x + 2]))
			continue;
		if (s == 0)
		{
			if (((bit_pieces[s][0] << 8) & mask_files[x] & bit_pieces[xs][0])
				|| ((bit_pieces[s][0] << 8) & mask_files[x + 1] & bit_pieces[xs][0])
				|| ((bit_pieces[s][0] << 8) & mask_files[x + 2] & bit_pieces[xs][0]))
			{
				continue;
			}
		}
		else
		{
			if (((bit_pieces[s][0] >> 8) & mask_files[x] & bit_pieces[xs][0])
				|| ((bit_pieces[s][0] >> 8) & mask_files[x + 1] & bit_pieces[xs][0])
				|| ((bit_pieces[s][0] >> 8) & mask_files[x + 2] & bit_pieces[xs][0]))
			{
				continue;
			}
		}
		if (bit_pieces[s][0] & mask_files[x] && bit_pieces[s][0] & mask_files[x + 1] &&
			bit_pieces[s][0] & mask_files[x + 2])
		{
			//printf(" 3 in a row ");
			return 0;
		}
	}

	for (x = 0; x < 5; x++)
	{
		if (bit_pieces[s][0] & mask_files[x] && bit_pieces[s][0] & mask_files[x + 1] &&
			bit_pieces[s][0] & mask_files[x + 2] && bit_pieces[s][0] & mask_files[x + 3])
		{
			//printf(" 4 in a row ");
			return 0;
		}
	}

	return 1;
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

void SetPawnFen()
{
	bit_pawnattacks[0] = (bit_pieces[0][0] & not_a_file) << 7;
	bit_pawnattacks[0] |= (bit_pieces[0][0] & not_h_file) << 9;
	bit_pawnattacks[1] = (bit_pieces[1][0] & not_a_file) >> 9;
	bit_pawnattacks[1] |= (bit_pieces[1][0] & not_h_file) >> 7;

	for (int s = 0; s < 2; s++)
	{
		bit_kingattacks[s] = bit_kingmoves[pieces[s][5][0]];
		bit_attacks[s] = bit_pawnattacks[s] | bit_kingattacks[s];
	}
}