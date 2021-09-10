#include "globals.h"

#define PAWN_SAME_BISHOP_COLOUR 8
#define MAJORITY_PENALTY	    	20
#define ISOLATED_PAWN_PENALTY		20
#define BACKWARDS_PAWN_PENALTY		8
#define DOUBLED_PAWN_PENALTY		10

int bishop_pair[2][3] = { {0,0,20},{0,0,20} };

int RookMoveCount(const int x);
int BishopMoveCount(const int x, const BITBOARD bp);

int EvalPawns(const int s, const int xs);
int EvalPawn(const int s, const int xs, const int sq);

int EvalEndgame();

int eval(const int alpha, const int beta)
{
	if (piece_mat[0] <= 900 && piece_mat[1] <= 900)
	{
		return EvalEndgame();
	}

	int king0 = pieces[0][5][0];
	int king1 = pieces[1][5][0];

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
	score[0] += piece_mat[0] + pawn_mat[0] + table_score[0];
	score[1] += piece_mat[1] + pawn_mat[1] + table_score[1];

	if (bit_pieces[1][4])
	{
		score[0] += KingScore[0][king0];
		score[0] += GetHashDefence(0, kingloc[king0]);
		score[1] += kingqueen[pieces[1][4][0]][king0];
	}
	else
	{
		score[0] += king_endgame_score[king0];
	}

	if (bit_pieces[0][4])
	{
		score[1] += KingScore[1][king1];
		score[1] += GetHashDefence(1, kingloc[king1]);
		score[0] += kingqueen[pieces[0][4][0]][king1];
	}
	else
	{
		score[1] += king_endgame_score[king1];
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
	BITBOARD bp = GetHashbp0();
	BITBOARD bp1 = GetHashbp1();

	int x, sq;
	for (x = 0; x < total[0][3]; x++)
	{
		sq = pieces[0][3][x];
		if (!(mask_cols[sq] & bit_pieces[0][0]))
		{
			score[0] += 10;
			if (!(mask_cols[sq] & bit_pieces[1][0]))
				score[0] += 10;
		}
		if (adjfile[sq][king1] && !(mask_path[0][sq] & bp))
		{
			score[0] += 5;
		}
		score[0] += RookMoveCount(sq);
	}
	for (x = 0; x < total[1][3]; x++)
	{
		sq = pieces[1][3][x];
		if (!(mask_cols[sq] & bit_pieces[1][0]))
		{
			score[1] += 10;
			if (!(mask_cols[sq] & bit_pieces[0][0]))
				score[1] += 10;
		}
		if (adjfile[sq][king0] && !(mask_path[0][sq] & bp))
		{
			score[1] += 5;
		}
		score[1] += RookMoveCount(sq);
	}

	for (x = 0; x < total[0][2]; x++)
	{
		if (bit_bishopmoves[pieces[0][2][x]] & bit_kingmoves[pieces[1][5][0]])
			score[0] += 5;
		score[0] += BishopMoveCount(pieces[0][2][x], (bp & bit_pawnattacks[1]) | bit_pieces[0][5]);
	}

	for (x = 0; x < total[1][2]; x++)
	{
		if (bit_bishopmoves[pieces[1][2][x]] & bit_kingmoves[pieces[0][5][0]])
			score[1] += 5;
		score[1] += BishopMoveCount(pieces[1][2][x], (bp1 & bit_pawnattacks[0]) | bit_pieces[1][5]);
	}

	score[0] += bishop_pair[0][total[0][2]];
	score[1] += bishop_pair[1][total[1][2]];

	for (x = 0; x < total[0][1]; x++)
	{
		if (bit_knightmoves[pieces[0][1][x]] & bit_kingmoves[king1])
			score[0] += 5;
		if (bit_knightmoves[pieces[0][1][x]] & bit_pawnattacks[1])
			score[0] -= 5;
	}

	for (x = 0; x < total[1][1]; x++)
	{
		score[1] += kingknight[pieces[1][1][x]][king0];
		if (bit_knightmoves[pieces[1][1][x]] & bit_pawnattacks[0])
			score[1] -= 5;
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

int EvalPawn(const int s, const int xs, const int sq)
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
		r += 8;
	//could add p defends centre pawn
	kingside[s] += KingSide[s][sq];
	queenside[s] += QueenSide[s][sq];

	kingattack[xs] += KingSide2[s][sq];
	queenattack[xs] += QueenSide2[s][sq];
	return r;
}
