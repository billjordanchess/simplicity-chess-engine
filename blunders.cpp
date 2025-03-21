#include "globals.h"

BITBOARD SetTargets(const int s, const int v);
int ThreatScore(const int s, const int xs, const int sq, const int val, const int best);

int p_value[6] = {1,3,3,5,9,0};

int Blunder(const int to, const int flags)
{
	int bc = 0;
	//bc = BestCapture(side, xside, bit_units[xside]);
	bc = BestCaptureSquare(side, xside, to, b[to]);//??20/1/
	if (bc > 0)
	{
		return -bc;
	}
	return 0;
}

int BlunderCapture(const int cv, const int to, const int flags)
{
	int bc = 0;
	if (p_value[b[to]] <= cv)
		return 0;
	if (b[to] == 3 && cv == BVAL)
		return 0;
	if (flags & CHECK)

	{
		if (difference[to][pieces[side][5][0]] == 1)
		{
			if (Attack(xside, to) == 0)
			{
				return cv - p_value[b[to]];
			}
			if (CheckAttack(side, to))
			{
				return cv - p_value[b[to]];
			}
			return 0;
		}
		if (BestCaptureSquare(side, xside, to, b[to]) > 0)
			bc = p_value[b[to]];
	}
	else
	{
		BITBOARD bu = SetTargets(xside, cv);
		bc = BestCapture2(side, xside, bu);
	}
	if (bc > cv)
	{
		return cv - bc;
	}
	return 0;
}

int BlunderCheck(const int to, const int flags)
{
	if (difference[to][pieces[side][5][0]] == 1)
	{
		if (Attack(xside, to) == 0)
		{
			return -p_value[b[to]];
		}
		if (b[to] == 4 && CheckAttack(side, to))
		{
			return -QVAL;
		}
		return 0;
	}
	if (BestCaptureSquare(side, xside, to, b[to]) > 0)
	{
		//return 25 - p_value[b[to]];//more for check 11/5/14
		return -p_value[b[to]];
	}
	return 0;
}

int BlunderThreat(const int threat, const int ts, const int td, const int cv, const int to, const int flags)
{
	int bc;
	if (flags & CAPTURE)
	{
		if (p_value[b[to]] <= cv)
			return 0;
		bc = BestCapture(side, xside, mask[to] | mask[td]);

		if (bc > cv)
		{
			return cv - bc;	
		}
		return 0;
	}
	//end captures

	bc = MakeThreat(side, xside, ts, td);
	if (bc < p_value[b[to]])
	{
		if (BestCaptureSquare(side, xside, to, b[to]) > 0)
			bc = p_value[b[to]];
	}
	if (bc > 0)
	{
		return -bc;
	}

	return 0;
}

int BestCaptureSquare(const int s, const int xs, int sq, int p)
{
	if (bit_pawndefends[s][sq] & bit_pieces[s][0])
	{
		if (p > 0 || Attack(xs, sq) == 0)
			return 1;
		else
			return 0;
	}
	if (bit_knightmoves[sq] & bit_pieces[s][1])
	{
		if (p > 2 || Attack(xs, sq) == 0)
			return 1;
		else
			return 0;
	}

	BITBOARD b1 = bit_bishopmoves[sq] & (bit_pieces[s][2] | bit_pieces[s][4]);
	b1 |= (bit_rookmoves[sq] & (bit_pieces[s][3] | bit_pieces[s][4]));

	while (b1)
	{
		int i = NextBit(b1);
		if (!(bit_between[i][sq] & bit_all))
		{
			if (p > b[i])
				return 1;
			if (Attack(xs, sq) == 0)
				return 1;
			//return GetScore2(s, xs, sq);//??8/9/21
		}
		b1 &= not_mask[i];
	}

	if (bit_kingmoves[sq] & bit_pieces[s][5] && Attack(xs, sq) == 0)
		return 1;
	return 0;
}

