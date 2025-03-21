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

const int INVALID = 11111;

int null_depth[48];

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

int deep;

int c_nodes[MAX_PLY];

#include <setjmp.h>
jmp_buf env;
bool stop_search;
int root_score;

move_data root;

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

void GenNon();
void SetRootScore();

void ShowMoves(int);

int Reduce(const int i);

void DisplayPV();

void UpdateKillers(const int i, const int from, const int to, const int x, const int depth);

int RemoveHashMove(const int start, const int from, const int to);
move_data GetHashMove();

void SetNullDepth()
{
	for (int x = 0; x < 48; x++)
	{
		if (x <= 6)
			null_depth[x] = 2;
		else
			null_depth[x] = 3;
	}
}

void Think(int fixed_time)
{
	int bookflag = 0;
	int alpha, beta;

	if (hply < 8)
	{
		if (hply == 0)
			LoadBook();
		bookflag = Book();
		if (bookflag > 0)
		{
			return;
		}
	}
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
		hash_move.from = root.from;
		hash_move.to = root.to;
		return;
	}

	if (fixed_time == 0)
	{
		if (game_list[hply - 1].capture == QVAL && b[game_list[hply - 1].to] == 4)
		{
			max_time = max_time / 2;
		}
		else if (game_list[hply - 1].capture == RVAL && b[game_list[hply - 1].to] == 3 &&
			Attack(game_list[hply - 1].to, side) && Attack(game_list[hply - 1].to, xside) == 0)
		{
			max_time = max_time / 2;
		}
		else if (piece_value[game_list[hply - 1].capture] == BVAL &&
			piece_value[b[game_list[hply - 1].to]] == BVAL &&
			Attack(game_list[hply - 1].to, side) && Attack(game_list[hply - 1].to, xside) == 0)
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

	if (hply > 4)
	{
		//SetKillers();
		//SetKillers();
	}
	memset(history, 0, sizeof(history));
	memset(check_history, 0, sizeof(check_history));
	memset(counter, 0, sizeof(counter));

	score = eval(-10000, 10000);

	int rs = side;
	int rx = xside;
	int count = 0;
	int flag = 0;

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
						hash_move.from = root.from;
						hash_move.to = root.to;
						return;
					}
				}
				else if (GetTime() >= start_time + max_time / 4)
				{
					stop_search = true;
					hash_move.from = root.from;
					hash_move.to = root.to;
					return;
				}

		}
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
			for (int i = 0; i < first_move[1]; i++)
			{
				rootscore[i].from = move_list[i].from;
				rootscore[i].to = move_list[i].to;
				rootscore[i].score = move_list[i].score;
			}
		}
		else
		{
			/*
			for(int y=0;y<1;y++)
			{
			alpha = score - 1,
			beta = score + 1;
			score = Search(alpha, beta, i, PV, 0);
			if(score>alpha && score < beta)
				break;
			if (score <= alpha)
				alpha = score - 4;
			if(beta >= score)
				beta = score + 4;		
			score = Search(alpha, beta, i, PV, 0);
			if(score>alpha && score < beta)
				break;
			if (score <= alpha)
				alpha = score - 16;
			if(beta >= score)
				beta = score + 16;		
			score = Search(alpha, beta, i, PV, 0);
			if(score>alpha && score < beta)
				break;
			if (score <= alpha)
				alpha = score - 32;
			if(beta >= score)
				beta = score + 32;		
			score = Search(alpha, beta, i, PV, 0);
			if(score>alpha && score < beta)
				break;
			score = Search(-10000, 10000, i, PV, 0);
			}
		//*/
			score = Search(-10000, 10000, i, PV, 0);
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
}

