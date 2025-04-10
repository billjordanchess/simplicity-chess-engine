#include "globals.h"

int AttackKing(const int s, const int sq);

int p_value[6] = { 1,3,3,5,9,0 };

int SEE(int s, int attacker, const int sq)
{
	int piece;
	int count = 0;
	int total = 0;
	int score[12];

	BITBOARD old = bit_all;

	memset(score, 0, sizeof(score));
	score[0] = p_value[b[sq]];
	score[1] = p_value[b[attacker]];

	int total_score = 0;

	while (count < 10)
	{
		bit_all &= not_mask[attacker];
		s ^= 1;

		total++;
		count++;

		piece = GetNextAttacker(s, sq);

		if (piece > -1)
		{
			score[count + 1] = p_value[b[piece]];
			if (score[count] > score[count - 1] + score[count + 1])
			{
				count--;
				break;
			}
		}
		else
		{
			break;
		}
		attacker = piece;
	}

	while (count > 1)
	{
		if (score[count - 1] >= score[count - 2])
			count -= 2;
		else
			break;
	}

	for (int x = 0; x < count; x++)
	{
		if (x % 2 == 0)
			total_score += score[x];
		else
			total_score -= score[x];
	}

	bit_all = old;

	return total_score;
}

bool BestThreat(const int s, const int xs, const int diff)
{
	if (diff >= QVAL)
		return false;
	int sq, i, x;

	BITBOARD b1, b2;

	b1 = bit_pieces[s][P] & mask_ranks[s][6];

	while (b1)
	{
		i = NextBit(b1);
		b1 &= not_mask[i];
		sq = pawnplus[s][i];
		if (b[sq] == EMPTY && Attack(xs, sq) == 0 &&
			!(mask_cols[i] & mask[pieces[xs][R][0]] && 
			!(bit_between[i][NextBit(pieces[xs][R][0])] & bit_all)))
		{
			return true;
		}
	}

	BITBOARD bit_targets = bit_pieces[xs][Q];

	if (diff < RVAL)//Rooks are potentially a target.
	{
		bit_targets |= bit_pieces[xs][R];
		if (diff < BVAL)//Minor pieces are potentially a target.
		{
			bit_targets |= bit_pieces[xs][N] | bit_pieces[xs][B];
			if (diff < PVAL)//Pawns are potentially a target.
				bit_targets |= bit_pieces[xs][P];
		}
	}
	if (bit_targets == 0)
		return false;

	if (s == 0)//Are there any pawn attacks?
	{
		b1 = bit_targets & (((bit_pieces[0][P] & not_h_file) << 9));
		b2 = bit_targets & (((bit_pieces[0][P] & not_a_file) << 7));
	}
	else
	{
		b1 = bit_targets & (((bit_pieces[1][P] & not_h_file) >> 7));
		b2 = bit_targets & (((bit_pieces[1][P] & not_a_file) >> 9));
	}

	while (b1)
	{
		sq = NextBit(b1);
		b1 &= not_mask[sq];
		if (b[sq] > P || Attack(xs, sq) == 0)
		{
			return true;
		}
		if (SEE(s, pawnleft[!s][sq], sq) > 0)
		{
			return true;
		}
	}

	while (b2)
	{
		sq = NextBit(b2);
		b2 &= not_mask[sq];
		if (b[sq] > P || Attack(xs, sq) == 0)
		{
			return true;
		}
		if (SEE(s, pawnright[!s][sq], sq) > 0)
		{
			return true;
		}
	}

	for (x = 0; x < total[s][N]; x++)
	{
		i = pieces[s][N][x];
		b1 = bit_knightmoves[i] & bit_targets;
		while (b1)
		{
			sq = NextBit(b1);
			b1 &= not_mask[sq];
			if (b[sq] > B || Attack(xs, sq) == 0)
			{
				return true;
			}
			else if (SEE(s, i, sq) > 0)
			{
				return true;
			}
		}
	}

	for (x = 0; x < total[s][B]; x++)
	{
		i = pieces[s][B][x];
		b1 = bit_bishopmoves[i] & bit_targets;
		while (b1)//Are there any bishop attacks?
		{
			sq = NextBit(b1);
			b1 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
			{
				if (b[sq] > B || Attack(xs, sq) == 0)
				{
					return true;
				}
				else if (SEE(s, i, sq) > 0)
				{
					return true;
				}
			}
		}
	}

	for (x = 0; x < total[s][R]; x++)
	{
		i = pieces[s][R][x];
		b1 = bit_rookmoves[i] & bit_targets;
		while (b1)//Are there any rook attacks?
		{
			sq = NextBit(b1);
			b1 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
			{
				if (b[sq] > R || Attack(xs, sq) == 0)
				{
					return true;
				}
				else if (SEE(s, i, sq) > 0)
				{
					return true;
				}
			}
		}
	}

	for (x = 0; x < total[s][Q]; x++)
	{
		i = pieces[s][Q][x];
		b1 = bit_queenmoves[i] & bit_targets;
		while (b1)//Are there any queen attacks?
		{
			sq = NextBit(b1);
			b1 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
			{
				if (Attack(xs, sq) == 0)
				{
					return true;
				}
			}
		}
	}

	i = pieces[s][K][0];
	b1 = bit_kingmoves[i] & bit_targets;
	while (b1)//Are there any king attacks?
	{
		sq = NextBit(b1);
		b1 &= not_mask[sq];
		if (Attack(xs, sq) == 0)
		{
			return true;
		}
	}
	return false;
}

int GetNextAttacker(const int s, const int sq)
{
	if (bit_pawndefends[s][sq] & bit_pieces[s][P] & bit_all)
	{
		if (bit_left[!s][sq] & bit_pieces[s][P] & bit_all)
		{
			return pawnleft[!s][sq];
		}
		else
		{
			return pawnright[!s][sq];
		}
	}

	int i;

	for (int x = 0; x < total[s][N]; x++)
	{
		i = pieces[s][N][x];
		if (bit_knightmoves[sq] & mask[i] & bit_all)
		{
			return i;
		}
	}
	for (int x = 0; x < total[s][B]; x++)
	{
		i = pieces[s][B][x];
		if (bit_bishopmoves[sq] & mask[i] & bit_all)
		{
			if (!(bit_between[i][sq] & bit_all))
			{
				return i;
			}
		}
	}
	for (int x = 0; x < total[s][R]; x++)
	{
		i = pieces[s][R][x];
		if (bit_rookmoves[sq] & mask[i] & bit_all)
		{
			if (!(bit_between[i][sq] & bit_all))
			{
				return i;
			}
		}
	}
	for (int x = 0; x < total[s][Q]; x++)
	{
		i = pieces[s][Q][x];
		if (bit_queenmoves[sq] & mask[i] & bit_all)
		{
			if (!(bit_between[i][sq] & bit_all))
			{
				return i;
			}
		}
	}
	if (bit_kingmoves[sq] & bit_pieces[s][K] & bit_all)
			return pieces[s][K][0];
	return -1;
}