int BestCapture2(const int s, const int xs, BITBOARD bu)
{
	int sq, i;
	int best = 0;
	int best2 = 0;

	BITBOARD b1, b2;
	if (s == 0)
	{
		b1 = bu & (((bit_pieces[0][0] & not_h_file) << 9));
		b1 |= bu & (((bit_pieces[0][0] & not_a_file) << 7));
	}
	else
	{
		b1 = bu & (((bit_pieces[1][0] & not_h_file) >> 7));
		b1 |= bu & (((bit_pieces[1][0] & not_a_file) >> 9));
	}

	while (b1)
	{
		sq = NextBit(b1);
		b1 &= not_mask[sq];
		bu &= not_mask[sq];//11/5/14
		if (p_value[b[sq]] > best)
			if (b[sq] > 0 || Attack(xs, sq) == 0)
			{
				best = p_value[b[sq]];
				bu = SetTargets(xs, best);
			}
		else
		{
			best2 = GetScore2(s,xs,sq);
			if(best2 >best)
			{
				best = best2;
				bu = SetTargets(xs,best);
			}
		}
	}
	int x;
	for (x = 0; x < total[s][1]; x++)
	{
		i = pieces[s][1][x];
		b2 = bit_knightmoves[i] & bu;
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			bu &= not_mask[sq];//11/5/14
			if (p_value[b[sq]] > best)
				if (b[sq] > 2 || Attack(xs, sq) == 0)//was >2
				{
					best = p_value[b[sq]];
					bu = SetTargets(xs, best);
				}
		   else
		   {
					best2 = GetScore2(s,xs,sq);
			   if(best2 >best)
			   {
				   best = best2;
				   bu = SetTargets(xs,best);
			   }
		   }
		}
	}

	for (x = 0; x < total[s][2]; x++)
	{
		i = pieces[s][2][x];
		b2 = bit_bishopmoves[i] & bu;
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
			{
				bu &= not_mask[sq];//11/5/14
				if (p_value[b[sq]] > best)
					if (b[sq] > 2 || Attack(xs, sq) == 0)//was ?2
					{
						best = p_value[b[sq]];
						bu = SetTargets(xs, best);
					}
				   else
				   {
						best2 = GetScore2(s,xs,sq);
					   if(best2 >best)
					   {
						   best = best2;
						   bu = SetTargets(xs,best);
					   }
				   }
			}
		}
	}

	for (x = 0; x < total[s][3]; x++)
	{
		i = pieces[s][3][x];
		b2 = bit_rookmoves[i] & bu;
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
			{
				bu &= not_mask[sq];//11/5/14
				if (p_value[b[sq]] > best)
					if (b[sq] > 3 || Attack(xs, sq) == 0)
					{
						best = p_value[b[sq]];
						bu = SetTargets(xs, best);
					}
				  else
				  {
						best2 = GetScore2(s,xs,sq);
					  if(best2 >best)
					  {
						  best = best2;
						  bu = SetTargets(xs,best);
					  }
				  }
			}
		}
	}

	for (x = 0; x < total[s][4]; x++)
	{
		i = pieces[s][4][x];
		b2 = bit_queenmoves[i] & bu;
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
			{
				bu &= not_mask[sq];
				if (p_value[b[sq]] > best)
					if (Attack(xs, sq) == 0)
					{
						best = p_value[b[sq]];
						bu = SetTargets(xs, best);
					}
			   else
			   {
					best2 = GetScore2(s,xs,sq);
				   if(best2 >best)
				   {
					   best = best2;
					   bu = SetTargets(xs,best);
				   }
			   }
			}
		}
	}

	b1 = bit_kingmoves[pieces[s][5][0]] & bu;
	while (b1)
	{
		sq = NextBit(b1);
		b1 &= not_mask[sq];
		if (p_value[b[sq]] > best)
			if (Attack(xs, sq) == 0)
				best = p_value[b[sq]];
	}
	return best;
}

BITBOARD SetTargets(const int s, const int v)
{
	if (v == 1) return bit_pieces[s][1] | bit_pieces[s][2] | bit_pieces[s][3] | bit_pieces[s][4];
	if (v == 3) return bit_pieces[s][3] | bit_pieces[s][4];
	if (v == 5) return bit_pieces[s][4];
	return 0;
}

