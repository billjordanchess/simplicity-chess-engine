#include "globals.h"

#define BACKWARDS_PAWN_PENALTY		8
#define MAJORITY_PENALTY 20

int bishop_pair[2][3] = { {0,0,20},{0,0,20} };

int RookMoveCount(const int x, const BITBOARD denied_squares);
int BishopMoveCount(const int x, const BITBOARD denied_squares);
int QueenMoveCount(const int x, const BITBOARD denied_squares);

int EvalPawns(const int s, const int xs);
int EvalPawn(const int s, const int xs, const int sq);

int EvalEndgame();

int bishopmoves[14] = {-5,1,2,3,4,5,6,7,8,9,10,11,12,13};
int rookmoves[15] = {-5,1,2,3,4,5,6,7,8,9,10,11,12,13,14};
int queenmoves[28] = {-5,1,1,1,2,2,2,3,3,3,4,4,4,5,5,5,6,6,6,7,7,7,8,8,8,9,9,9};

int king_defend[2][2][2][2][2][2][2][2][2];

void z();

int Eval(const int alpha, const int beta)
{
	if (!piece_mat[xside])
	{
		if (bit_pieces[side][Q])
			if (!(bit_pieces[xside][0] & (mask_ranks[side][1] | mask_ranks[side][2])))
				 return 9900;
		if (bit_pieces[side][R])
			if (!(bit_pieces[xside][0] &
				(mask_ranks[side][1] | mask_ranks[side][2] | mask_ranks[side][3])))
				 return 9900;
	}

	if (piece_mat[0] <= QVAL && piece_mat[1] <= QVAL)
	{
		if (pawn_mat[0] == 0 && pawn_mat[1] == 0)
		{
			return EvalPawnless();
		}
		if ((piece_mat[0] < BBVAL || piece_mat[0] == QVAL) && 
			(piece_mat[1] < BBVAL || piece_mat[1] == QVAL))
		{
			return EvalEndgame(); 
		}
	}

	int king[2];
	king[0] = pieces[0][K][0];
	king[1] = pieces[1][K][0];

	int score[2] = {0,0};

	if (LookUpPawn())
	{
		score[0] = GetHashPawn(0);
		score[1] = GetHashPawn(1);
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

		bit_pawnattacks[0] = (bit_pieces[0][P] & not_a_file) << 7;
		bit_pawnattacks[0] |= (bit_pieces[0][P] & not_h_file) << 9;
		AddPawnAttackHash(0, bit_pawnattacks[0]);

		bit_pawnattacks[1] = (bit_pieces[1][P] & not_a_file) >> 9;
		bit_pawnattacks[1] |= (bit_pieces[1][P] & not_h_file) >> 7;
		AddPawnAttackHash(1, bit_pawnattacks[1]);
	}
	score[0] += centi_pieces[piece_mat[0]] + centi_pawns[pawn_mat[0]] + table_score[0];
	score[1] += centi_pieces[piece_mat[1]] + centi_pawns[pawn_mat[1]] + table_score[1];

	if (bit_pieces[1][Q])
	{
		score[0] += KingScore[0][king[0]];
		score[0] += GetHashDefence(0, kingloc[king[0]]);
		score[1] += kingqueen[pieces[1][Q][0]][king[0]];
	}
	else
	{
		score[0] += king_endgame_score[king[0]];
	}

	if (bit_pieces[0][Q])
	{
		score[1] += KingScore[1][king[1]];
		score[1] += GetHashDefence(1, kingloc[king[1]]);
		score[0] += kingqueen[pieces[0][Q][0]][king[1]];
	}
	else
	{
		score[1] += king_endgame_score[king[1]];
	}
	if (mask[A7] & bit_pieces[0][B] && mask[B6] & bit_pieces[1][P])
	{
		score[0] -= 150;
	}
	if (mask[H7] & bit_pieces[0][B] && mask[G6] & bit_pieces[1][P])
	{
		score[0] -= 150;
	}
	if (mask[A2] & bit_pieces[1][B] && mask[B3] & bit_pieces[0][P])
	{
		score[1] -= 150;
	}
	if (mask[H2] & bit_pieces[1][B] && mask[G3] & bit_pieces[0][P])
	{
		score[1] -= 150;
	}
	if (score[side] - score[xside] + 80 <= alpha || score[side] - score[xside] - 80 > beta)
	{
	    return score[side] - score[xside];
	}
	
	bit_pawnattacks[0] = GetHashPawnAttacks(0);
	bit_pawnattacks[1] = GetHashPawnAttacks(1);

	int x, sq, xs;
	for (int s = 0; s < 2; s++)
	{
		xs = !s;
		for (x = 0; x < total[s][R]; x++)
		{
			sq = pieces[s][R][x];
			if (!(mask_cols[sq] & bit_pieces[s][P]))
			{
				score[s] += 10;
				if (!(mask_cols[sq] & bit_pieces[xs][P]))
				{
					score[s] += 10;
					if (mask_cols[sq] & bit_pieces[xs][K])
					{
						score[s] += 10;
					}
				}
			}
			if (adjfile[sq][king[xs]] && 
				!(mask_path[s][sq] & bit_pawnattacks[xs] & bit_pieces[xs][P]))
			{
				score[s] += 5;
			}
			score[s] += rookmoves[RookMoveCount(sq, bit_pawnattacks[xs] | bit_units[s])];
		}
		for (x = 0; x < total[s][B]; x++)
		{
			if (bit_bishopmoves[pieces[s][B][x]] & bit_kingmoves[pieces[xs][K][0]])
				 score[s] += 2;
			score[s] += bishopmoves[BishopMoveCount(pieces[s][B][x], bit_pawnattacks[xs] | bit_units[s])];
		}
		score[s] += bishop_pair[s][total[s][B]];
		int i, n;

		for (x = 0; x < total[s][N]; x++)
		{
			i = pieces[side][1][x];
			for (int j = 0; j < knight_total[i]; j++)
			{
				n = knightmoves[i][j];
				if (mask[n] & bit_pawnattacks[xs])
					score[s] -= 2;
			}
			if (bit_knightmoves[pieces[s][N][x]] & bit_kingmoves[king[xs]])
				score[s] += 2;
		}
		if (bit_pieces[s][Q])
			score[s] += queenmoves[QueenMoveCount(pieces[s][Q][0], bit_pawnattacks[xs] | bit_units[s])];
	}
		if ((bit_pieces[0][P] & (mask[D2] | mask[E2])) << 8 & bit_units[0])
		{
			score[0] -= 20;
		}
		if ((bit_pieces[1][P] & (mask[D7] | mask[E7])) >> 8 & bit_units[1])
		{
			score[1] -= 20;
		}
	if (bit_pawnattacks[side] & bit_pieces[xside][1])
	{
		score[side] += 4;
	}

	return score[side] - score[xside];
}

