#include <stdlib.h>
#include "globals.h"

int GetThreat(int first, int target);

#define NO_PV 0
#define PV 1
#define CUT 2
#define ALL 3
#define NO_NULL 0
#define DO_NULL 1

#define RANK_7 6
#define WON_ENDGAME 9900
#define LOST_ENDGAME -9900

#define UNDEFENDED 1
#define WEAKER_ATTACK 2
#define SEE_ATTACK 3

#define INF 10000

const int INVALID = 11111;

int startmat[2];

const int MAX1 = MAX_PLY - 1;
const int MAX2 = MAX_PLY - 2;

move_data counter[64][64];

int currentdepth;

int frontier[8] = { 0,0,8,20,10,20,0 };//k as 30 june 17

move_data rootscore[100];

int stats_depth[20];
int stats_count[100];
int stats_killers[2];
int total_depth[20];

int total_killers[2];

move_data killer[MAX_PLY];
move_data killer2[MAX_PLY];

move_data root_list[100];

int deep;

int c_nodes[MAX_PLY];

int reduce[MAX_PLY];//

#include <setjmp.h>
jmp_buf env;
bool stop_search;
int root_score;

move_data root_move;

bool IsLegal(const int, const int);

void LoadBook();
int Book();

int Sort(const int from, const int, const int);
void CheckUp();

void SortCaptures(const int from, const int last);
void z();
int BlockedPawns3(const int s);
void SetKillers();
void ClearKillers();

void GenNonCaptures();
void SetRootScore();
void SetRootMoves();

void ShowMoves(int);

int Reduce(const int i);

void DisplayPV();

void UpdateKillers(const int i, const int from, const int to, const int x, const int depth);

int RemoveHashMove(const int start, const int from, const int to);
move_data GetHashMove();

int all, cut;

void z();

void GenCheck();

int InCheck[MAX_PLY];

int null_depth[64] = {
-2, -1, 0, 1, 1, 2, 2, 3,
3, 4, 4, 5, 5, 6, 6, 7,
7, 8, 8, 9, 9, 10, 10, 11,
11, 12, 12, 13, 13, 14, 14, 15,
15, 16, 16, 17, 17, 18, 18, 19,
19, 20, 20, 21, 21, 22, 22, 23,
23, 23, 23, 24, 23, 23, 23, 23,
23, 23, 23, 24, 23, 23, 23, 23
};

