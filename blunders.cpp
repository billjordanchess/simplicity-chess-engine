#include "globals.h"

BITBOARD SetTargets(const int s, const int v);
int ThreatScore(const int s, const int xs, const int sq, const int val, const int best);

int Blunder(const int to, const int flags)
{
	int bc = 0;
	bc = BestCapture(side, xside, bit_units[xside]);
	if (bc > 0)
	{
		return -bc;
	}
	return 0;
}

int BlunderCapture(const int cv, const int to, const int flags)
{
	int bc = 0;
	if (piece_value[b[to]] <= cv)
		return 0;
	if (b[to] == 3 && cv == 300)
		return 0;
	if (flags & CHECK)

	{
		if (difference[to][pieces[side][5][0]] == 1)
		{
			if (Attack(xside, to) == 0)
			{
				return cv - piece_value[b[to]];
			}
			if (CheckAttack(side, to))
			{
				return cv - piece_value[b[to]];
			}
			return 0;
		}
		if (BestCaptureSquare(side, xside, to, b[to]) > 0)
			bc = piece_value[b[to]];
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
			return -piece_value[b[to]];
		}
		if (b[to] == 4 && CheckAttack(side, to))
		{
			return -900;
		}
		return 0;
	}
	if (BestCaptureSquare(side, xside, to, b[to]) > 0)
	{
		return 25 - piece_value[b[to]];//more for check 11/5/14
	}
	return 0;
}