int GetThreatMove(const int s, const int xs, int& threat_start, int& threat_dest)
{
	int sq, i, x;
	int best = 0;

	BITBOARD b1, b2;

	b1 = bit_pieces[s][0] & mask_ranks[s][6];

	while (b1)
	{
		i = NextBit(b1);
		b1 &= not_mask[i];
		sq = pawnplus[s][i];
		if (b[sq] == EMPTY && Attack(xs, sq) == 0)//no behind
		{
			threat_start = i;
			threat_dest = sq;
			best = 8;
		}
	}

	BITBOARD bu = 0;

	int diff = piece_mat[xs] + pawn_mat[xs] - piece_mat[s] - pawn_mat[s];

	if (diff < 9)
	{
		bu |= bit_pieces[xs][4];
		if (diff < 5)
		{
			bu |= bit_pieces[xs][3];
			if (diff < 3)
			{
				bu |= bit_pieces[xs][1] | bit_pieces[xs][2];
				if (diff < 1)
					bu |= bit_pieces[xs][0];
			}
		}
	}
	if (bu == 0)
	{
		//z();
		return 0;
	}
		//*/
	int b3=0;
	if (s == 0)
	{
		b1 = bu & (((bit_pieces[0][0] & not_h_file) << 9));
		b2 = bu & (((bit_pieces[0][0] & not_a_file) << 7));
	}
	else
	{
		b1 = bu & (((bit_pieces[1][0] & not_h_file) >> 7));
		b2 = bu & (((bit_pieces[1][0] & not_a_file) >> 9));
	}

	while (b1)
	{
		sq = NextBit(b1);
		b1 &= not_mask[sq];
		if (p_value[b[sq]] > best)
			if (b[sq] > 0 || Attack(xs, sq) == 0)
			{
				best = p_value[b[sq]];
				threat_start = pawnleft[xs][sq];
				threat_dest = sq;
				bu = SetTargets(xs, best);
			}
		else
		{
		  b3 = GetScore2(s,xs,sq);
		  if(b3>best)
		  {
		   best = b3;
		   threat_start = pawnleft[xs][sq];
		   threat_dest = sq;
		   bu = SetTargets(xs, best);
		  }
		}
	}
	while (b2)
	{
		sq = NextBit(b2);
		b2 &= not_mask[sq];
		if (p_value[b[sq]] > best)
			if (b[sq] > 0 || Attack(xs, sq) == 0)
			{
				best = p_value[b[sq]];
				threat_start = pawnright[xs][sq];
				threat_dest = sq;
				bu = SetTargets(xs, best);
			}
		 else
		 {
		   b3 = GetScore2(s,xs,sq);
		   if(b3>best)
		   {
			best = b3;
			threat_start = pawnright[xs][sq];
			threat_dest = sq;
			bu = SetTargets(xs, best);
		   }
		}
	}
	for (x = 0; x < total[s][1]; x++)
	{
		i = pieces[s][1][x];
		b1 = bit_knightmoves[i] & bu;
		while (b1)
		{
			sq = NextBit(b1);
			b1 &= not_mask[sq];
			if (p_value[b[sq]] > best)
			{
				if (Attack(xs, sq) == 0)
				{
					best = p_value[b[sq]];
					threat_start = i;
					threat_dest = sq;
					bu = SetTargets(xs, best);
				}
				else
				{
					if (b[sq] > 2)
					{
						if (b[sq] - 3 > best)
						{
							best = b[sq] - 3;
							threat_start = i;
							threat_dest = sq;
							bu = SetTargets(xs, best);
						}
					}
					  else
					  {
						b3 = GetScore2(s,xs,sq);
						if(b3>best)
						{
						 best = b3;
						 threat_start = i;
						 threat_dest = sq;
						 bu = SetTargets(xs, best);
						}
				 }
				}
			}
		}
	}
	//threat_start line pieces
	for (x = 0; x < total[s][2]; x++)
	{
		i = pieces[s][2][x];
		b1 = bit_bishopmoves[i] & bu;
		while (b1)
		{
			sq = NextBit(b1);
			b1 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
			{
				if (p_value[b[sq]] > best)
					if (b[sq] > 2 || Attack(xs, sq) == 0)
					{
						best = p_value[b[sq]];
						threat_start = i;
						threat_dest = sq;
						bu = SetTargets(xs, best);
					}
				  else
				  {
					b3 = GetScore2(s,xs,sq);
					if(b3>best)
					{
					 best = b3;
					 threat_start = i;
					 threat_dest = sq;
					 bu = SetTargets(xs, best);
					}
			}
			}
		}
	}

	for (x = 0; x < total[s][3]; x++)
	{
		i = pieces[s][3][x];
		b1 = bit_rookmoves[i] & bu;
		while (b1)
		{
			sq = NextBit(b1);
			b1 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
			{
				if (p_value[b[sq]] > best)
					if (b[sq] > 3 || Attack(xs, sq) == 0)
					{
						best = p_value[b[sq]];
						threat_start = i;
						threat_dest = sq;
						bu = SetTargets(xs, best);
					}
				  else
				  {
					b3 = GetScore2(s,xs,sq);
					if(b3>best)
					{
					 best = b3;
					 threat_start = i;
					 threat_dest = sq;
					 bu = SetTargets(xs, best);
					}
			}
			}
		}
	}

	for (x = 0; x < total[s][4]; x++)
	{
		i = pieces[s][4][x];
		b1 = bit_queenmoves[i] & bu;
		while (b1)
		{
			sq = NextBit(b1);
			b1 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
			{
				if (p_value[b[sq]] > best)
					if (Attack(xs, sq) == 0)
					{
						best = p_value[b[sq]];
						threat_start = i;
						threat_dest = sq;
						bu = SetTargets(xs, best);
					}
					  else
					  {
						b3 = GetScore2(s,xs,sq);
						if(b3>best)
						{
						 best = b3;
						 threat_start = i;
						 threat_dest = sq;
						 bu = SetTargets(xs, best);
						}
				}
			}
		}
	}

	i = pieces[s][5][0];
	b1 = bit_kingmoves[i] & bu;
	while (b1)
	{
		sq = NextBit(b1);
		b1 &= not_mask[sq];
		if (p_value[b[sq]] > best)
			if (Attack(xs, sq) == 0)
			{
				best = p_value[b[sq]];
				threat_start = i;
				threat_dest = sq;
			}
	}
	return best;
}