int EvalPawns(const int s, const int xs)
{
	int i, score = 0;

	BITBOARD b1 = bit_pieces[s][P];
	while (b1)
	{
		i = NextBit(b1);
		b1 &= not_mask[i];
		score += EvalPawn(s, xs, i);
	}
	if (bit_pieces[s][P] & mask[D4] && bit_adjacent[D4] & bit_pieces[s][P])
		score += 12;
	else if (bit_pieces[s][P] & mask[E4] && bit_adjacent[E4] & bit_pieces[s][P])
		score += 12;
	else if (bit_pieces[s][P] & mask[D5] && bit_adjacent[D5] & bit_pieces[s][P])
		score += 12;
	else if (bit_pieces[s][P] & mask[E5] && bit_adjacent[E5] & bit_pieces[s][P])
		score += 12;

	return score;
}

int EvalPawn(const int s, const int xs, const int sq)
{
	int score = 0;
	if (!(mask_passed[s][sq] & bit_pieces[xs][P]) && 
		!(mask_path[s][sq] & bit_pieces[s][P]))
	{
		if (bit_pieces[s][P] & bit_adjacent[sq])
		{
			score += adjacent_passed[s][sq];
		}
		if (bit_pawncaptures[xs][sq] & bit_pieces[s][P])
		{
			score += defended_passed[s][sq];
		}
		score += passed[s][sq];
		score += PieceScore[s][0][sq];
		passed_list[s] |= mask[sq]; 
		kingside[s] += KingSide[s][sq];
		queenside[s] += QueenSide[s][sq];
		kingattack[xs] += KingSide2[s][sq];
		queenattack[xs] += QueenSide2[s][sq];
		return score;
	}

	if ((mask_isolated[sq] & bit_pieces[s][P]) == 0)
	{
		score -= isolated[sq];
		if ((mask_cols[sq] & bit_pieces[xs][P]) == 0)
		{
			score -= isolated[sq];
			if (bit_pieces[s][P] & mask_path[s][sq])
			{
				score -= 10;
			}
		}
	}

	else
	{
		if (bit_pieces[s][P] & mask_path[s][sq])
		{
			score -= 10;
			if (!((bit_pieces[0][P] | bit_pieces[1][P]) & mask_left_col[sq]) ||
				!((bit_pieces[0][P] | bit_pieces[1][P]) & mask_right_col[sq]))
			{
				score -= MAJORITY_PENALTY;
			}
		}
		if ((mask_backward[s][sq] & bit_pieces[s][P]) == 0)
		{
			score -= BACKWARDS_PAWN_PENALTY;
			if (bit_pawncaptures[s][pawnplus[s][sq]] & bit_pieces[xs][P])
			{
				score -= BACKWARDS_PAWN_PENALTY;
			}
			if (bit_pieces[s][P] & mask_path[xs][sq])
			{
				score -= isolated[sq];
			}
			if ((mask_cols[sq] & bit_pieces[xs][P]) == 0)
				score -= BACKWARDS_PAWN_PENALTY;
		}
	}
	score += PieceScore[s][0][sq];
	
	kingside[s] += KingSide[s][sq];
	queenside[s] += QueenSide[s][sq];

	kingattack[xs] += KingSide2[s][sq];
	queenattack[xs] += QueenSide2[s][sq];
	return score;
}

