#include <stdlib.h>
#include "globals.h"

#define STALEMATE 0

const int MAX1 = MAX_PLY - 1;
const int MAX2 = MAX_PLY - 2;

extern int deep;
extern int root_score;
extern int currentdepth;

void CheckUp();

int Sort(const int from, const int, const int);
void SortCaptures(const int from, const int last);
int check_history[6][64];

void z();//

void SetRootScore();

void ShowMoves(int);

int MakeCapture(const int, const int, const int);
void UnMakeCapture();
void MakeRecapture(const int, const int);
void UnMakeRecapture();

int CaptureSearch(int alpha, const int beta, const int depth);
int RecaptureSearch(int alpha, const int beta, const int depth, const int sq);
int GenRecaptures(const int diff, const int to);

int BlockedPawns(const int s);

int QuietSearch(int alpha, int beta, int depth)
{
	nodes++;

	if (nodes % 4096 == 0)
		CheckUp();
	if (ply > MAX2)
		return Eval(alpha, beta);

	int from, to, flags, score;
	int check = Check(xside, pieces[side][K][0]);

	currentdepth = depth;

	//start evasion
	if (check > -1)
	{
		first_move[ply + 1] = first_move[ply];
		Evasion(check);
		//EvadeCapture(check);
		
		//EvadeQuiet(check);

		if (first_move[ply] == first_move[ply + 1])
		{
			return ply - 10000;
		}

		int count = 0;
		int top = HASH_SCORE;

		for (int i = first_move[ply]; i < first_move[ply + 1]; i++)
		{
			top = Sort(i, top, first_move[ply + 1]);
			from = move_list[i].from;
			to = move_list[i].to;

			if (!MakeCapture(from, to, move_list[i].flags))
			{
				continue;
			}

			count++;
			score = -QuietSearch(-beta, -alpha, depth - 1);
			UnMakeCapture();

			if (score > alpha)
			{
				if (score >= beta)
				{
					return beta;
				}
				alpha = score;
			}
		}
		if (count == 0)
		{
			return -10000 + ply;
		}
		return alpha;
	}
	//end evasion

	if (piece_mat[side] == 0 && BlockedPawns(side) == 1)
	{
		if (SafeKingMoves(side, xside) == 0)
			return 0;
	}

	int lookup = LookUp2(side);
	if (lookup == -1)
		score = Eval(alpha, beta);
	else
	{
		score = hash_move.score;
	}
	if (score >= beta)
	{
		return beta;
	}
	if (score > alpha)
	{
		alpha = score;
	}

	if (depth > -2)
		GenCapsChecks((alpha - score) / 100, depth);
	else
		GenQuietCaptures((alpha - score) / 100);

	if (first_move[ply] == first_move[ply + 1])
	{
		return alpha;
	}
	if (depth <= -4)
	{
		return CaptureSearch(alpha, beta, depth);
	}

	if (first_move[ply] + 1 == first_move[ply + 1])
	{
		from = move_list[first_move[ply]].from;
		to = move_list[first_move[ply]].to;
		flags = move_list[first_move[ply]].flags;

		if (MakeCapture(from, to, flags))
		{
			score = -QuietSearch(-beta, -alpha, depth);
			extend[ply] = 1;

			UnMakeCapture();
			if (score > alpha)
			{
				if (score >= beta)
				{
					AddHash(side, depth, score, 0, from, to, flags);
					return beta;
				}
				AddHash(side, depth, score, 0, from, to, flags);
				return score;
			}
		}
		return alpha;
	}

	int top = HASH_SCORE;
	for (int i = first_move[ply]; i < first_move[ply + 1]; i++)
	{
		top = Sort(i, top, first_move[ply + 1]);
		from = move_list[i].from;
		to = move_list[i].to;
		flags = move_list[i].flags;

		if (!MakeCapture(from, to, flags))
		{
			continue;
		}
	
		if (piece_value[b[to]] < piece_value[game_list[hply - 1].capture] &&
			game_list[hply - 1].capture != EMPTY)
		{
			score = -QuietSearch(-beta, -alpha, depth);
		}
		else
			score = -QuietSearch(-beta, -alpha, depth - 1);

		UnMakeCapture();

		if (score > alpha)
		{
			if (score >= beta)
			{
				if (move_list[i].flags & CHECK)
				{
					if (check_history[b[from]][to] < HISTORY_LIMIT)
						check_history[b[from]][to] ++;
					else
						check_history[b[from]][to] /= 2;
					if (score > 9900 && check_history[b[from]][to] < CHECK_SCORE)
						check_history[b[from]][to] = CHECK_SCORE;
				}
				AddHash(side, depth, score, 0, from, to, flags);//13/4/25
				return beta;
			}
			if (score > 9000)
			{
				AddHash(side, depth, score, BETA, from, to, flags);
				return score;
			}
			alpha = score;
		}
		if (score > root_score && currentmax > 6 && PlyMove[1] > 1)
		{
			return score;
		}
	}
	return alpha;
}

