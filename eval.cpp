#include "globals.h"

#define PAWN_SAME_BISHOP_COLOUR 8
#define MAJORITY_PENALTY	    	20
#define ISOLATED_PAWN_PENALTY		20
#define BACKWARDS_PAWN_PENALTY		8
#define DOUBLED_PAWN_PENALTY		10

int bishop_pair[2][3] = { {0,0,20},{0,0,20} };

int RookMoveCount(const int x, const BITBOARD bp, const BITBOARD xp);
int BishopMoveCount(const int x, const BITBOARD bp, const BITBOARD xp);
int QueenMoveCount(const int x, const BITBOARD bp);

int EvalPawns(const int s, const int xs);
int EvalPawn(const int s, const int xs, const int sq);

int EvalEndgame();

int bishopmoves[14] = {-5,1,2,3,4,5,6,7,8,9,10,11,12,13};
int rookmoves[15] = {-5,1,2,3,4,5,6,7,8,9,10,11,12,13,14};
int queenmoves[28] = {-5,1,1,1,2,2,2,3,3,3,4,4,4,5,5,5,6,6,6,7,7,7,8,8,8,9,9,9};

/*
failure to break centre e4 c5 d4 nf6 nc3 e6
bd3 in front of d2
too many kh1s
early kf1 and kg1
self pin with bd6
king blocking rook or bishop
failure to attack with pawns
pawn forks
*/

void z();

int eval(const int alpha, const int beta)
{
	if (piece_mat[0] <= QVAL && piece_mat[1] <= QVAL)
	{
		if (pawn_mat[0] == 0 && pawn_mat[1] == 0)
		{
			return EvalPawnless();
		}
		if ((piece_mat[0] <BBVAL || piece_mat[0] == QVAL) && (piece_mat[1] < BBVAL || piece_mat[1] == QVAL))
		{
			return EvalEndgame(); 
		}
	}

	int king[2];
	king[0] = pieces[0][5][0];
	king[1] = pieces[1][5][0];

	int score[2] = {0,0};

	if (LookUpPawn())
	{
		score[0] = GetHashPawn0();
		score[1] = GetHashPawn1();
	}
	else
	{
		passed_list[0] = 0;
		passed_list[1] = 0;

		kingside[0] = 0;
		kingside[1] = 0;
		queenside[0] = 0;
		queenside[1] = 0;
		kingattack[0] = 0;
		kingattack[1] = 0;
		queenattack[0] = 0;
		queenattack[1] = 0;

		score[0] += EvalPawns(0, 1);
		score[1] += EvalPawns(1, 0);
	
		AddPawnHash(score[0], score[1], passed_list[0], passed_list[1]);

		AddQueenHash(0, scale[queenside[0]] + queenattack[0]);
		AddKingHash(0, scale[kingside[0]] + kingattack[0]);
		AddQueenHash(1, scale[queenside[1]] + queenattack[1]);
		AddKingHash(1, scale[kingside[1]] + kingattack[1]);

		bit_pawnattacks[0] = (bit_pieces[0][0] & not_a_file) << 7;
		bit_pawnattacks[0] |= (bit_pieces[0][0] & not_h_file) << 9;
		AddPawnAttackHash(0, bit_pawnattacks[0]);

		bit_pawnattacks[1] = (bit_pieces[1][0] & not_a_file) >> 9;
		bit_pawnattacks[1] |= (bit_pieces[1][0] & not_h_file) >> 7;
		AddPawnAttackHash(1, bit_pawnattacks[1]);

		BITBOARD  bp = bit_pieces[0][0] | (bit_pieces[1][0] & bit_pawnattacks[1]);
		AddHashbp0(bp);
		bp = bit_pieces[1][0] | (bit_pieces[0][0] & bit_pawnattacks[0]);
		AddHashbp1(bp);
	}
	score[0] += piece_mat[0] + centi_pawns[pawn_mat[0]] + table_score[0];
	score[1] += piece_mat[1] + centi_pawns[pawn_mat[1]] + table_score[1];

	if (bit_pieces[1][4])
	{
		score[0] += KingScore[0][king[0]];
		score[0] += GetHashDefence(0, kingloc[king[0]]);
		score[1] += kingqueen[pieces[1][4][0]][king[0]];
	}
	else
	{
		score[0] += king_endgame_score[king[0]];
	}

	if (bit_pieces[0][4])
	{
		score[1] += KingScore[1][king[1]];
		score[1] += GetHashDefence(1, kingloc[king[1]]);
		score[0] += kingqueen[pieces[0][4][0]][king[1]];
	}
	else
	{
		score[1] += king_endgame_score[king[1]];
	}
	if (mask[A7] & bit_pieces[0][2] && mask[B6] & bit_pieces[1][0])
	{
		score[0] -= 150;
	}
	if (mask[H7] & bit_pieces[0][2] && mask[G6] & bit_pieces[1][0])
	{
		score[0] -= 150;
	}
	if (mask[A2] & bit_pieces[1][2] && mask[B3] & bit_pieces[0][0])
	{
		score[1] -= 150;
	}
	if (mask[H2] & bit_pieces[1][2] && mask[G3] & bit_pieces[0][0])
	{
		score[1] -= 150;
	}
	if (score[side] - score[xside] + 80 <= alpha || score[side] - score[xside] - 80 > beta)
	{
	    return score[side] - score[xside];
	}
	
	bit_pawnattacks[0] = GetHashPawnAttacks(0);
	bit_pawnattacks[1] = GetHashPawnAttacks(1);
	BITBOARD bpawn[2];
	bpawn[0] = GetHashbp0();
	bpawn[1] = GetHashbp1();

	int x, sq, xs;
	for (int s = 0; s < 2; s++)
	{
		xs = !s;
		for (x = 0; x < total[s][3]; x++)
		{
			sq = pieces[s][3][x];
			if (!(mask_cols[sq] & bit_pieces[s][0]))
			{
				score[s] += 10;
				if (!(mask_cols[sq] & bit_pieces[xs][0]))
				{
					score[s] += 10;
					if (mask_cols[sq] & bit_pieces[xs][5])
					{
						//score[s] += 10;//7/10/21
					}
				}
			}
			if (adjfile[sq][king[xs]] && !(mask_path[s][sq] & bpawn[s]))
			{
				score[s] += 5;
			}
			score[s] += rookmoves[RookMoveCount(sq, (bpawn[xs] & bit_pawnattacks[xs]) | bit_units[s], bit_pieces[xs][0])];
		}
		for (x = 0; x < total[s][2]; x++)
		{
			//if (pieces[s][2][x] == H5)
			//	z();
			if (bit_bishopmoves[pieces[s][2][x]] & bit_kingmoves[pieces[xs][5][0]])
				 score[s] += 2;
			score[s] += bishopmoves[BishopMoveCount(pieces[s][2][x], (bpawn[xs] & bit_pawnattacks[xs]) | bit_pieces[s][0] | bit_pieces[s][5], bit_pieces[xs][0])];
		}
		score[s] += bishop_pair[s][total[s][2]];
		int i, n;

		for (x = 0; x < total[s][1]; x++)
		{
			i = pieces[side][1][x];
			for (int j = 0; j < knight_total[i]; j++)
			{
				n = knightmoves[i][j];
				if (mask[n] & bit_pawnattacks[xs])
					score[s] --;
			}
			if (bit_knightmoves[pieces[s][1][x]] & bit_kingmoves[king[xs]])
				score[s] += 2;
		}
		if (bit_pieces[s][4])
			score[s] += queenmoves[QueenMoveCount(pieces[s][4][0], (bpawn[xs] & bit_pawnattacks[xs]) | bit_pieces[s][5])];
		//kingmoves etc
	}
		if ((bit_pieces[0][0] & (mask[squares[0][D2]] | mask[E2])) << 8 & bit_units[0])
		{
			score[0] -= 20;
		}
		if ((bit_pieces[1][0] & (mask[D7] | mask[E7])) >> 8 & bit_units[1])
		{
			score[1] -= 20;
		}
	if (bit_pawnattacks[side] & bit_pieces[xside][1])
	{
		score[side] += 4;
		//if(bit_pawnattacks[side] & bit_pieces[xside][1] & 1)
	}

	return score[side] - score[xside];
}