move_data Think(int fixed_time)
{
	int bookflag = 0;
	int alpha, beta;
//*	
	if (hply < 8)
	{
		if (hply == 0)
			LoadBook();
		bookflag = Book();
		if (bookflag > 0)
		{
			return hash_move;
		}
	}
//*/
	int score = 0;
	stop_search = false;

	setjmp(env);
	if (stop_search) {
		while (ply)
		{
			if (ply > 1 && first_move[ply - 1] == first_move[ply])
			{
				side ^= 1;
				xside ^= 1;
				--ply;
				--hply;
			}
			else
				UnMakeMove();
		}
		return root_move;
	}

	if (fixed_time == 0)
	{
		if (game_list[hply - 1].capture == QVAL &&
			b[game_list[hply - 1].to] == 4)
		{
			max_time = max_time / 2;
		}
		else if (game_list[hply - 1].capture == RVAL &&
			b[game_list[hply - 1].to] == 3 &&
			Attack(game_list[hply - 1].to, side) &&
			Attack(game_list[hply - 1].to, xside) == 0)
		{
			max_time = max_time / 2;
		}
		else if (piece_value[game_list[hply - 1].capture] == BVAL &&
			piece_value[b[game_list[hply - 1].to]] == BVAL &&
			Attack(game_list[hply - 1].to, side) &&
			Attack(game_list[hply - 1].to, xside) == 0)
		{
			max_time = max_time / 2;
		}
		else if (Attack(xside, pieces[side][K][0]))
		{
			max_time = max_time / 2;
		}
	}

	start_time = GetTime();
	stop_time = start_time + max_time;
	if (max_time < 100)
		max_depth = 1;

	ply = 0;
	nodes = 0;
	cut_nodes = 0;
	av_nodes = 0;
	first_nodes = 0;
	a_nodes = 0;
	b_nodes = 0;

	startmat[0] = piece_mat[0];
	startmat[1] = piece_mat[1];

	memset(PlyMove, 0, sizeof(PlyMove));

	memset(c_nodes, 0, sizeof(c_nodes));
	memset(stats_depth, 0, sizeof(stats_depth));
	memset(stats_count, 0, sizeof(stats_count));
	memset(stats_killers, 0, sizeof(stats_killers));

	ClearKillers();

	memset(history, 0, sizeof(history));
	memset(check_history, 0, sizeof(check_history));
	memset(counter, 0, sizeof(counter));
	memset(reduce, 0, sizeof(reduce));

	score = Eval(-10000, 10000);

	all = 0; cut = 0;

/*
	move_data best_move;
int prev_score = 0;

GenCheck();//
int from, to, flags;
int best_score;

	move_data engine_move = { 0, 0 };  // default "no move"
for (int i = 1; i <= max_depth; i++)
{
    best_score = -INF;

	currentkey = GetKey();
	currentlock = GetLock();
	currentpawnkey = GetPawnKey();
	currentpawnlock = GetPawnLock();
	memset(extend, 0, sizeof(extend));
	currentmax = i;
	deep = 0;
	prev_score = Search(-INF, INF, i, PV, 0);

 for (int x = first_move[0]; x < first_move[1]; x++)
	{
		from = move_list[x].from;
		to = move_list[x].to;
		flags = move_list[x].flags;
		if (!MakeMove(from, to, flags))
		{
			continue;
		}

        alpha = prev_score - 1;
        beta = prev_score + 1;

			while (true)
			{
				alpha = score - 1,
				beta = score + 1;
				score = -Search(-beta, -alpha, i - 1, PV, 0);
				if (score > alpha && score < beta)
					break;
				if (score <= alpha)
					alpha = score - 50;
				if (beta >= score)
					beta = score + 50;
				score = -Search(-beta, -alpha, i - 1, PV, 0);
				if (score > alpha && score < beta)
					break;
				score = -Search(-INF, INF, i - 1, PV, 0);
				break;
			}

        UnMakeMove();

        if (score > best_score) 
		{
            best_score = score;
            root_move = move_list[x];
        }
    }

    prev_score = best_score;  // Update for next depth
    root_score = best_score;

	printf("%d %d %d %d", i * 100 + deep, root_score, (GetTime() - start_time) / 10, nodes);

	if (LookUp2(side) > -1)
	{
		DisplayPV();
		printf("\n");
		fflush(stdout);
	}

	if (score > 9000 || score < -9000)
	{
		break;
	}

    if (best_score > 9000 || best_score < -9000)
        break;
}

return root_move;
}
*/
	for (int i = 1; i <= max_depth; i++)
	{
		if (i > 1)
		{
			CheckUp();
			if (fixed_depth == 0)
				if (fixed_time == 1)
				{
					if (GetTime() >= start_time + max_time)
					{
						stop_search = true;
						return root_move;
					}
				}
				else if (GetTime() >= start_time + max_time / 4)
				{
					stop_search = true;
					return root_move;
				}
		}
		//
		currentkey = GetKey();
		currentlock = GetLock();
		currentpawnkey = GetPawnKey();
		currentpawnlock = GetPawnLock();
		memset(extend, 0, sizeof(extend));
		currentmax = i;
		deep = 0;

		if (i == 1)
		{
			score = Search(-10000, 10000, i, PV, 0);
		}
		else
		{
			while (true)
			{
				alpha = score - 1,
					beta = score + 1;
				score = Search(alpha, beta, i, PV, 0);
				if (score > alpha && score < beta)
					break;
				if (score <= alpha)
					alpha = score - 50;
				if (beta >= score)
					beta = score + 50;
				score = Search(alpha, beta, i, PV, 0);
				if (score > alpha && score < beta)
					break;
				score = Search(-10000, 10000, i, PV, 0);
				break;
			}
		}

		root_score = score;

		printf("%d %d %d %d", i * 100 + deep, score, (GetTime() - start_time) / 10, nodes);

		if (LookUp2(side) > -1)
		{
			DisplayPV();
			printf("\n");
			fflush(stdout);
		}

		if (score > 9000 || score < -9000)
		{
			break;
		}
	}
	return root_move;
}