int CaptureSearch(int alpha, const int beta, const int depth)
{
	int score;
	int from, to, lowest;
	int val_from, val_to;
	nodes++;
	
	for (int i = first_move[ply]; i < first_move[ply + 1]; i++)
	{
		SortCaptures(i, first_move[ply + 1]);
		from = move_list[i].from;
		to = move_list[i].to;

		lowest = GetLowestAttacker(xside, move_list[i].to);
		if(lowest>-1)
		{
			val_to = p_value[b[to]];
			val_from = p_value[b[from]];

			if(val_from < val_to)
			{
				move_list[i].score = val_to - val_from;
			}
			else if(val_from > val_to + p_value[lowest] && lowest != K)
			{
				move_list[i].score = (val_to + p_value[lowest]) - val_from;
			}
			else
			{
				move_list[i].score = SEE(side,from, to);
			}
			if(move_list[i].score <= 0)
			{
				continue;
			}
		}

		if (!MakeCapture(from, to, move_list[i].flags))
			continue;

		if (ply > deep)
		{
			deep = ply;
		}

		score = -RecaptureSearch(-beta, -alpha, depth - 1, game_list[hply - 1].to);
		UnMakeCapture();

		if (score > alpha)
		{
			if (score >= beta)
			{
				return beta;
			}
			alpha = score;
		}
	}
	return alpha;
}

int RecaptureSearch(int alpha, const int beta, const int depth, const int sq)
{
	if (piece_value[game_list[hply-2].capture] + piece_value[b[sq]] < piece_value[game_list[hply-1].capture] && 
				!(game_list[hply-2].flags & INCHECK))
	{
			return alpha;
	}
	nodes++;

	int score = Eval(alpha, beta);

	if (score >= beta)
	{
		return beta;
	}
	if (score > alpha)
	{
		alpha = score;
	}
	
	int recapture_type = GenRecaptures((alpha - score) / 100, sq);

	if (recapture_type == NO_MOVES)
	{
		if (piece_mat[side] == 0 && BlockedPawns(xside) == 1)
		{
			if (SafeKingMoves(side, xside) == 0)
			{
				return STALEMATE;
			}
		}
		return alpha;
	}

	if (recapture_type == RECAPTURE_STOP)
	{
		return alpha;
	}

	if (ply > deep)
	{
		deep = ply;
	}

	MakeRecapture(move_list[first_move[ply]].from, sq);
	score = -RecaptureSearch(-beta, -alpha, depth - 1, sq);
	UnMakeRecapture();

	if (score >= beta)
	{
		return beta;
	}

	if (score > alpha)
	{
		return score;
	}
	return alpha;
}

int BlockedPawns(const int s)
{
	if (s == 0)
		if (bit_pieces[0][P] & (~(bit_units[0] | bit_units[1])) >> 8)
			return 0;
		else
			if (bit_pieces[1][P] & (~(bit_units[0] | bit_units[1])) << 8)
				return 0;
	return 1;
}

void SortCaptures(const int from, const int last)
{
	int best = from;
	for (int i = from + 1; i < last; i++)
		if (move_list[i].score > move_list[best].score)
		{
			best = i;
		}

	move_data g = move_list[from];
	move_list[from] = move_list[best];
	move_list[best] = g;
}
