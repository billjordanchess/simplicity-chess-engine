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
				return NextBit(b2);
			}
		}
		b1 &= not_mask[i];
	}
	return 0;
}