int Search(int alpha, int beta, int depth, int pvs, int null)
{
	nodes++;
	if (nodes % 4096 == 0 && null == 1)
		CheckUp();

	if (ply && Reps2())
	{
		if (alpha >= -10)
		{
			return alpha;
		}
		return -10;
	}
	if (pawn_mat[0] == 0 && pawn_mat[1] == 0 &&
		game_list[hply - 1].capture != EMPTY)
	{
		if (endmatrix[piece_mat[side]][total[side][N]][piece_mat[xside]][total[xside][N]] == DRAWN)
		{
			if (alpha >= 0)
			{
				return alpha;
			}
			return 0;
		}
	}
	if (piece_mat[xside] == 0 && (bit_pieces[side][R] | bit_pieces[side][Q]) &&
		pawn_mat[xside] == 0)
		if (piece_mat[xside] < startmat[xside] || piece_mat[side] > startmat[side])
			return WON_ENDGAME - ply;

	int pvflag = 0;

	int lookup = LookUp(side, depth, alpha, beta);
	move_data bestmove;

	if (lookup > -1)
	{
		bestmove = GetHashMove();
		if ((mask[bestmove.from] & bit_units[side]) == 0 ||
			(mask[bestmove.to] & bit_units[side]) ||
			IsLegal(bestmove.from, bestmove.to) == 0)
		{
			lookup = -1;
		}
	}
	if (lookup > -1)
	{
		if (lookup == BETA)
		{
			return beta;
		}
		if (lookup == ALPHA)
			return alpha;

		if (lookup == EXACT)
		{
			if (hash_move.score >= beta)
			{
				alpha = hash_move.score;
			}
			if (alpha >= beta)
				return beta;
			else
			{
				pvflag = 1;
				PlyMove[ply] = PV;
			}
		}
		bestmove.flags = 0;
		bestmove.score = HASH_SCORE;
	}

	if (depth < 1)
	{
		return QuietSearch(alpha, beta, 0);
	}

	if (ply > MAX2)
		return Eval(alpha, beta);

	currentdepth = depth;
	int score;
	int check = Check(xside, pieces[side][K][0]);
	int ev1 = INVALID;
	//*
	if (depth > 2
		&& null
		&& !pvs
		&& lookup == -1
		&& piece_mat[side] > RVAL
		&& check == -1
		&& ply > 1
		)
	{
		ev1 = Eval(alpha, beta);
		if (ev1 > beta)
		{
			first_move[ply + 1] = first_move[ply];
			game_list[hply].from = 0;
			game_list[hply].to = 0;
			game_list[hply].flags = 0;

			side ^= 1;
			xside ^= 1;
			ply++; hply++;
			int old_castle = castle;

			score = -Search(-beta, -beta + 1, null_depth[depth], NO_PV, NO_NULL);

			side ^= 1;
			xside ^= 1;
			castle = old_castle;

			ply--; hply--;
			CheckUp();
			if (score >= beta)
			{
				return beta;
			}
			if (score >= 9000)
			{
				return score;
			}
		}
	}
	//*/
	int count = 0;
	int initial_alpha = alpha;
	int bestscore = -20000;//
	int first = first_move[ply];
	int from, to, flags;

	first_move[ply + 1] = first_move[ply];

	move_data firstmove;

	if (lookup > -1)
	{
		bestmove = GetHashMove();
		from = bestmove.from;
		to = bestmove.to;
		flags = bestmove.flags;

		move_list[first].flags = flags;
		move_list[first].from = from;
		move_list[first].to = to;
		move_list[first].score = HASH_SCORE;
		first_move[ply + 1] = first_move[ply] + 1;

		if (MakeMove(from, to, flags))
		{
			count++;
			if (check > -1)
			{
				score = -Search(-beta, -alpha, depth, pvs, DO_NULL);
			}
			else
				score = -Search(-beta, -alpha, depth - 1, pvs, DO_NULL);

			UnMakeMove();

			if (score > alpha)
			{
				if (score >= beta)
				{
					AddHash(side, depth, score, BETA, from, to, flags);
					PlyMove[ply - 1] = CUT;
					return beta;
				}
				if (score > 9000 && ply > 0)
				{
					AddHash(side, depth, score, BETA, from, to, flags);
					PlyMove[ply - 1] = CUT;
					return score;
				}
				alpha = score;
			}
			bestscore = score;
		}
	}

	int r = 0;
	int ev = -10000;
	int top = HASH_SCORE;
	int d;
	int lowest;

	if (check > -1)
	{
		EvadeCapture(check);
		game_list[hply].flags |= INCHECK;

		if (lookup > -1)
		{
			move_list[first] = bestmove;
			RemoveHashMove(first + 1, bestmove.from, bestmove.to);
			first++;
		}

		for (int i = first; i < first_move[ply + 1]; i++)
		{
			top = Sort(i, top, first_move[ply + 1]);

			from = move_list[i].from;
			to = move_list[i].to;
			flags = move_list[i].flags;

			if (!MakeMove(from, to, flags))
			{
				continue;
			}

			count++;

			if ((ply + depth > currentmax &&
				(to == game_list[hply - 2].to &&
					game_list[hply - 2].capture != EMPTY &&
					game_list[hply - 2].capture > 0 &&
					Attack(side, to) == 0)))
			{
				d = depth;
				extend[ply] = 1;
			}
			else
			{
				d = depth - 1;
			}

			if (alpha == initial_alpha)
				score = -Search(-beta, -alpha, d, pvs, DO_NULL);
			else
			{
				score = -Search(-alpha - 1, -alpha, d, NO_PV, DO_NULL);
				if (score > alpha)
				{
					score = -Search(-beta, -alpha, d, PV, DO_NULL);
				}
			}

			UnMakeMove();

			if (score > bestscore)
			{
				bestscore = score;
				bestmove = move_list[i];
			}
			if (score > alpha)
			{
				if (score >= beta)
				{
					AddHash(side, depth, score, BETA, from, to, flags);
					PlyMove[ply - 1] = CUT;
					return beta;
				}
				alpha = score;
			}
		}
		first = first_move[ply];

		EvadeQuiet(check);

		game_list[hply].flags |= INCHECK;

		if (lookup > -1 && b[bestmove.to] == EMPTY)
		{
			move_list[first] = bestmove;
			RemoveHashMove(first + 1, bestmove.from, bestmove.to);
		}

		for (int i = first; i < first_move[ply + 1]; i++)
		{
			top = Sort(i, top, first_move[ply + 1]);

			from = move_list[i].from;
			to = move_list[i].to;
			flags = move_list[i].flags;

			if (!MakeMove(from, to, flags))
			{
				continue;
			}
			if (count > 0 && !pvs)
			{
				if (depth <= 2)
				{
					if (ev1 == INVALID)
						ev1 = Eval(alpha, beta);
				}
				if (depth <= 1)
				{
					ev = ev1 + PieceScore[side][b[from]][to] - PieceScore[side][b[from]][from];
					if (ev + frontier[b[from]] <= alpha)
					{
						UnMakeMove();
						if (alpha - ev > 100)
						{
							break;
						}
						continue;
					}
				}
			}
			if (count > 0 && depth == 2 && ply > 1 && !pvs)
			{
				ev = ev1 + PieceScore[xside][b[to]][to] - PieceScore[xside][b[to]][from];
				if (ev <= alpha && BestThreat(xside, side, alpha - ev) == 0)
				{
					UnMakeMove();
					continue;
				}
			}

			count++;

			d = depth - 1;

			if (alpha == initial_alpha)
				score = -Search(-beta, -alpha, d, pvs, DO_NULL);
			else
			{
				score = -Search(-alpha - 1, -alpha, d, NO_PV, DO_NULL);
				if (score > alpha)
				{
					score = -Search(-beta, -alpha, d, PV, DO_NULL);
				}
			}

			UnMakeMove();

			if (score > bestscore)
			{
				bestscore = score;
				bestmove = move_list[i];
			}
			if (score > alpha)
			{
				if (score >= beta)
				{
					AddHash(side, depth, score, BETA, from, to, flags);
					PlyMove[ply - 1] = CUT;
					return beta;
				}
				alpha = score;
			}
		}
		if (!count)
		{
			if (!(game_list[hply - 2].flags & INCHECK))
			{
				for (int i = first_move[ply - 2]; i < first_move[ply - 1]; i++)
					move_list[i].flags |= MATETHREAT;
			}
			PlyMove[ply - 1] = CUT;
			return -10000 + ply;
		}
		PlyMove[ply] = CUT;
		if (fifty >= 100)
			return 0;

		if (alpha > initial_alpha)
		{
			AddHash(side, depth, alpha, EXACT, bestmove.from, bestmove.to, bestmove.flags);
		}
		else
		{
			if (depth > 3)
				AddHash(side, depth, alpha, ALPHA, bestmove.from, bestmove.to, bestmove.flags);
		}
		return alpha;
	}
	//end check
	GenCaptures();

	firstmove = bestmove;

	int val_to, val_from;

	if (lookup > -1)
	{
		if (bestmove.flags & (CAPTURE | PROMOTE | EP))
			RemoveHashMove(first_move[ply] + 1, bestmove.from, bestmove.to);
		first = first_move[ply] + 1;
	}
	else
	{
		first = first_move[ply];
	}
	for (int i = first; i < first_move[ply + 1]; i++)
	{
		if (b[move_list[i].to] != EMPTY)
		{
			to = move_list[i].to;
			from = move_list[i].from;
			lowest = GetLowestAttacker(xside, to);
			val_to = p_value[b[to]];
			val_from = p_value[b[from]];
			if (lowest > -1)
			{
				if (val_from < val_to)
				{
					move_list[i].score = val_to - val_from;
				}
				else if (val_from > val_to + p_value[lowest] && lowest != K)
				{
					move_list[i].score = (val_to + p_value[lowest]) - val_from;
				}
				else
				{
					move_list[i].score = SEE(side, from, to);
				}
			}
			else
			{
				move_list[i].score = val_to;
			}
		}
	}
	for (int i = first; i < first_move[ply + 1]; i++)
	{
		SortCaptures(i, first_move[ply + 1]);

		from = move_list[i].from;
		to = move_list[i].to;
		flags = move_list[i].flags;

		if (count > 0 && !pvs)
		{
			if (depth <= 2)
			{
				if (ev1 == INVALID)
					ev1 = Eval(alpha, beta);
			}
			if (depth <= 1)
			{
				ev = ev1 + PieceScore[side][b[from]][to] - PieceScore[side][b[from]][from];
				ev += piece_value[b[to]] + PieceScore[xside][b[to]][to] + frontier[b[to]];
				if (ev + frontier[b[from]] <= alpha)
				{
					continue;
				}
			}
		}
		if (!MakeMove(from, to, flags))
		{
			continue;
		}
		if (count > 0 && depth == 2 && ply > 1)
		{
			ev = ev1 + PieceScore[xside][b[to]][to] - PieceScore[xside][b[to]][from];
			ev += piece_value[game_list[hply - 1].capture] + PieceScore[side][game_list[hply - 1].capture][to] + frontier[b[to]];
			if (ev <= alpha && BestThreat(xside, side, alpha - ev) == 0)
			{
				UnMakeMove();
				continue;
			}
		}

		count++;

		d = depth - 1;

		if (alpha == initial_alpha)
			score = -Search(-beta, -alpha, d, pvs, DO_NULL);
		else
		{
			score = -Search(-alpha - 1, -alpha, d, NO_PV, DO_NULL);
			if (score > alpha)
			{
				score = -Search(-beta, -alpha, d, PV, DO_NULL);
			}
		}

		UnMakeMove();

		if (score > bestscore)
		{
			bestscore = score;
			bestmove = move_list[i];
		}
		if (score > alpha)
		{
			if (score >= beta)
			{
				AddHash(side, depth, score, BETA, from, to, flags);
				PlyMove[ply - 1] = CUT;
				return beta;
			}
			if (score > 9000 && ply > 0)
			{
				AddHash(side, depth, score, BETA, from, to, flags);
				PlyMove[ply - 1] = CUT;
				return score;
			}
			alpha = score;
			if (score > root_score && currentmax > 6 && PlyMove[1] > 1)
			{
				break;
			}
		}
	}
	//end captures

	int killer_from = killer[ply].from;
	int killer_to = killer[ply].to;
	int killer2_from = killer2[ply].from;
	int killer2_to = killer2[ply].to;
	int counter_from = counter[game_list[hply - 1].from][game_list[hply - 1].to].from;
	int counter_to = counter[game_list[hply - 1].from][game_list[hply - 1].to].to;

	int h2 = history[killer_from][killer_to];
	int h3 = history[killer2_from][killer2_to];
	int h4 = history[counter_from][counter_to];

	history[killer_from][killer_to] = KILLER_SCORE;
	history[killer2_from][killer2_to] = KILLER_SCORE;
	if (!(game_list[hply - 1].flags & INCHECK))
		history[counter_from][counter_to] = COUNTER_SCORE;

	int startmoves = first_move[ply + 1];

	GenNonCaptures();

	history[killer_from][killer_to] = h2;
	history[killer2_from][killer2_to] = h3;
	history[counter_from][counter_to] = h4;

	if (lookup > -1)
	{
		if (b[firstmove.to] == EMPTY && !((b[firstmove.from] == P &&
			(rank[side][firstmove.from] == RANK_7 || firstmove.flags & EP))))
		{
			RemoveHashMove(startmoves, firstmove.from, firstmove.to);
		}
	}
	//
	ev1 = Eval(alpha, beta);//??
	//if(alpha > ev1 +100)
	//	z();
	//

	int diff = alpha - ev1;//??
	int attacker = -1, attacked = -1, attack_type = 0;
	/*
	if (depth > 2)
	{
		for (int piece = Q; piece > P; piece--)
			for (int i = 0; i < total[side][piece]; i++)
			{
				attacker = GetNextAttacker(xside, pieces[side][piece][i]);
				if (attacker > -1)
				{
					attacked = pieces[side][piece][i];
					if (p_value[b[attacker]] < p_value[piece])
					{
						attack_type = WEAKER_ATTACK;
						piece = 0;
						break;
					}
					if (Attack(side, attacked) == 0)
					{
						attack_type = UNDEFENDED;
						piece = 0; 
						break;
					}
					if (SEE(xside, attacker, attacked) > 0)
					{
						attack_type = SEE_ATTACK;
						piece = 0;
						break;
					}
				}
			}
	}
	//*/
	/*
	if(pvflag==1 && depth > 500)
		//printf("+");
	for (int i = startmoves; i < first_move[ply + 1]; i++)
	{
		from = move_list[i].from;
		to = move_list[i].to;
		flags = move_list[i].flags;
		if(flags & CHECK)
			continue;
		if (!MakeQuietMove(from, to, flags))
		{
			continue;
		}
		move_list[i].score = -Search(-beta, -alpha, 0, 0, 0);
		UnMakeMove();
	}
	*/
	int checkflag=0;
	int extendflag=0;

	for (int i = startmoves; i < first_move[ply + 1]; i++)
	{
		//if(checkflag==0 && diff >100)
		top = Sort(i, top, first_move[ply + 1]);

		from = move_list[i].from;
		to = move_list[i].to;
		flags = move_list[i].flags;

		if (count > 0 && ply > 0)
		{
			if (flags & CHECK &&
				move_list[i].score > BLUNDER_CHECK_SCORE)
			{
				bit_all &= not_mask[from];
				lowest = GetLowestAttacker(xside, to);
				bit_all |= mask[from];
				if (lowest > -1 && lowest < K && lowest <= b[from])
				{
					move_list[i].score = BLUNDER_CHECK_SCORE;
					i--;
					continue;
				}
			}
			if (!(flags & CHECK) &&
				move_list[i].score > -1 && depth > 2)// && diff > 0)
			{
				bit_all &= not_mask[from];
				lowest = GetLowestAttacker(xside, to);
				bit_all |= mask[from];
				if (lowest > -1)
				{
					if (Attack(side, to) == 0 || lowest < b[from])
					{
						move_list[i].score = -1;
						i--;
						continue;
					}
				}
			}
			/*/
			extendflag = 0;
			if (!(flags & CHECK))
			{
				if (depth == 1)
				{
					if(b[from]==0 && bit_pawncaptures[side][to] 
					& (bit_pieces[xside][N] | bit_pieces[xside][B] | bit_pieces[xside][R] | bit_pieces[xside][Q]))
					{
						//Alg(from,to);
						//z();
						//printf("+");
						extendflag = 1;
						depth++;
					}
					else if(b[from]==1 && bit_knightmoves[to] 
					& (bit_pieces[xside][B] | bit_pieces[xside][R] | bit_pieces[xside][Q]))
					{
						//Alg(from,to);
						//z();
						//printf("-");
						depth++;
						extendflag = 1;
					}
					else if(b[from]==2 && bit_bishopmoves[to] 
					& (bit_pieces[xside][R] | bit_pieces[xside][Q]))
					{
						if (!(bit_between[to][NextBit(bit_bishopmoves[to] & bit_pieces[xside][R] | bit_pieces[xside][Q])] & bit_all))
						{
						//Alg(from,to);printf("-");
						//Alg(to, NextBit(bit_bishopmoves[to] & bit_pieces[xside][R] | bit_pieces[xside][Q]));
						//z();
						depth++;
						extendflag = 1;
						}
					}
					else if(b[from]==3 && bit_rookmoves[to] 
					& (bit_pieces[xside][Q]))
					{
						if (!(bit_between[to][NextBit(bit_rookmoves[to] & bit_pieces[xside][Q])] & bit_all))
						{
						//Alg(from,to);
						//z();
						//printf("-");
						depth++;
						extendflag = 1;
						}
					}
				}
			}
			//*/

			if (!(flags & CHECK))
			{
				if (depth == 1)
				{
					if (ev1 == INVALID)
					{
						ev1 = Eval(alpha, beta);
					}
					ev = ev1 + PieceScore[side][b[from]][to] - PieceScore[side][b[from]][from] + frontier[b[from]];

					if (ev <= alpha)
					{
						if (alpha - ev > 100)
						{
							break;
						}
						continue;
					}
				}
			}

			if (depth > 2)
			{
				r = Reduce(i);
				if (r > 0 && depth > 2 && ply > 2 && reduce[ply - 1] == 1 && reduce[ply - 2] == 1)// && reduce[ply-3]==1)
				{
					//r++;
					//printf("+");
				}
				reduce[ply] = r;
			}
		}

		if (!MakeQuietMove(from, to, flags))
		{
			continue;
		}

		if (depth - r == 2 && !extendflag && 
			count > 0 && !(flags & CHECK))
		{
			if (ev1 == INVALID)
			{
				ev1 = -Eval(alpha, beta);
			}
			ev = ev1 + PieceScore[xside][b[to]][to] - PieceScore[xside][b[to]][from] + frontier[b[to]] +
				frontier[game_list[hply - 1].capture];
			if (ev <= alpha && BestThreat(xside, side, alpha - ev) == 0)
			{
				UnMakeMove();
				continue;
			}
		}
		/*/
		if(!(flags & CHECK) && attacker > -1 && attack_type > -1 && move_list[i].score > -1)
		{
			if(attacked == from)
			{
				//printf(" move ");
				//Alg(from,to);
				//z();
			}
			else if(attacked != from && !Attack(side, attacked))
			{
				printf(" blocked ");
				Alg(from,to);
				z();
			}
			else if(attack_type == UNDEFENDED && Attack(xside, attacked))
			{
				/
				printf(" defend ");
				Alg(attacker, attacked);
				printf(" ");
				Alg(from,to);
				z();
				/
			}
		}
		//*/

		count++;

		d = depth - 1;

		if (alpha == initial_alpha)
		{
			score = -Search(-beta, -alpha, d - r, pvs, DO_NULL);
			if (r > 0 && score > alpha)
			{
				score = -Search(-beta, -alpha, d, pvs, DO_NULL);
			}
		}
		else
		{
			score = -Search(-alpha - 1, -alpha, d - r, NO_PV, DO_NULL);

			if (score > alpha)
			{
				score = -Search(-beta, -alpha, d - r, PV, DO_NULL);
			}
			if (r > 0)
			{
				if (score > alpha)
				{
					extend[ply] = 0;
					score = -Search(-alpha - 1, -alpha, d, NO_PV, DO_NULL);
					if (score > alpha)
						score = -Search(-beta, -alpha, d, PV, DO_NULL);
				}
			}
		}

		UnMakeQuietMove();

		if (score > bestscore)
		{
			bestscore = score;
			bestmove = move_list[i];
		}
		if (score > alpha)
		{
			if (score >= beta)
			{
				if (depth < 20)
				{
					stats_depth[depth] ++;
				}
				if (count < 100)
				{
					stats_count[count] ++;
				}
				UpdateKillers(i, from, to, score, depth);
				AddHash(side, depth, score, BETA, from, to, flags);
				PlyMove[ply] = CUT;
				cut++;
				return beta;
			}
			if (score > 9000 && ply > 0)
			{
				AddHash(side, depth, score, BETA, from, to, flags);
				PlyMove[ply] = CUT;
				cut++;
				return score;
			}

			alpha = score;

			if (score > root_score && currentmax > 6 && PlyMove[1] > 1)
			{
				break;
			}
		}
	}
	all++;
	PlyMove[ply] = ALL;

	if (!count)
	{
		return 0;
	}

	if (fifty >= 100)
		return 0;

	if (alpha > initial_alpha)
	{
		AddHash(side, depth, alpha, EXACT, bestmove.from, bestmove.to, bestmove.flags);
	}
	else
	{
		if (depth > 3)
			AddHash(side, depth, alpha, ALPHA, bestmove.from, bestmove.to, bestmove.flags);
	}
	return alpha;
}