int RookMoveCount(const int x, const BITBOARD denied_squares)
{
    int nc = 0;
    int sq = rooklist[x][nc].sq;
    int count = 0;
    do
    {
        if (mask[sq] & bit_all)
        {
            if (!(mask[sq] & denied_squares))
            {
               count++;
            }
            nc = rooklist[x][nc].next;
			if(nc==-1)break;
        }
        else
        {
            nc++;
            count++;
        }
        sq = rooklist[x][nc].sq;
    } while (sq > -1);
    return count;
}

int BishopMoveCount(const int x, const BITBOARD denied_squares)
{
    int nc = 0;
    int sq = bishoplist[x][nc].sq;
    int count = 0;
    do
    {
        if (mask[sq] & bit_all)
        { 
            if (!(mask[sq] & denied_squares))
            {
                count++;
            }
            nc = bishoplist[x][nc].next;
			if(nc==-1)break;
        }
        else
        {
            nc++;
            count++;
        }
        sq = bishoplist[x][nc].sq;
    } while (sq > -1);
    return count;
}

int QueenMoveCount(const int x, const BITBOARD denied_squares)
{
    int nc = 0;
    int sq = queenlist[x][nc].sq;
    int count = 0;
    do
    {
        if (mask[sq] & bit_all)
        {
            if (!(mask[sq] & denied_squares))
            {
                count++;
            }
            nc = queenlist[x][nc].next;
			if(nc==-1)break;
        }
        else
        {
            nc++;
            count++;
        }
        sq = queenlist[x][nc].sq;
    } while (sq > -1);
    return count;
}