void SetRootScore()
{
	for (int i = 0; i < first_move[1]; i++)
	{
		for (int j = 0; j < first_move[1]; j++)
			if (move_list[i].from == rootscore[j].from && move_list[i].to == rootscore[j].to)
			{
				move_list[i].score = rootscore[j].score;
				break;
			}
	}
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
	if (pawn_mat[0] == 0 && pawn_mat[1] == 0 && game_list[hply - 1].capture != EMPTY)
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
	if (piece_mat[xside] == 0 && (bit_pieces[side][R] | bit_pieces[side][Q]) && pawn_mat[xside] == 0)
		if (piece_mat[xside] < startmat[xside] || piece_mat[side] > startmat[side])
			return WON_ENDGAME - ply;

	int pvflag = 0;

	int lookup = LookUp(side, depth, alpha, beta);
	move_data bestmove;

	if (lookup > -1)
	{
		bestmove = GetHashMove();
		if((mask[bestmove.from] & bit_units[side]) == 0 ||
			(mask[bestmove.to] & bit_units[side]) ||
			IsLegal(bestmove.from,bestmove.to) == 0)
		{
			lookup = -1;
		}
		if(b[bestmove.from]==6)
		{
			//printf("\nsearch ");
			//Alg(bestmove.from,bestmove.to);
			//z();
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
		return eval(alpha, beta);

	currentdepth = depth;
	int x;
	int check = Check(xside, pieces[side][K][0]);
	int ev1 = INVALID;

	if (depth > 2
		&& null
		&& !pvs
		&& piece_mat[side] > 500
		&& check < 0
		&& max_time > 100)
	{
		ev1 = eval(alpha, beta);
		if (ev1 > beta)
		{
			first_move[ply + 1] = first_move[ply];
			side ^= 1;
			xside ^= 1;
			ply++; hply++;

			x = -Search(-beta, -beta + 1, depth - null_depth[depth] - 1, NO_PV, NO_NULL);

			side ^= 1;
			xside ^= 1;

			ply--; hply--;
			CheckUp();
			if (x >= beta)
			{
				return beta;
			}
			if (x >= 9000)
			{
				return x;
			}
		}
	}

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
		flags = 0;

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
				x = -Search(-beta, -alpha, depth, pvs, DO_NULL);
			}
			else
				x = -Search(-beta, -alpha, depth - 1, pvs, DO_NULL);

			UnMakeMove();

		if (x > alpha)
		{
			if (x >= beta)
			{
				AddHash(side, depth, x, BETA, from, to);
				PlyMove[ply - 1] = CUT;
				return beta;
			}
			if (x > 9000 && ply > 0)
			{
				AddHash(side, depth, x, BETA, from, to);
				PlyMove[ply - 1] = CUT;
				return x;
			}
			alpha = x;
		}
		bestscore = x;
	}
		else
		{
			first_move[ply + 1] = first_move[ply];
			printf(" illegal ");
			Alg(from,to);
			z();
			MakeMove(from, to, flags);
		}
	}

	int r = 0;
	int ev = -10000;
	int top = HASH_SCORE;
	int d;
	int lowest;
		
	if (check > -1)
	{
		Evasion(check);
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
			if (count > 0 && !pvs)
			{
				if (depth <= 2)
				{
					if (ev1 == INVALID)
						ev1 = eval(alpha, beta);
				}
				if (depth <= 1)
				{
					ev = ev1 + PieceScore[side][b[from]][to] - PieceScore[side][b[from]][from];
					if (b[to] != EMPTY)
						ev += piece_value[b[to]] + PieceScore[xside][b[to]][to] + frontier[b[to]];
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
				if (flags & CAPTURE)
					ev += piece_value[game_list[hply - 1].capture] + PieceScore[side][game_list[hply - 1].capture][to] + frontier[b[to]];
				if (ev <= alpha && BestThreat(xside, side, alpha - ev) == 0)
				{
					UnMakeMove();
					continue;
				}
			}

			count++;

			if ((ply + depth > currentmax &&
				(to == game_list[hply - 2].to && game_list[hply - 2].capture != EMPTY && game_list[hply - 2].capture > 0 &&
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
				x = -Search(-beta, -alpha, d, pvs, DO_NULL);
			else
			{
				x = -Search(-alpha - 1, -alpha, d, NO_PV, DO_NULL);
				if (x > alpha)
				{
					x = -Search(-beta, -alpha, d, PV, DO_NULL);
				}
			}

			UnMakeMove();

			if (x > bestscore)
			{
				bestscore = x;
				bestmove = move_list[i];
			}
			if (x > alpha)
			{
				if (x >= beta)
				{
					AddHash(side, depth, x, BETA, from, to);
					PlyMove[ply - 1] = CUT;
					return beta;
				}
				if (x > 9000 && ply > 0)
				{
					AddHash(side, depth, x, BETA, from, to);
					PlyMove[ply - 1] = CUT;
					return x;
				}
				alpha = x;
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
			AddHash(side, depth, alpha, EXACT, bestmove.from, bestmove.to);
		}
		else
		{
			if (depth > 3)
				AddHash(side, depth, alpha, ALPHA, bestmove.from, bestmove.to);
		}
		return alpha;
	}
	//end check

	GenCaptures();
	
	firstmove = bestmove;
	
	if (lookup > -1)
	{
		//if(bestmove.flags & (CAPTURE | PROMOTE | EP))
		if (b[bestmove.to] != EMPTY ||
			(b[bestmove.from] == P && (rank[side][bestmove.from] == RANK_7 || bestmove.flags & EP)))
		{
			//if(!(hash_move.flags & (CAPTURE | PROMOTE | EP)))
			//	z();
			RemoveHashMove(first_move[ply] + 1, bestmove.from, bestmove.to);
		}
		first = first_move[ply] + 1;
		//	ShowAll(ply);
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
				if (lowest > -1)
				{
					if (p_value[b[from]] < p_value[b[to]])
					{
						move_list[i].score = p_value[b[to]] - p_value[b[from]];
					}
					else if (p_value[b[from]] > p_value[b[to]] + p_value[lowest] && lowest != 5)
					{
						move_list[i].score = (p_value[b[to]] + p_value[lowest]) - p_value[b[from]];
					}
					else
					{
						move_list[i].score = SEESearch(side, from, to);
					}
				}
				else
				{
					move_list[i].score = p_value[b[to]];
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
					ev1 = eval(alpha, beta);
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
			x = -Search(-beta, -alpha, d, pvs, DO_NULL);
		else
		{
			x = -Search(-alpha - 1, -alpha, d, NO_PV, DO_NULL);
			if (x > alpha)
			{
				x = -Search(-beta, -alpha, d, PV, DO_NULL);
			}
		}

		UnMakeMove();

		if (x > bestscore)
		{
			bestscore = x;
			bestmove = move_list[i];
		}
		if (x > alpha)
		{
			if (x >= beta)
			{
				AddHash(side, depth, x, BETA, from, to);
				PlyMove[ply - 1] = CUT;
				return beta;
			}
			if (x > 9000 && ply > 0)
			{
				AddHash(side, depth, x, BETA, from, to);
				PlyMove[ply - 1] = CUT;
				return x;
			}
			alpha = x;
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

	GenNon();

	history[killer_from][killer_to] = h2;
	history[killer2_from][killer2_to] = h3;
	history[counter_from][counter_to] = h4;
	
	if (lookup > -1)
	{
		if (b[firstmove.to] == EMPTY && !((b[firstmove.from] == P && (rank[side][firstmove.from] == RANK_7 || firstmove.flags & EP))))
		{	

			RemoveHashMove(startmoves, firstmove.from, firstmove.to);
		}
	}

	if (depth > 2)//??
	{
		for (int piece = Q; piece > P; piece--)
			for (int x = 0; x < total[side][piece]; x++)
			{
				if (GetThreat(startmoves, pieces[side][piece][x]) > -1)
				{
					piece = 0;
					break;
				}
			}
	}

	for (int i = startmoves; i < first_move[ply + 1]; i++)
	{
		top = Sort(i, top, first_move[ply + 1]);

		from = move_list[i].from;
		to = move_list[i].to;
		flags = move_list[i].flags;
		lowest = -1;

		if (count > 0 && flags & CHECK && move_list[i].score > BLUNDER_CHECK_SCORE)
		{
			bit_all &= not_mask[from];
			lowest = GetLowestAttacker(xside, to);
			bit_all |= mask[from];
			if (lowest > -1 && lowest < 5 && lowest <= b[from])
			{
				move_list[i].score = BLUNDER_CHECK_SCORE;
				i--;
				continue;
			}
		}
		if (count > 0 && !(flags & (CHECK | KILLER)) && move_list[i].score > -1 && depth > 2)
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

		if (count > 0 && depth > 2)
		{
			r = Reduce(i);
		}

		if (count > 0 && !(flags & CHECK))
		{
			if (depth == 1)
			{
				if (ev1 == INVALID)
				{
					ev1 = eval(alpha, beta);
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

		if (!MakeMove(from, to, flags))
		{
			continue;
		}

		if (depth - r == 2 &&
			count > 0 && !(flags & CHECK))
		{
			if (ev1 == INVALID)
			{
				ev1 = -eval(alpha, beta);
			}
			ev = ev1 + PieceScore[xside][b[to]][to] - PieceScore[xside][b[to]][from] + frontier[b[to]] +
				frontier[game_list[hply - 1].capture];
			if (ev <= alpha && BestThreat(xside, side, alpha - ev) == 0)
			{
				UnMakeMove();
				continue;
			}
		}

		count++;

		d = depth - 1;

		if (alpha == initial_alpha)
		{
			x = -Search(-beta, -alpha, d - r, pvs, DO_NULL);
			if (r > 0 && x > alpha)
			{
				x = -Search(-beta, -alpha, d, pvs, DO_NULL);
			}
		}
		else
		{
			x = -Search(-alpha - 1, -alpha, d - r, NO_PV, DO_NULL);

			if (x > alpha)
			{
				x = -Search(-beta, -alpha, d - r, PV, DO_NULL);
			}
			if (r > 0)
			{
				if (x > alpha)
				{
					extend[ply] = 0;
					x = -Search(-alpha - 1, -alpha, d, NO_PV, DO_NULL);
					if (x > alpha)
						x = -Search(-beta, -alpha, d, PV, DO_NULL);
				}
			}
		}
		UnMakeMove();

		if (x > bestscore)
		{
			bestscore = x;
			bestmove = move_list[i];
		}
		if (x > alpha)
		{
			if (x >= beta)
			{
				if (depth < 20)
				{
					stats_depth[depth] ++;
				}
				if (count < 100)
				{
					stats_count[count] ++;
				}
				UpdateKillers(i, from, to, x, depth);
				AddHash(side, depth, x, BETA, from, to);
				PlyMove[ply] = CUT;
				return beta;
			}
			if (x > 9000 && ply > 0)
			{
				AddHash(side, depth, x, BETA, from, to);
				PlyMove[ply] = CUT;
				return x;
			}

			alpha = x;

			if (ply == 0)
			{
				rootscore[i].score = x;
			}

			if (x > root_score && currentmax > 6 && PlyMove[1] > 1)
			{
				break;
			}
		}
	}

	PlyMove[ply] = ALL;

	if (!count)
	{
		return 0;
	}

	if (fifty >= 100)
		return 0;

	if (alpha > initial_alpha)
	{
		AddHash(side, depth, alpha, EXACT, bestmove.from, bestmove.to);
	}
	else
	{
		if (depth > 3)
			AddHash(side, depth, alpha, ALPHA, bestmove.from, bestmove.to);
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
	if (fixed_depth == 0 && ((GetTime() >= stop_time) || (max_time < 100 && ply>1)) || (max_time < 100))
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
		move.flags = 0;

		Alg(move.from, move.to);
		if (x == 0)
		{
			hash_move.from = root.from = move.from;
			hash_move.to = root.to = move.to;
			hash_move.flags = root.flags = 0;
		}
		MakeMove(move.from, move.to, 0);
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

	if (!(move_list[i].flags & CHECK) && !(killer[ply].from == from && killer[ply].to == to))
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
		if (game_list[i].hash == currentkey && game_list[i].lock == currentlock)
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
	if (b[to] == EMPTY && col[from] != col[to] && bit_pawndefends[xside][from] & mask[to])
	{
		if ((b[game_list[hply - 1].to] == P && 
			abs(game_list[hply - 1].from - game_list[hply - 1].to) == 16 &&
			col[game_list[hply - 1].to] == col[to]))
		{
			return true;
		}
		return false;
	}
	if(b[from]==3)
{
	if(bit_rookmoves[from] & mask[to] && 
		(bit_between[from][to] & bit_all) == 0)
	return true;
	else
		return false;
}
if(b[from]==2)
{
	if(bit_bishopmoves[from] & mask[to] && 
		(bit_between[from][to] & bit_all) == 0)
	return true;
	else
		return false;
}
if(b[from]==4)
{
	if(bit_queenmoves[from] & mask[to] && 
		(bit_between[from][to] & bit_all) == 0)
	return true;
	else
		return false;
}
if(b[from]==2)
{
	if(bit_knightmoves[from] & mask[to])
	return true;
	else
		return false;
}
if(b[from]==0)
{
	if(pawnplus[side][from] == to)
	{
		if(b[to]==6)
		{
			return true;
		}
		return false;
	}
	if (bit_pawndefends[xside][from] & mask[to] && b[to]<6)
	{
		return true;
	}
	if(pawndouble[side][from] == to)
	{
		if(b[pawnplus[side][from]]==6 && b[to]==6)
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
			SEESearch(xside, attacker, target) > 0)
		{
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
		}
		if(count > 0)
		{
		//	printf("\n threat");
		//ShowAll(ply);
		}
	}
	return attacker;
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
//printf("\nremove ");
//Alg(from,to);
//z();
return 1;
}