int Sort(const int from, const int top, const int last)
{
	int bestscore = move_list[from].score;
	int bi = from;
	for (int i = from + 1; i < last; i++)
		if (move_list[i].score > bestscore)
		{
			bestscore = move_list[i].score;
			bi = i;
			if (bestscore >= top)
				break;
		}

	move_data g = move_list[from];
	move_list[from] = move_list[bi];
	move_list[bi] = g;

	return bestscore;
}

void CheckUp()
{
	if (fixed_depth == 0 && ((GetTime() >= stop_time) ||
		(max_time < 100 && ply>1)) || (max_time < 100))
	{
		stop_search = true;
		longjmp(env, 0);
	}
}

int Reduce(const int i)
{
	if (move_list[i].flags & (CAPTURE | CHECK | PROMOTE | PASSED7 | MATETHREAT))
		return 0;
	if (piece_mat[xside] == 0)
		return 0;
	return 1;
}

void DisplayPV()
{
	move_data move;
	move.from = 0;
	move.to = 0;

	for (int x = 0; x < MAX_PLY; x++)
	{
		if (LookUp2(side) == -1)
		{
			break;
		}
		if (x > 1 && Reps2())
			break;
		printf(" ");
		if (move.from == hash_move.from && move.to == hash_move.to)
			break;
		move.from = hash_move.from;
		move.to = hash_move.to;
		move.flags = hash_move.flags;

		Alg(move.from, move.to);
		if (x == 0)
		{
			hash_move.from = move.from;
			hash_move.to = move.to;
			hash_move.flags = move.flags;
		}
		MakeMove(move.from, move.to, move.flags);
	}
	while (ply)
		UnMakeMove();
}

