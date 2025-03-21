#include "globals.h"

int ThreatScore(const int s, const int xs, const int sq, const int p, const int best);

int p_value[6] = { 1,3,3,5,9,0 };

int SEESearch(int s, int attacker, const int sq)
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

int BestThreat(const int s, const int xs, const int diff)
{
	if (diff >= 9)
		return 0;
	int sq, i, x;

	int best = 0;

	BITBOARD b1, b2;

	b1 = bit_pieces[s][P] & mask_ranks[s][6];

	while (b1)
	{
		i = NextBit(b1);
		b1 &= not_mask[i];
		sq = pawnplus[s][i];
		if (b[sq] == EMPTY && Attack(xs, sq) == 0 &&
			!(mask_cols[i] & mask[pieces[xs][R][0]] && !(bit_between[i][NextBit(pieces[xs][R][0])] & bit_all)))
		{
			return i;
		}
	}

	BITBOARD bu = bit_pieces[xs][Q];

	if (diff < 5)
	{
		bu |= bit_pieces[xs][R];
		if (diff < 3)
		{
			bu |= bit_pieces[xs][N] | bit_pieces[xs][B];
			if (diff < 1)
				bu |= bit_pieces[xs][P];
		}
	}
	if (bu == 0)
		return 0;

	if (s == 0)
	{
		b1 = bu & (((bit_pieces[0][P] & not_h_file) << 9));
		b2 = bu & (((bit_pieces[0][P] & not_a_file) << 7));
	}
	else
	{
		b1 = bu & (((bit_pieces[1][P] & not_h_file) >> 7));
		b2 = bu & (((bit_pieces[1][P] & not_a_file) >> 9));
	}

	while (b1)
	{
		sq = NextBit(b1);
		b1 &= not_mask[sq];
		if (bit_left[!s][sq] & bit_pieces[s][P] & bit_all)
		{
			i = pawnleft[!s][sq];
		}
		if (ThreatScore(s, xs, sq, i, best))
			return 1;
	}

	while (b2)
	{
		sq = NextBit(b2);
		b2 &= not_mask[sq];
		if (bit_right[!s][sq] & bit_pieces[s][P] & bit_all)
		{
			i = pawnright[!s][sq];
		}
		if (ThreatScore(s, xs, sq, i, best))
			return 1;
	}

	for (x = 0; x < total[s][N]; x++)
	{
		i = pieces[s][N][x];
		b1 = bit_knightmoves[i] & bu;
		while (b1)
		{
			sq = NextBit(b1);
			b1 &= not_mask[sq];
			if (ThreatScore(s, xs, sq, i, best))
				return 1;
		}
	}

	for (x = 0; x < total[s][B]; x++)
	{
		i = pieces[s][B][x];
		b1 = bit_bishopmoves[i] & bu;
		while (b1)
		{
			sq = NextBit(b1);
			b1 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
			{
				if (ThreatScore(s, xs, sq, i, best))
					return 1;
			}
		}
	}

	for (x = 0; x < total[s][R]; x++)
	{
		i = pieces[s][R][x];
		b1 = bit_rookmoves[i] & bu;
		while (b1)
		{
			sq = NextBit(b1);
			b1 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
			{
				if (ThreatScore(s, xs, sq, i, best))
					return 1;
			}
		}
	}

	for (x = 0; x < total[s][Q]; x++)
	{
		i = pieces[s][Q][x];
		b1 = bit_queenmoves[i] & bu;
		while (b1)
		{
			sq = NextBit(b1);
			b1 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
			{
				if (Attack(xs, sq) == 0)
				{
					return 1;
				}
				else
				{
					if (SEESearch(s, i, sq) > best)
					{
						return 1;
					}
				}
			}
		}
	}

	i = pieces[s][K][0];
	b1 = bit_kingmoves[i] & bu;
	while (b1)
	{
		sq = NextBit(b1);
		b1 &= not_mask[sq];
		if (Attack(xs, sq) == 0)
		{
			return 1;
		}
	}
	return 0;
}

int ThreatScore(const int s, const int xs, const int sq, const int a, const int best)
{
	if ((p_value[b[sq]] > p_value[b[a]]) || Attack(xs, sq) == 0)
	{
		return 1;
	}
	else
	{
		if (SEESearch(s, a, sq) > best)
		{
			return 1;
		}
	}
	return 0;
}