int EvalPawns(const int s, const int xs)
{
	int i, score = 0;

	BITBOARD b1 = bit_pieces[s][0];
	while (b1)
	{
		i = NextBit(b1);
		b1 &= not_mask[i];
		score += EvalPawn(s, xs, i);
	}

	return score;
}

int EvalPawn(const int s, const int xs, const int sq)//165579
{
	int r = 0;

	if (!(mask_passed[s][sq] & bit_pieces[xs][0]) && !(mask_path[s][sq] & bit_pieces[s][0]))
	{
		if (bit_pieces[s][0] & bit_adjacent[sq])
		{
			r += adjacent_passed[s][sq];
		}
		if (bit_pawncaptures[xs][sq] & bit_pieces[s][0])
		{
			r += defended_passed[s][sq];
		}
		r += passed[s][sq];
		r += PieceScore[s][0][sq];
		passed_list[s] |= mask[sq]; //contesting pawns
		kingside[s] += KingSide[s][sq];
		queenside[s] += QueenSide[s][sq];
		kingattack[xs] += KingSide2[s][sq];
		queenattack[xs] += QueenSide2[s][sq];
		return r;
	}

	if ((mask_isolated[sq] & bit_pieces[s][0]) == 0)
	{
		r -= isolated[sq];
		if ((mask_cols[sq] & bit_pieces[xs][0]) == 0)
			r -= isolated[sq];
	}

	else
	{
		if (bit_pieces[s][0] & mask_path[s][sq])
		{
			r -= 10;
			if (!((bit_pieces[0][0] | bit_pieces[1][0]) & mask_left_col[sq]) ||
				!((bit_pieces[0][0] | bit_pieces[1][0]) & mask_right_col[sq]))
			{
				r -= MAJORITY_PENALTY;
			}
		}
		if ((mask_backward[s][sq] & bit_pieces[s][0]) == 0)
		{
			r -= BACKWARDS_PAWN_PENALTY;
			if (bit_pawncaptures[s][pawnplus[s][sq]] & bit_pieces[xs][0])
			{
				r -= BACKWARDS_PAWN_PENALTY;
			}
			if (bit_pieces[s][0] & mask_path[xs][sq])
			{
				r -= isolated[sq];//or doubled?
			}
			if ((mask_cols[sq] & bit_pieces[xs][0]) == 0)
				r -= BACKWARDS_PAWN_PENALTY;//worse on open file
		}
	}
	r += PieceScore[s][0][sq];
	if (mask[sq] & mask_centre && bit_adjacent[sq] & bit_pieces[s][0])
		r += 12;// 12;//8
	kingside[s] += KingSide[s][sq];
	queenside[s] += QueenSide[s][sq];

	kingattack[xs] += KingSide2[s][sq];
	queenattack[xs] += QueenSide2[s][sq];
	return r;
}
