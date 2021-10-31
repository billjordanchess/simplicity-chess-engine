#include "globals.h"

int GetScore2(const int s, const int xs, const int n);

void PopSwap()
{
	int key1, key2;
	int s = 0, xs = 1;
	int bb, cc, dd, ee, bb1, cc1, dd1, ee1;
	for (int a = 0; a < 5; a++)
	{
		for (int b0 = 0; b0 < 3; b0++)
			for (int c = 0; c < 4; c++)
				for (int d = 0; d < 3; d++)
					for (int e = 0; e < 2; e++)
						for (int f = 0; f < 3; f++)
							for (int k = 0; k < 2; k++)
								for (int b1 = 0; b1 < 3; b1++)
									for (int c1 = 0; c1 < 4; c1++)
										for (int d1 = 0; d1 < 3; d1++)
											for (int e1 = 0; e1 < 2; e1++)
												for (int f1 = 0; f1 < 3; f1++)
													for (int k1 = 0; k1 < 2; k1++)
													{
														key1 = b0 + c * 3 + d * 12 + e * 36;
														key2 = b1 + c1 * 3 + d1 * 12 + e1 * 36;
														//genlists
														Num[0] = Num[1] = 0;
														memset(List, 0, sizeof(List));
														List[1][Num[1]++] = p_value[a];

														bb = b0; cc = c; dd = d; ee = e;
														bb1 = b1; cc1 = c1; dd1 = d1; ee1 = e1;

														while (bb)
														{
															List[0][Num[0]++] = 1;
															bb--;
														}
														while (cc)
														{
															List[0][Num[0]++] = 3;
															cc--;
														}
														while (dd)
														{
															List[0][Num[0]++] = 5;
															dd--;
														}
														while (ee)
														{
															List[0][Num[0]++] = 9;
															ee--;
														}

														if (f == 1)
															List[0][Num[0]++] = 3;
														if (f == 2)
															List[0][Num[0]++] = 5;

														while (bb1)
														{
															List[1][Num[1]++] = 1;
															bb1--;
														}
														while (cc1)
														{
															List[1][Num[1]++] = 3;
															cc1--;
														}
														while (dd1)
														{
															List[1][Num[1]++] = 5;
															dd1--;
														}
														while (ee1)
														{
															List[1][Num[1]++] = 9;
															ee1--;
														}

														if (f1 == 1)
															List[1][Num[1]++] = 3;
														if (f1 == 2)
															List[1][Num[1]++] = 5;

														if (k == 1)
															List[0][Num[0]++] = 1;
														if (k1 == 1)
															List[1][Num[1]++] = 1;

														int sc = Score(s, xs);
														if (sc < -90)
															sc = 0;

														Swap[a][key1][key2][f][f1][k][k1] = sc;
													}
	}
	//Swap[3][6][4][0][0][0][0] = 200;//
	Swap[0][36][0][0][0][0][1] = 0;//13/3
	Swap[0][3][0][0][0][0][1] = 0;//13/3
}

int Score(const int s, const int xs)
{
	int score[2];
	score[0] = score[1] = 0;

	for (int x = 0; x < Num[s]; x++)
	{
		if (!List[xs][x + 1])
		{
			score[s] += List[xs][x];
			break;
		}
		if (List[s][x] > List[xs][x] + List[xs][x + 1])
			break;
		score[s] += List[xs][x];

		if (!List[s][x + 1])
		{
			score[xs] += List[s][x];
			break;
		}
		if (!List[xs][x + 2] && List[xs][x + 1] > List[s][x])//wj
			break;
		//old was just x
		if (List[xs][x + 1] > List[s][x] + List[s][x + 1])//31/5/12
			break;
		score[xs] += List[s][x];

		if (x >= 7) break;
	}
	return score[s] - score[xs];
}

int GetScore2(const int s, const int xs, const int n)
{
	behind_queen = 0;
	king_defends = 0;
	int attack_key = GenKey(s, xs, n);
	int b1 = behind_queen;
	int k1 = king_defends;

	behind_queen = 0;
	king_defends = 0;
	int defend_key = GenKey(xs, s, n);
	int score = Swap[b[n]][attack_key][defend_key][b1][behind_queen][k1][king_defends];

	return score;
}

int GenKey(const int s, const int xs, const int n)
{
	int sq;
	int key = 0;
	int x;

	BITBOARD bit_new = bit_units[0] | bit_units[1];

	if (bit_pieces[s][0] & bit_left[xs][n])
	{
		key += 1;
		bit_new ^= bit_left[xs][n];
	}
	if (bit_pieces[s][0] & bit_right[xs][n])
	{
		key += 1;
		bit_new ^= bit_right[xs][n];
	}

	BITBOARD b1;
	b1 = bit_knightmoves[n] & bit_pieces[s][1];
	while (b1)
	{
		sq = NextBit(b1);
		b1 &= not_mask[sq];
		key += 3;
	}

	b1 = bit_bishopmoves[n] & bit_pieces[s][2];
	if (b1)
	{
		sq = NextBit(b1);
		if (!(bit_between[n][sq] & bit_new))
		{
			key += 3;
			bit_new ^= mask[sq];
		}
	}

	b1 = bit_rookmoves[n] & bit_pieces[s][3];
	while (b1)
	{
		sq = NextBit(b1);
		b1 &= not_mask[sq];
		if (!(bit_between[n][sq] & bit_new & ~bit_pieces[s][3]))
		{
			key += 12;
			bit_new ^= mask[sq];
		}
	}

	int q;

	for (int y = 0; y < total[s][4]; y++)
	{
		q = pieces[s][4][y];
		if ((bit_queenmoves[n] & mask[q]) && !(bit_between[n][q] & bit_new))
		{
			key += 36;
			bit_new ^= mask[q];

			for (x = 0; x < total[s][2]; x++)
			{
				sq = pieces[s][2][x];
				if ((bit_bishopmoves[sq] & mask[n]) &&
					!(bit_between[sq][n] & bit_new) && bit_new & mask[sq])
					behind_queen = 1;
			}
			b1 = bit_rookmoves[n] & bit_pieces[s][3];
			while (b1)
			{
				sq = NextBit(b1);
				b1 &= not_mask[sq];
				if (!(bit_between[sq][n] & bit_new) && (bit_new & mask[sq]))
					behind_queen = 2;
			}
		}
	}

	if (bit_kingmoves[n] & bit_pieces[s][5])
		king_defends = 1;
	return key;
}
