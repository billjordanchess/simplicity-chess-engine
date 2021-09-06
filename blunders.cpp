#include "globals.h"

BITBOARD SetBu(const int s, const int v);

int BlunderOne(const int n)
{
	move_data* g;
	g = &move_list[n];

	if (BestCaptureSquare(side, xside, g->to, b[g->to]) > 0)
	{
		g->score = -piece_value[b[g->to]];
		return 1;
	}

	return 0;
}

int Blunder(const int cv, const int to, const int flags)
{
	int bc = 0;

	//if(cv && flags & CHECK)
   //	 return 0;//??

	if (cv)
	{
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
					return cv - piece_value[b[to]];// + HISTORY_LIMIT +1000;
				}
				if (CheckAttack(side, to))
				{
					return cv - piece_value[b[to]];// + HISTORY_LIMIT +1000;
				}
				return 0;
			}
			if (BestCaptureSquare(side, xside, to, b[to]) > 0)
				bc = piece_value[b[to]];
		}
		else
		{
			BITBOARD bu = SetBu(xside, cv);
			bc = BestCapture2(side, xside, bu);
		}
		if (bc > cv)
		{
			return cv - bc;// + HISTORY_LIMIT +1000;
			//if(bc>500) g->score += HISTORY_LIMIT +1000;
		}
		return 0;
	}
	//end captures

	if (flags & CHECK)
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
	//end check
	bc = BestCapture(side, xside, bit_units[xside]);// & not_mask[to]);//does not need to

	if (bc > 0)
	{
		return -bc;
	}

	return 0;
}

int BlunderThreat(const int threat, const int ts, const int td, const int cv, const int to, const int flags)
{
	if (to != ts)
	{
		//threat = 0;
	}

	int bc;
	//*/
	if (flags & CAPTURE)
	{
		if (piece_value[b[to]] <= cv)
			return 0;
		/*
		BITBOARD bu = bit_units[xside];
		int best = piece_value[game_list[hply-1].capture];
		SetBu(xside,best,bu);
	   bc = BestCapture(side,xside,bu) - 25;
	   */
		bc = BestCapture(side, xside, mask[to] | mask[td]) - 25;//

		if (bc > cv)
		{
			return cv - bc;// + HISTORY_LIMIT +1000;	
		}
		if (to == ts)
		{
			//g->score += threat;
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
				bu = SetBu(xs, best);
			}
		/*
		else
		{
			//b2 = GetScore2(s,xs,sq);
			if(b2>best)
			{
				best = b2;
				bu = SetBu(xs,best);
			}
		}
		//*/
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
					bu = SetBu(xs, best);
				}
			/*
		   else
		   {
			   //b2 = GetScore2(s,xs,sq);
			   if(b2>best)
			   {
				   best = b2;
				   bu = SetBu(xs,best);
			   }
		   }
		   //*/
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
						bu = SetBu(xs, best);
					}
				/*
				   else
				   {
					   //b2 = GetScore2(s,xs,sq);
					   if(b2>best)
					   {
						   best = b2;
						   bu = SetBu(xs,best);
					   }
				   }
				   //*/
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
						bu = SetBu(xs, best);
					}
				/*
				  else
				  {
					  //b2 = GetScore2(s,xs,sq);
					  if(b2>best)
					  {
						  best = b2;
						  bu = SetBu(xs,best);
					  }
				  }
				  //*/
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
				bu &= not_mask[sq];//11/5/14
				if (piece_value[b[sq]] > best)
					if (Attack(xs, sq) == 0)
					{
						best = piece_value[b[sq]];
						bu = SetBu(xs, best);
					}
				/*
			   else
			   {
				   //b2 = GetScore2(s,xs,sq);
				   if(b2>best)
				   {
					   best = b2;
					   bu = SetBu(xs,best);
				   }
			   }
			   //*/
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

BITBOARD SetBu(const int s, const int v)
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
	//
	int diff = alpha - (piece_mat[s] + pawn_mat[s] - piece_mat[xs] - pawn_mat[xs]) - 100;

	if (diff <= -10000)
	{
		//printf("threat");
		//z();
	}
	if (diff < 900)//??- && diff>-10000)
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
	//*/
	//int b3;
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
			}
		/*
		else
		{
		  b3 = GetScore2(s,xs,sq);
		  if(b3>best)
		  {
		   best = b3;
		   threat_start = pawnleft[xs][sq];
		   threat_dest = sq;
		  }
		}
		//*/
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
			}
		/*
		 else
		 {
		   b3 = GetScore2(s,xs,sq);
		   if(b3>best)
		   {
			best = b3;
			threat_start = pawnright[xs][sq];
			threat_dest = sq;
		   }
		}
		//*/
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
						}
					}
					/*
					  else
					  {
						b3 = GetScore2(s,xs,sq);
						if(b3>best)
						{
						 best = b3;
						 threat_start = i;
						 threat_dest = sq;
						}
				 }
				 //*/
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
					}
				/*
				  else
				  {
					b3 = GetScore2(s,xs,sq);
					if(b3>best)
					{
					 best = b3;
					 threat_start = i;
					 threat_dest = sq;
					}
			}
			//*/
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
					}
				/*
				  else
				  {
					b3 = GetScore2(s,xs,sq);
					if(b3>best)
					{
					 best = b3;
					 threat_start = i;
					 threat_dest = sq;
					}
			}
			//*/
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
					}
				/*
					  else
					  {
						b3 = GetScore2(s,xs,sq);
						if(b3>best)
						{
						 best = b3;
						 threat_start = i;
						 threat_dest = sq;
						}

				}
				//*/
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
				//if(piece_value[b[sq]] < best)
				//	z();
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
	//if(GetScore2(s,xs,threat_dest)>0)
	{
		;// return piece_value[b[threat_dest]];
	}
	return 0;
}