int MakeThreat(const int s, const int xs, const int threat_start, const int threat_dest)
{
	if (bit_between[threat_start][threat_dest] & bit_all)
	{
		return 0;
	}
	if (p_value[b[threat_start]] < p_value[b[threat_dest]])
	{
		return p_value[b[threat_dest]];
	}
	if (Attack(xs, threat_dest) == 0)
	{
		return p_value[b[threat_dest]];
	}
	if(GetScore2(s,xs,threat_dest)>0)
	{
		 return p_value[b[threat_dest]];
	}
	return 0;
}

int BestThreat(const int s, const int xs, const int diff)
{
	if (diff >= 9)
		return 0;
	int sq, i, x;
	
	int best = 0;

	BITBOARD b1, b2;

	b1 = bit_pieces[s][0] & mask_ranks[s][6];

	while (b1)
	{
		i = NextBit(b1);
		b1 &= not_mask[i];
		sq = pawnplus[s][i];
		if (b[sq] == EMPTY && Attack(xs, sq) == 0 && 
			!(mask_cols[i] & mask[pieces[xs][3][0]] && !(bit_between[i][NextBit(pieces[xs][3][0])] & bit_all)))
		{
			return 1;
		}
	}

	BITBOARD bu = bit_pieces[xs][4];

	if (diff < 5)
	{
		bu |= bit_pieces[xs][3];
		if (diff < 3)
		{
			bu |= bit_pieces[xs][1] | bit_pieces[xs][2];
			if (diff < 1)
				bu |= bit_pieces[xs][0];
		}
	}
	if (bu == 0)
		return 0;

	if (s == 0)
	{
		b1 = bu & (((bit_pieces[0][0] & not_h_file) << 9));
		b2 = bu & (((bit_pieces[0][0] & not_a_file) << 7));
	}
	else
	{
		b1 = bu & (((bit_pieces[1][0] & not_h_file) >> 7));
		b2 = bu & (((bit_pieces[1][0] & not_a_file) >> 9));
	}

	while (b1)
	{
		sq = NextBit(b1);
		b1 &= not_mask[sq];
		if (ThreatScore(s, xs, sq, 0, best))
			return 1;
	}

	while (b2)
	{
		sq = NextBit(b2);
		b2 &= not_mask[sq];
		if (ThreatScore(s, xs, sq, 0, best))
			return 1;
	}

	for (x = 0; x < total[s][1]; x++)
	{
		i = pieces[s][1][x];
		b1 = bit_knightmoves[i] & bu;
		while (b1)
		{
			sq = NextBit(b1);
			b1 &= not_mask[sq];
			if (ThreatScore(s, xs, sq, 2, best))
				return 1;
		}
	}

	for (x = 0; x < total[s][2]; x++)
	{
		i = pieces[s][2][x];
		b1 = bit_bishopmoves[i] & bu;
		while (b1)
		{
			sq = NextBit(b1);
			b1 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
			{
				if (ThreatScore(s, xs, sq, 2, best))
					return 1;
			}
		}
	}

	for (x = 0; x < total[s][3]; x++)
	{
		i = pieces[s][3][x];
		b1 = bit_rookmoves[i] & bu;
		while (b1)
		{
			sq = NextBit(b1);
			b1 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
			{
				if(ThreatScore(s, xs, sq, 3, best))
					return 1;
			}
		}
	}

	for (x = 0; x < total[s][4]; x++)
	{
		i = pieces[s][4][x];
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
					if (GetScore2(s, xs, sq) > best)
					{
						return 1;
					}
				}
			}
		}
	}

	i = pieces[s][5][0];
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