void UpdateKillers(const int i, const int from, const int to, const int x, const int depth)
{
	if (!(move_list[i].flags & PROMOTE))
	{
		if (move_list[i].flags & CHECK)
		{
			if (check_history[b[from]][to] < HISTORY_LIMIT)
				check_history[b[from]][to] += depth;
			else
				check_history[b[from]][to] >>= 1;
			if (x > WON_ENDGAME && check_history[b[from]][to] < CHECK_SCORE)
				check_history[b[from]][to] = CHECK_SCORE;
		}
		else if (history[from][to] < HISTORY_LIMIT)
		{
			history[from][to] += depth;
			if (game_list[hply - 1].capture == EMPTY)
			{
				counter[game_list[hply - 1].from][game_list[hply - 1].to].from = from;
				counter[game_list[hply - 1].from][game_list[hply - 1].to].to = to;
			}
		}
		else
			history[from][to] >>= 1;
	}

	if (!(move_list[i].flags & CHECK) &&
		!(killer[ply].from == from && killer[ply].to == to))
	{
		killer2[ply] = killer[ply];
		killer[ply].from = from;
		killer[ply].to = to;
		killer[ply].score = b[from];
	}
}

void SetKillers()
{
	for (int x = 0; x < MAX2; x++)
	{
		killer[x] = killer[x + 1];
		killer2[x] = killer2[x + 1];
	}
}