void MoveAttacked(const int sq, const int s2, const int ply)
{
	int score = piece_value[b[sq]];//could add near capture score 21/4/17
	BITBOARD bu = 0;
	//
	if (xside == 0)
	{
		bu = (bit_pieces[0][0] & not_a_file) << 7;
		bu |= (bit_pieces[0][0] & not_h_file) << 9;
	}
	else
	{
		bu = (bit_pieces[1][0] & not_a_file) >> 9;
		bu |= (bit_pieces[1][0] & not_h_file) >> 7;
	}
	//
	if (b[sq] > 1)
	{
		if (b[s2] == 3)
		{
			bu |= bit_rookmoves[s2];
		}
		if (b[s2] == 2)
		{
			bu |= bit_bishopmoves[s2];
		}
		if (b[s2] == 1)
		{
			bu |= bit_knightmoves[s2];
		}
	}
	bu &= ~(bit_pieces[xside][1] | bit_pieces[xside][2] | bit_pieces[xside][3] | bit_pieces[xside][4]);

	for (int x = first_move[ply]; x < first_move[ply + 1]; x++)
	{
		if (move_list[x].score <= KILLER_SCORE && move_list[x].score > score && !(move_list[x].flags & CHECK))
			move_list[x].score = 0;
		if (move_list[x].from == sq && !(mask[move_list[x].to] & bu))
		{
			move_list[x].score += score;
		}
	}
	//PrintBitBoard(bu);
	//ShowAll(ply);//??
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
		if (b[sq] == EMPTY && Attack(xs, sq) == 0)//no behind
		{
			return 1;
			//Alg(i,sq);
			//z();
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

		if (b[sq] > 0 || Attack(xs, sq) == 0)
		{
			return 1;
		}
		//*
		else
		{
			if (GetScore2(s, xs, sq) > best)
			{
				return 1;
			}
		}
		//*/
	}

	while (b2)
	{
		sq = NextBit(b2);
		b2 &= not_mask[sq];
		if (b[sq] > 0 || Attack(xs, sq) == 0)
		{
			return 1;
		}
		//*
		else
		{
			if (GetScore2(s, xs, sq) > best)
				return 1;
		}
		//*/
	}
	//*/
	for (x = 0; x < total[s][1]; x++)
	{
		i = pieces[s][1][x];
		b1 = bit_knightmoves[i] & bu;
		while (b1)
		{
			sq = NextBit(b1);
			b1 &= not_mask[sq];

			if (Attack(xs, sq) == 0)
			{
				return 1;
			}
			else
			{
				if (b[sq] > 2)
				{
					return 1;
				}
				//*
				else
				{
					if (GetScore2(s, xs, sq) > best)
					{
						return 1;
					}
				}
				//*/
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
				if (b[sq] > 2 || Attack(xs, sq) == 0)
				{
					return 1;
				}
				//*
				else
				{
					if (GetScore2(s, xs, sq) > best)
					{
						return 1;
					}
				}
				//*/
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
				if (b[sq] > 3 || Attack(xs, sq) == 0)
				{
					return 1;
				}
				//*
				else
				{
					if (GetScore2(s, xs, sq) > best)
					{
						return 1;
					}
				}
				//*/
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
				//*
				else
				{
					if (GetScore2(s, xs, sq) > best)
					{
						return 1;
					}
				}
				//*/
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
		bu &= not_mask[sq];//11/5/14

		if (b[sq] > 0 || Attack(xs, sq) == 0)
		{
			return piece_value[b[sq]];
		}
		else
		{
			//return GetScore2(s,xs,sq);
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

			if (b[sq] > 2 || Attack(xs, sq) == 0)//was >2
			{
				return piece_value[b[sq]];
			}
			else
			{
				//return GetScore2(s,xs,sq);
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
				if (b[sq] > 2 || Attack(xs, sq) == 0)//was ?2
				{
					return piece_value[b[sq]];
				}
				else
				{
					//return GetScore2(s,xs,sq);
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
				if (b[sq] > 3 || Attack(xs, sq) == 0)
				{
					return piece_value[b[sq]];
				}
				else
				{
					//return GetScore2(s,xs,sq);
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
				bu &= not_mask[sq];//11/5/14
				if (b[sq] > 4 || Attack(xs, sq) == 0)
				{
					return piece_value[b[sq]];
				}
				else
				{
					//return GetScore2(s,xs,sq);
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
