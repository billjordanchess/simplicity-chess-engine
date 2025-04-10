#include "globals.h"

int AttackKing(const int s, const int sq);

bool Attack(const int s, const int sq)
{
	if (bit_pawndefends[s][sq] & bit_pieces[s][P])
		return true;
	if (bit_knightmoves[sq] & bit_pieces[s][N])
		return true;

	int i;
	BITBOARD b1 = bit_rookmoves[sq] & (bit_pieces[s][R] | bit_pieces[s][Q]);
	b1 |= (bit_bishopmoves[sq] & (bit_pieces[s][B] | bit_pieces[s][Q]));

	while (b1)
	{
		i = NextBit(b1);
		if (!(bit_between[i][sq] & bit_all))
		{
			return true;
		}
		b1 &= not_mask[i];
	}

	if (bit_kingmoves[sq] & bit_pieces[s][K])
		return true;
	return false;
}

int Check(const int s, const int sq)
{
	int i;
	if (bit_knightmoves[sq] & bit_pieces[s][N])
	{
		for (int x = 0; x < total[s][N]; x++)
		{
			i = pieces[s][N][x];
			if (bit_knightmoves[sq] & mask[i])
				return i;
		}
	}
	if (bit_pawndefends[s][sq] & bit_pieces[s][P])
	{
		if (bit_left[!s][sq] & bit_pieces[s][P])
		{
			return pawnleft[!s][sq];
		}
		if (bit_right[!s][sq] & bit_pieces[s][P])
		{
			return pawnright[!s][sq];
		}
	}
	BITBOARD b1 = bit_bishopmoves[sq] & (bit_pieces[s][B] | bit_pieces[s][Q]);
	b1 |= (bit_rookmoves[sq] & (bit_pieces[s][R] | bit_pieces[s][Q]));

	while (b1)
	{
		i = NextBit(b1);
		if (!(bit_between[i][sq] & bit_all))
		{
			return i;
		}
		b1 &= not_mask[i];
	}
	return -1;
}

bool CheckAttack(const int s, const int sq)
{
	if (bit_knightmoves[sq] & bit_pieces[s][N])
		return true;
	if (bit_pawndefends[s][sq] & bit_pieces[s][P])
		return true;
	BITBOARD b1 = bit_bishopmoves[sq] & (bit_pieces[s][B] | bit_pieces[s][Q]);
	b1 |= (bit_rookmoves[sq] & (bit_pieces[s][R] | bit_pieces[s][Q]));
	int i;

	while (b1)
	{
		i = NextBit(b1);
		if (!(bit_between[i][sq] & bit_all))
			return true;
		b1 &= not_mask[i];
	}
	return false;
}

bool LineAttack(const int s, const int sq)
{
	BITBOARD b1 = bit_bishopmoves[sq] & (bit_pieces[s][B] | bit_pieces[s][Q]);
	b1 |= (bit_rookmoves[sq] & (bit_pieces[s][R] | bit_pieces[s][Q]));

	while (b1)
	{
		int i = NextBit(b1);
		if (!(bit_between[i][sq] & bit_all))
			return true;
		b1 &= not_mask[i];
	}
	return false;
}

bool isPinned(const int s, const int sq, const int pinned)
{
	BITBOARD b1 = bit_bishopmoves[sq] & (bit_pieces[s][B] | bit_pieces[s][Q]);
	b1 |= (bit_rookmoves[sq] & (bit_pieces[s][R] | bit_pieces[s][Q]));

	while (b1)
	{
		int i = NextBit(b1);
		if (!(bit_between[i][sq] & (bit_units[0] | bit_units[1]) & not_mask[pinned]))
			return true;
		b1 &= not_mask[i];
	}
	return false;
}

bool isPinnedLinePiece(const int s, const int sq, const int pinned, const int to)
{
	BITBOARD b1 = bit_bishopmoves[sq] & (bit_pieces[s][B] | bit_pieces[s][Q]);
	b1 |= (bit_rookmoves[sq] & (bit_pieces[s][R] | bit_pieces[s][Q]));
	b1 &= not_mask[to];

	while (b1)
	{
		int i = NextBit(b1);
		if (!(bit_between[i][sq] & (bit_units[0] | bit_units[1]) & not_mask[pinned]))
			return true;
		b1 &= not_mask[i];
	}
	return false;
}

int GetLowestAttacker(const int s, const int sq)
{
	if (bit_pawndefends[s][sq] & bit_pieces[s][P])
		return P;
	if (bit_knightmoves[sq] & bit_pieces[s][N])
		return N;

	int i;

	for (int x = 0; x < total[s][B]; x++)
	{
		i = pieces[s][B][x];
		if (bit_bishopmoves[sq] & mask[i])
		{
			if (!(bit_between[i][sq] & bit_all))
			{
				return B;
			}
		}
	}
	for (int x = 0; x < total[s][R]; x++)
	{
		i = pieces[s][R][x];
		if (bit_rookmoves[sq] & mask[i])
		{
			if (!(bit_between[i][sq] & bit_all))
			{
				return R;
			}
		}
	}
	for (int x = 0; x < total[s][Q]; x++)
	{
		i = pieces[s][Q][x];
		if (bit_queenmoves[sq] & mask[i])
		{
			if (!(bit_between[i][sq] & bit_all))
			{
				return Q;
			}
		}
	}
	if (bit_kingmoves[sq] & bit_pieces[s][K])
		return K;
	return -1;
}

