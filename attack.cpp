#include "globals.h"

bool Attack(const int s, const int sq)
{
	if (bit_pawndefends[s][sq] & bit_pieces[s][0])
		return true;
	if (bit_knightmoves[sq] & bit_pieces[s][1])
		return true;

	int i;
	BITBOARD b1 = bit_rookmoves[sq] & (bit_pieces[s][3] | bit_pieces[s][4]);
	b1 |= (bit_bishopmoves[sq] & (bit_pieces[s][2] | bit_pieces[s][4]));

	while (b1)
	{
		i = NextBit(b1);
		if (!(bit_between[i][sq] & bit_all))
		{
			return true;
		}
		b1 &= not_mask[i];
	}

	if (bit_kingmoves[sq] & bit_pieces[s][5])
		return true;
	return false;
}

int Check(const int s, const int sq)
{
	int i;
	if (bit_knightmoves[sq] & bit_pieces[s][1])
	{
		for (int x = 0; x < total[s][1]; x++)
		{
			i = pieces[s][1][x];
			if (bit_knightmoves[sq] & mask[i])
				return i;
		}
	}
	if (bit_pawndefends[s][sq] & bit_pieces[s][0])
	{
		if (bit_left[!s][sq] & bit_pieces[s][0])
		{
			return pawnleft[!s][sq];
		}
		if (bit_right[!s][sq] & bit_pieces[s][0])
		{
			return pawnright[!s][sq];
		}
	}
	BITBOARD b1 = bit_bishopmoves[sq] & (bit_pieces[s][2] | bit_pieces[s][4]);
	b1 |= (bit_rookmoves[sq] & (bit_pieces[s][3] | bit_pieces[s][4]));

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
	if (bit_knightmoves[sq] & bit_pieces[s][1])
		return true;
	if (bit_pawndefends[s][sq] & bit_pieces[s][0])
		return true;
	BITBOARD b1 = bit_bishopmoves[sq] & (bit_pieces[s][2] | bit_pieces[s][4]);
	b1 |= (bit_rookmoves[sq] & (bit_pieces[s][3] | bit_pieces[s][4]));
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

bool IsCheck(const int s, const int p, const int to, const int sq)
{
	if (p == 1)
	{
		if (bit_knightmoves[to] & mask[sq])
			return true;
		else return false;
	}
	if (p == 0)
	{
	if (bit_pawndefends[s][sq] & mask[to])
		return true;
	else return false;
	}
	if (p == 3)
	{
		if (bit_rookmoves[to] & mask[sq] && !(bit_between[to][sq] & bit_all))
			return true;
		else return false;
	}
	if (p == 2)
	{
		if (bit_bishopmoves[to] & mask[sq] && !(bit_between[to][sq] & bit_all))
			return true;
		else return false;
	}
	if (p == 4)
	{
		if (bit_queenmoves[to] & mask[sq] && !(bit_between[to][sq] & bit_all))
			return true;
		else return false;
	}
	return false;
}

bool LineAttack(const int s, const int sq)
{
	BITBOARD b1 = bit_bishopmoves[sq] & (bit_pieces[s][2] | bit_pieces[s][4]);
	b1 |= (bit_rookmoves[sq] & (bit_pieces[s][3] | bit_pieces[s][4]));

	while (b1)
	{
		int i = NextBit(b1);
		if (!(bit_between[i][sq] & bit_all))
			return true;
		b1 &= not_mask[i];
	}
	return false;
}

bool LineAttack1(const int s, const int sq, const int pinned)
{
	BITBOARD b1 = bit_bishopmoves[sq] & (bit_pieces[s][2] | bit_pieces[s][4]);
	b1 |= (bit_rookmoves[sq] & (bit_pieces[s][3] | bit_pieces[s][4]));

	while (b1)
	{
		int i = NextBit(b1);
		if (!(bit_between[i][sq] & (bit_units[0] | bit_units[1]) & not_mask[pinned]))
			return true;
		b1 &= not_mask[i];
	}
	return false;
}

bool LineAttack2(const int s, const int sq, const int pinned, const int dest)
{
	BITBOARD b1 = bit_bishopmoves[sq] & (bit_pieces[s][2] | bit_pieces[s][4]);
	b1 |= (bit_rookmoves[sq] & (bit_pieces[s][3] | bit_pieces[s][4]));
	b1 &= not_mask[dest];

	while (b1)
	{
		int i = NextBit(b1);
		if (!(bit_between[i][sq] & (bit_units[0] | bit_units[1]) & not_mask[pinned]))
			return true;
		b1 &= not_mask[i];
	}
	return false;
}

int Disco(const int s, const int sq)
{
	BITBOARD b1 = bit_bishopmoves[sq] & (bit_pieces[s][2] | bit_pieces[s][4]);
	b1 |= (bit_rookmoves[sq] & (bit_pieces[s][3] | bit_pieces[s][4]));
	BITBOARD b2;

	while (b1)
	{
		int i = NextBit(b1);
		if (!(bit_between[i][sq] & bit_units[!s]))
		{
			b2 = bit_between[i][sq] & bit_units[s];
			if ((b2 & b2 - 1) == 0 && 
				!(bit_between[i][sq] & bit_pieces[s][0] && mask_cols[i]==mask_cols[sq]))
			{
				//Alg(i, NextBit(b2));
				//z();
				//return -1;//
				return NextBit(b2);
			}
		}
		b1 &= not_mask[i];
	}
	return -1;
}