int ThreatScore(const int s,const int xs,const int sq,const int p,const int best)
{
	if (b[sq] > p  || Attack(xs, sq) == 0)
		return 1;
	else
	{
		if (GetScore2(s, xs, sq) > best)
			return 1;
	}
	return 0;
}

int BestCapture(const int s, const int xs, BITBOARD bu)
{
	int sq, i;
	BITBOARD b1, b2;
	if (s == 0)
	{
		b1 = bu & (((bit_pieces[0][0] & not_h_file) << 9));
		b1 |= bu & (((bit_pieces[0][0] & not_a_file) << 7));
	}
	else
	{
		b1 = bu & (((bit_pieces[1][0] & not_h_file) >> 7));
		b1 |= bu & (((bit_pieces[1][0] & not_a_file) >> 9));
	}

	while (b1)
	{
		sq = NextBit(b1);
		b1 &= not_mask[sq];
		bu &= not_mask[sq];

		if (b[sq] > 0 || Attack(xs, sq) == 0)
		{
			return p_value[b[sq]];
		}
	}
	int x;
	for (x = 0; x < total[s][1]; x++)
	{
		i = pieces[s][1][x];
		b2 = bit_knightmoves[i] & bu;
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			bu &= not_mask[sq];

			if (b[sq] > 2 || Attack(xs, sq) == 0)
			{
				return p_value[b[sq]];
			}
		}
	}

	for (x = 0; x < total[s][2]; x++)
	{
		i = pieces[s][2][x];
		b2 = bit_bishopmoves[i] & bu;
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
			{
				bu &= not_mask[sq];
				if (b[sq] > 2 || Attack(xs, sq) == 0)
				{
					return p_value[b[sq]];
				}
			}
		}
	}

	for (x = 0; x < total[s][3]; x++)
	{
		i = pieces[s][3][x];
		b2 = bit_rookmoves[i] & bu;
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
			{
				bu &= not_mask[sq];
				if (b[sq] > 3 || Attack(xs, sq) == 0)
				{
					return p_value[b[sq]];
				}
			}
		}
	}

	for (x = 0; x < total[s][4]; x++)
	{
		i = pieces[s][4][x];
		b2 = bit_queenmoves[i] & bu;
		while (b2)
		{
			sq = NextBit(b2);
			b2 &= not_mask[sq];
			if (!(bit_between[i][sq] & bit_all))
			{
				bu &= not_mask[sq];
				if (Attack(xs, sq) == 0)
				{
					return p_value[b[sq]];
				}
				else
				{
				    return GetScore2(s, xs, sq);
				}
			}
		}
	}

	b1 = bit_kingmoves[pieces[s][5][0]] & bu;
	while (b1)
	{
		sq = NextBit(b1);
		b1 &= not_mask[sq];
		if (Attack(xs, sq) == 0)
			return p_value[b[sq]];
	}
	return 0;
}