int BlunderThreat(const int threat, const int ts, const int td, const int cv, const int to, const int flags)
{
	int bc;
	if (flags & CAPTURE)
	{
		if (piece_value[b[to]] <= cv)
			return 0;
		bc = BestCapture(side, xside, mask[to] | mask[td]) - 25;//

		if (bc > cv)
		{
			return cv - bc;	
		}
		return 0;
	}
	//end captures

	bc = MakeThreat(side, xside, ts, td);
	if (bc < piece_value[b[to]])
	{
		if (BestCaptureSquare(side, xside, to, b[to]) > 0)
			bc = piece_value[b[to]];
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
		if (piece_value[b[sq]] > best)
			if (b[sq] > 0 || Attack(xs, sq) == 0)
			{
				best = piece_value[b[sq]];
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
			if (piece_value[b[sq]] > best)
				if (b[sq] > 2 || Attack(xs, sq) == 0)//was >2
				{
					best = piece_value[b[sq]];
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
				if (piece_value[b[sq]] > best)
					if (b[sq] > 2 || Attack(xs, sq) == 0)//was ?2
					{
						best = piece_value[b[sq]];
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
				if (piece_value[b[sq]] > best)
					if (b[sq] > 3 || Attack(xs, sq) == 0)
					{
						best = piece_value[b[sq]];
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
				if (piece_value[b[sq]] > best)
					if (Attack(xs, sq) == 0)
					{
						best = piece_value[b[sq]];
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
		if (piece_value[b[sq]] > best)
			if (Attack(xs, sq) == 0)
				best = piece_value[b[sq]];
	}
	return best;
}

BITBOARD SetTargets(const int s, const int v)
{
	if (v == 100) return bit_pieces[s][1] | bit_pieces[s][2] | bit_pieces[s][3] | bit_pieces[s][4];
	if (v == 300) return bit_pieces[s][3] | bit_pieces[s][4];
	if (v == 500) return bit_pieces[s][4];
	return 0;
}

int GetThreatMove(const int s, const int xs, int& threat_start, int& threat_dest, const int alpha)//threat to capture
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
			best = 800;
		}
	}

	BITBOARD bu = 0;
	
	int diff = alpha - (piece_mat[s] + pawn_mat[s] - piece_mat[xs] - pawn_mat[xs]) - 100;

	if (diff < 900)
	{
		bu |= bit_pieces[xs][4];
		if (diff < 500)
		{
			bu |= bit_pieces[xs][3];
			if (diff < 300)
			{
				bu |= bit_pieces[xs][1] | bit_pieces[xs][2];
				if (diff < 100)
					bu |= bit_pieces[xs][0];
			}
		}
	}
	if (bu == 0)
		return 0;
	int b3;
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
		if (piece_value[b[sq]] > best)
			if (b[sq] > 0 || Attack(xs, sq) == 0)
			{
				best = piece_value[b[sq]];
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
		if (piece_value[b[sq]] > best)
			if (b[sq] > 0 || Attack(xs, sq) == 0)
			{
				best = piece_value[b[sq]];
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
			if (piece_value[b[sq]] > best)
			{
				if (Attack(xs, sq) == 0)
				{
					best = piece_value[b[sq]];
					threat_start = i;
					threat_dest = sq;
					bu = SetTargets(xs, best);
				}
				else
				{
					if (b[sq] > 2)
					{
						if (b[sq] - 300 > best)
						{
							best = b[sq] - 300;
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
				if (piece_value[b[sq]] > best)
					if (b[sq] > 2 || Attack(xs, sq) == 0)
					{
						best = piece_value[b[sq]];
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
				if (piece_value[b[sq]] > best)
					if (b[sq] > 3 || Attack(xs, sq) == 0)
					{
						best = piece_value[b[sq]];
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
				if (piece_value[b[sq]] > best)
					if (Attack(xs, sq) == 0)
					{
						best = piece_value[b[sq]];
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
		if (piece_value[b[sq]] > best)
			if (Attack(xs, sq) == 0)
			{
				best = piece_value[b[sq]];
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
	if (piece_value[b[threat_start]] < piece_value[b[threat_dest]])
	{
		return piece_value[b[threat_dest]];
	}
	if (Attack(xs, threat_dest) == 0)
	{
		return piece_value[b[threat_dest]];
	}
	if(GetScore2(s,xs,threat_dest)>0)
	{
		 return piece_value[b[threat_dest]];
	}
	return 0;
}

void MoveAttacked(const int xs,const int sq, const int attacker, const int ply)
{
	if (b[sq] == 6)
	{
		int from,to;
		for (int x = first_move[ply]; x < first_move[ply + 1]; x++)
		{
			from = move_list[x].from;
			to = move_list[x].to;
			if (b[from] == 1 && bit_knightmoves[to] & mask[sq]
				|| (b[from] == 2 && bit_bishopmoves[to] & mask[sq] && !(bit_between[to][sq] & bit_all))
				|| (b[from] == 3 && bit_rookmoves[to] & mask[sq] && !(bit_between[to][sq] & bit_all))
				|| (b[from] == 4 && bit_queenmoves[to] & mask[sq] && !(bit_between[to][sq] & bit_all))
				|| (b[from] == 5 && bit_kingmoves[to] & mask[sq])
				)
			{
				move_list[x].score += 800;
			}
		}
		return;
	}
	int score = piece_value[b[sq]];

	BITBOARD bu = 0;
	if (LookUpPawn())
	{
		bu = GetHashPawnAttacks(xs);
	}
	else
	{
		if (xs == 0)
		{
			bu = (bit_pieces[0][0] & not_a_file) << 7;
			bu |= (bit_pieces[0][0] & not_h_file) << 9;
		}
		else
		{
			bu = (bit_pieces[1][0] & not_a_file) >> 9;
			bu |= (bit_pieces[1][0] & not_h_file) >> 7;
		}
	}
	if (b[sq] > 1)
	{
		if (b[attacker] == 3)
		{
			bu |= bit_rookmoves[attacker];
		}
		else if (b[attacker] == 2)
		{
			bu |= bit_bishopmoves[attacker];
		}
		else if (b[attacker] == 1)
		{
			bu |= bit_knightmoves[attacker];
		}
		else if (b[attacker] == 4)
		{
			bu |= bit_queenmoves[attacker];
		}
	}
	bu &= ~(bit_pieces[xs][1] | bit_pieces[xs][2] | bit_pieces[xs][3] | bit_pieces[xs][4]);

	for (int x = first_move[ply]; x < first_move[ply + 1]; x++)
	{   
		if (move_list[x].flags & CAPTURE && b[move_list[x].to] >= b[sq])
		{
			if(move_list[x].to == attacker)
				move_list[x].score += score;
			continue;
		}
		if (move_list[x].flags & CHECK)
			continue;
		if (move_list[x].from == sq && !(mask[move_list[x].to] & bu))
		{
			move_list[x].score += score + KILLER_SCORE;
			if (x < first_move[ply + 1]-1 && move_list[x+1].from != sq)
				break;
		}
	}
}

int BestThreat(const int s, const int xs, const int diff)
{
	if (diff >= 900)
		return 0;
	int sq, i, x;
	int best = diff;

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

	if (diff < 500)
	{
		bu |= bit_pieces[xs][3];
		if (diff < 300)
		{
			bu |= bit_pieces[xs][1] | bit_pieces[xs][2];
			if (diff < 100)
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
			return piece_value[b[sq]];
		}
		else
		{
			;// return GetScore2(s, xs, sq);
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
				return piece_value[b[sq]];
			}
			else
			{
				;// return GetScore2(s, xs, sq);
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
					return piece_value[b[sq]];
				}
				else
				{
					;// return GetScore2(s, xs, sq);
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
					return piece_value[b[sq]];
				}
				else
				{
					;// return GetScore2(s, xs, sq);
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
				if (b[sq] > 4 || Attack(xs, sq) == 0)
				{
					return piece_value[b[sq]];
				}
				else
				{
					;// return GetScore2(s, xs, sq);
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
			return piece_value[b[sq]];
	}
	return 0;
}