void ClearKillers()
{
	memset(killer, 0, sizeof(killer));
	memset(killer2, 0, sizeof(killer2));
	memset(history, 0, sizeof(history));
}

int GetCurrentDepth()
{
	return currentdepth;
}

int Reps2()
{
	for (int i = hply - 4; i >= hply - fifty; i -= 2)
	{
		if (game_list[i].hash == currentkey &&
			game_list[i].lock == currentlock)
		{
			return 1;
		}
	}
	return 0;
}

bool IsLegal(const int from, const int to)
{
	if (b[from] == K)
	{
		if (bit_kingmoves[from] & mask[to])
			return true;
		if (abs(from - to) == 2)
		{
			if (to == G1 && castle & 1)
				return true;
			else if (to == C1 && castle & 2)
				return true;
			else if (to == G8 && castle & 4)
				return true;
			else if (to == C8 && castle & 8)
				return true;
		}
		return false;
	}
	if (b[to] == EMPTY && col[from] != col[to] &&
		bit_pawndefends[xside][from] & mask[to])
	{
		if ((b[game_list[hply - 1].to] == P &&
			abs(game_list[hply - 1].from - game_list[hply - 1].to) == 16 &&
			col[game_list[hply - 1].to] == col[to]))
		{
			return true;
		}
		return false;
	}
	if (b[from] == R)
	{
		if (bit_rookmoves[from] & mask[to] &&
			(bit_between[from][to] & bit_all) == 0)
			return true;
		else
		{
			return false;
		}
	}
	if (b[from] == B)
	{
		if (bit_bishopmoves[from] & mask[to] &&
			(bit_between[from][to] & bit_all) == 0)
			return true;
		else
			return false;
	}
	if (b[from] == Q)
	{
		if (bit_queenmoves[from] & mask[to] &&
			(bit_between[from][to] & bit_all) == 0)
			return true;
		else
		{
			return false;
		}
	}
	if (b[from] == N)
	{
		if (bit_knightmoves[from] & mask[to])
			return true;
		else
		{
			return false;
		}
	}
	if (b[from] == P)
	{
		if (pawnplus[side][from] == to)
		{
			if (b[to] == EMPTY)
			{
				return true;
			}
			return false;
		}
		if (bit_pawndefends[xside][from] & mask[to] && b[to] != EMPTY)
		{
			return true;
		}
		if (pawndouble[side][from] == to)
		{
			if (b[pawnplus[side][from]] == EMPTY && b[to] == EMPTY)
			{
				return true;
			}
			return false;
		}
	}
	return true;
}

int GetThreat(int first, int target)
{
	int attacker = GetNextAttacker(xside, target);
	int count = 0;

	if (attacker > -1)
	{
		if (p_value[b[attacker]] < p_value[b[target]] ||
			Attack(side, target) == 0 ||
			SEE(xside, attacker, target) > 0)
		{
			return attacker;
			/*
			for (int x = first; x < first_move[ply + 1]; x++)
			{
				if (move_list[x].from == target)
				{
					if(GetNextAttacker(xside, move_list[x].to) > -1)
						break;
					count++;
					move_list[x].score += ESCAPE_SCORE + 9;
				}
				else if (count > 0)
					break;
			}
			*/
		}
		if (count > 0)
		{
			//	printf("\n threat");
			//ShowAll(ply);
		}
	}
	return -1;
}

int RemoveHashMove(const int start, const int from, const int to)
{
	for (int i = start; i < first_move[ply + 1]; i++)
	{
		if (move_list[i].from == from && move_list[i].to == to)
		{
			move_list[i] = move_list[first_move[ply + 1] - 1];
			first_move[ply + 1]--;
			return 0;
		}
	}
	return 1;
}

