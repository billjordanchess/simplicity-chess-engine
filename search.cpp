#include <stdlib.h>
#include "globals.h"

int MakeCapture(const int, const int, const int);
void UnMakeCapture();
void MakeRecapture(const int, const int);
void UnMakeRecapture();

int CaptureSearch(int alpha, int beta, int depth);
int RecaptureSearch(int alpha, int beta, int depth);

int Forced();

#define NO_PV 0
#define PV 1
#define CUT 2
#define ALL 3
#define NO_NULL 0
#define DO_NULL 1

int null_depth[48];

int epflag = 0;

int reduce[2][64][64];

int reduction[64][64][3];

int best = 0;//

int startmat[2];

const int MAX1 = MAX_PLY - 1;//
const int MAX2 = MAX_PLY - 2;//

int counter[64][64][2];

int currentdepth;

int SetHashMove();
void Sort2(const int from, const int last);

int c1 = 0, c2 = 0;

int frontier[8] = { 0,0,8,20,0,20,0 };//k as 30 june 17

int BlockedPawns3(const int s);
void SetKillers();
void ClearKillers();

move_data rootscore[100];

int Sort(const int from, const int, const int);
void checkup();

void UpdateKillers(const int i, const int from, const int to, const int x, const int depth);

int stats_depth[20];
int stats_count[100];
int stats_killers[2];
int total_depth[20];

int total_killers[2];

void z();

int Reduce(const int i);

void DisplayPV();

int killer[MAX_PLY][3];
int killer2[MAX_PLY][3];

int deep;

int c_nodes[MAX_PLY];

int DrawnMaterial();
int DrawnPawnEnding(const int s, const int xs);

int qflag;

int LoneKingLoses(const int s, const int xs);

void SetHashCapture();

#include <setjmp.h>
jmp_buf env;
bool stop_search;
int root_score;

move_data root;

void LoadBook();
int Book();

void r();

void SetNullDepth()
{
	for (int x = 0; x < 48; x++)
	{
		if(x<=6)
			null_depth[x] = 2;
		else
			null_depth[x] = 3;
	}
}

void think()
{
	int bookflag = 0;
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
	int x=0;

	stop_search = false;

	setjmp(env);
	if (stop_search) {
		hash_start = root.from;
		hash_dest = root.to;
		while (ply)
			takeback();
		return;
	}
	
	if (fixed_time == 0)
	{
		if (game_list[hply - 1].capture == 4 && b[game_list[hply - 1].to] == 4)
		{
			max_time = max_time / 2;
		}
		else if (game_list[hply - 1].capture == 3 && b[game_list[hply - 1].to] == 3 &&
			Attack(game_list[hply - 1].to, side) && Attack(game_list[hply - 1].to, xside) == 0)
		{
			max_time = max_time / 2;
		}
		else if (piece_value[game_list[hply - 1].capture] == 300 &&
			piece_value[b[game_list[hply - 1].to]] == 300 &&
			Attack(game_list[hply - 1].to, side) && Attack(game_list[hply - 1].to, xside) == 0)
		{
			max_time = max_time / 2;
		}
		else if (Attack(xside, pieces[side][5][0]))
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

	SetKillers();
	startmat[0] = piece_mat[0];
	startmat[1] = piece_mat[1];

	memset(reduction, 0, sizeof(reduction));
	memset(c_nodes, 0, sizeof(c_nodes));
	memset(PlyMove, 0, sizeof(PlyMove));
	memset(history, 0, sizeof(history));
	memset(check_history, 0, sizeof(check_history));
	memset(stats_depth, 0, sizeof(stats_depth));
	memset(stats_count, 0, sizeof(stats_count));
	memset(stats_killers, 0, sizeof(stats_killers));
	memset(counter, 0, sizeof(counter));
	ClearKillers();
	
	x = eval(-10000, 10000);
	int alpha, beta;
	int rs = side;
	int rx = xside;

	for (int i = 1; i <= max_depth; ++i)
	{
		if (fixed_depth == 0)
			if (fixed_time == 1)
			{
				if (GetTime() >= start_time + max_time)
				{
					stop_search = true;
					hash_start = root.from;
					hash_dest = root.to;
					return;
				}
			}
			else if (GetTime() >= start_time + max_time / 4)
			{
				stop_search = true;
				hash_start = root.from;
				hash_dest = root.to;
				return;
			}
		if (i > 1)
			checkup();
		currentkey = GetKey();
		currentlock = GetLock();
		currentpawnkey = GetPawnKey();
		currentpawnlock = GetPawnLock();
		memset(extend, 0, sizeof(extend));
		currentmax = i;
		deep = 0;
		
		x = search(-10000, 10000, i, PV, 0);		

		root_score = x;
		printf("%d %d %d %d",i * 100 + deep, x, (GetTime() - start_time) / 10, nodes);
		if (LookUp2(side) > -1)
		{
			DisplayPV();
			printf("\n");
			fflush(stdout);
		}
		LookUp2(side);

		if (x > 9000 || x < -9000)
		{
			break;
		}
	}
}

int search(int alpha, int beta, int depth, int pvs, int null)
{
	nodes++;
	if (nodes % 4096 == 0 && null==1)
		checkup();

	if (ply && reps2())
	{
		return 0;
	}

	int pvflag = 0;
	int lookup = LookUp(side, depth, alpha, beta);
	if(lookup > -1)
	{
		if(hash_start==0 && hash_dest==0)
			lookup = -1;
		if (b[hash_start]==0 && b[hash_dest] == EMPTY && col[hash_start] != col[hash_dest])
		{
			if(epflag == 0)
			{
				lookup = -1;
			}
		}
	}

	if (lookup == BETA)
		return beta;
	if (lookup == ALPHA)
		return alpha;
	if (lookup == EXACT)
	{
		if (hash_score >= beta)
		{
			alpha = hash_score;
		}
		if (alpha >= beta)
			return beta;
		else
		{
			pvflag = 1;//8/5/17
			PlyMove[ply] = PV;
		}
	}

	if (depth < 1)
	{
		return quiesce(alpha, beta, 0);
	}

	if (ply > MAX2)
		return eval(alpha, beta);

	int x;
	
	int check = Attack(xside, pieces[side][5][0]);
	
	drawn = 0;
	int ev1 = eval(alpha, beta);
	
	if (drawn == 1)
	{
		if (ply > 1)
			return 0;
	}
	
	currentdepth = depth;

	if ( ( depth > 2)
    &&   ( null )
    &&   (!pvs)
    &&   ( ev1 > beta )
    &&   ( piece_mat[side] > 1200 )
    &&   ( !check )  )
    {
        int oldep = epflag;
		int oldside = side;
		int oldxside = xside;
		first_move[ply + 1] = first_move[ply];
        side ^= 1;
		xside ^= 1;
		ply++; hply++;
		epflag = 0;

        x = -search( -beta, -beta+1,  depth - null_depth[depth] - 1, NO_PV, NO_NULL);
	
		side ^= 1;
		xside ^= 1;

		ply--; hply--;
		epflag = oldep;
		checkup();
        if (x >= beta) return beta;
		if (x >= 9900) return x;
    }
	
	int count = 0;
	int top = HASH_SCORE;
	int total_count = 0;
	int d;
	int initial_alpha = alpha;

	move_data bestmove;
	int bestscore = -10000;

	a_nodes += first_move[ply + 1] - first_move[ply];
	c_nodes[ply] = 0;
	int last, from, to, flags, ev;
	bool f = false;
	ev = -10000;

	int k0 = killer[ply][0];
	int k1 = killer[ply][1];
	int k20 = killer2[ply][0];
	int k21 = killer2[ply][1];
	int k22 = counter[game_list[hply - 1].from][game_list[hply - 1].to][0];
	int k23 = counter[game_list[hply - 1].from][game_list[hply - 1].to][1];

	int h2 = history[k0][k1];
	int h3 = history[k20][k21];
	int h4 = history[k22][k23];

	history[k0][k1] = KILLER_SCORE;
	history[k20][k21] = KILLER_SCORE;
	history[k22][k23] = COUNTER_SCORE;
	
	gen(check);
	int first = first_move[ply];
	bestmove = move_list[first];
	if (lookup > -1)
	{
		if(SetHashMove()==0)
		{	
			lookup = -1;
		}
	}

	history[k0][k1] = h2;
	history[k20][k21] = h3;
	history[k22][k23] = h4;

	if (ply == 0)
	{
		for (int i = 0; i < first_move[1]; i++)
		{
			for (int j = 0; j < first_move[1]; j++)
				if (move_list[i].from == rootscore[j].from && move_list[i].to == rootscore[j].to)
				{
					if (move_list[i].score < HASH_SCORE)
					{
						if (rootscore[j].score > move_list[i].score)
							move_list[i].score = rootscore[j].score;
					}
					else
						rootscore[j].score = HASH_SCORE2 + currentmax;
				}
		}
	}

	int r = 0;
	
	last = first_move[ply + 1];
	
	if (check > 0)
	{
		game_list[hply].flags |= INCHECK;
		for (int i = first; i < last; ++i)
		{
			top = Sort(i, top, last);

			if (!MakeMove(move_list[i].from, move_list[i].to, move_list[i].flags))
			{
				continue;
			}

			f = true;

			from = move_list[i].from;
			to = move_list[i].to;
			flags = move_list[i].flags;

			if (move_list[i].score < HASH_SCORE)
			{
				if (depth <= 1)
				{
					ev = ev1 + PieceScore[xside][b[to]][to] - PieceScore[xside][b[to]][from];

					if (flags & CAPTURE)
						ev += piece_value[game_list[hply - 1].capture] + PieceScore[side][game_list[hply - 1].capture][to];
					if (ev + frontier[b[to]] <= alpha)
					{
						takeback();
						continue;
					}
				}
				if (depth == 2 && ply > 1)
				{
					ev = ev1 + PieceScore[xside][b[to]][to] - PieceScore[xside][b[to]][from];
					if (flags & CAPTURE)
						ev += piece_value[game_list[hply - 1].capture] + PieceScore[side][game_list[hply - 1].capture][to] + frontier[b[to]];
					if (ev <= alpha && BestThreat(xside, side, alpha - ev) == 0)
					{
						takeback();
						continue;
					}
				}

				//blunder
				if (move_list[i].score > -50 && b[to] != 5 && game_list[hply - 1].capture == 6 && i < last - 1)
				{
					if (BestCaptureSquare(side, xside, to, b[to]) > 0)
					{
						move_list[i].score = -piece_value[b[to]];
						takeback();
						i--;
						continue;
					}
				}
			}

			count++;

			total_count++;

			b_nodes++;
			c_nodes[ply - 1]++;
			PlyMove[ply] = c_nodes[ply - 1]; 

			d = depth-r;
			extend[ply] = 1;//28/4

			if (alpha == initial_alpha)
				x = -search(-beta, -alpha, d, pvs, DO_NULL);
			else
			{
				x = -search(-alpha - 1, -alpha, d, NO_PV, DO_NULL);
				if (x > alpha)
				{
					x = -search(-beta, -alpha, d, PV, DO_NULL);
				}
			}

			takeback();

			if (x > bestscore)//24/9/13
			{
				bestscore = x;
				bestmove = move_list[i];
			}
			if (x > alpha)
			{
				if (x >= beta)
				{
#if STATS
					cut_nodes++;
					if (i - first_move[ply] == 0)
						first_nodes++;
					if (first_move[ply + 1] > first_move[ply])
						av_nodes += 100 * (i - first_move[ply]) / (first_move[ply + 1] - first_move[ply]);
#endif
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

		if (!f)
		{
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

	int threat = 0;
	int threat_start = 0;
	int threat_dest = 0;

	if (depth > 1 || pvflag)
	{
		threat = GetThreatMove(xside, side, threat_start, threat_dest, alpha);
		if (threat > 100)
		{
			MoveAttacked(threat_dest, threat_start, ply);
		}
	}
	int cc;
	for (int i = first; i < last; i++)
	{
		top = Sort(i, top, last);

		from = move_list[i].from;
		to = move_list[i].to;
		flags = move_list[i].flags;

		if (!MakeMove(from, to, flags))
		{
			continue;
		}

		if (!(flags & CHECK) && CheckAttack(xside, pieces[side][5][0]))
		{
			flags |= CHECK;
		}

		f = true;

		if (move_list[i].score == KILLER_SCORE)
			flags |= KILLER;
		if (move_list[i].score == COUNTER_SCORE)
			flags |= COUNTER;

		if (count > 0 && depth > 2)
		{
			r = Reduce(i);
			if (ply > 1 && extend[ply - 1] < 0 && r>0)
			{
				r += abs(extend[ply - 1]);
			}
			if (r == 1 && depth > 4 && currentmax / 2 < depth && count > (last - first) / 2)
			{
				r++;
			}
		}

		if (move_list[i].score<HASH_SCORE &&
			!(flags & CHECK) &&
			!(flags & PROMOTE) &&
			initial_alpha>-10000 &&
			!(piece_mat[side] == 0 && b[to] == 0 && !(mask_passed[xside][to] & bit_pieces[side][0])))
		{
			if (depth - r <= 1)
			{
				if (!(flags & CAPTURE))
				{
					if (alpha > initial_alpha)
					{
						takeback();
						continue;
					}
					if (move_list[i].score < -50 && count>0)
					{
						takeback();
						continue;
					}
					ev = ev1 + PieceScore[xside][b[to]][to] - PieceScore[xside][b[to]][from] + frontier[b[to]];

					if (ev <= alpha)
					{
						takeback();
						continue;
					}
				}
			}

			if (depth - r == 2 && !(flags & PASSED7))
			{
				ev = ev1 + PieceScore[xside][b[to]][to] - PieceScore[xside][b[to]][from] + frontier[b[to]]
					+ piece_value[game_list[hply - 1].capture] + PieceScore[side][game_list[hply - 1].capture][to] + frontier[game_list[hply - 1].capture];
				if (ev <= alpha && BestThreat(xside, side, alpha - ev) == 0)
				{
					takeback();
					continue;
				}
			}
		}
		//blunder
		if (depth - r > 1 && move_list[i].score < HASH_SCORE && !(flags & PROMOTE) &&
			!(flags & CHECK && flags & CAPTURE) && //21/2/20
			((move_list[i].score > -50 && !(flags & CAPTURE)) ||
				(move_list[i].score > KILLER_SCORE) && (flags & CAPTURE)))
		{
			if (threat > 0 && !(flags & CHECK) && from != threat_dest)
			{
				move_list[i].score = BlunderThreat(threat, threat_start, threat_dest, piece_value[game_list[hply - 1].capture], to, flags);
			}
			else
			{
				move_list[i].score = Blunder(piece_value[game_list[hply - 1].capture], to, flags);
			}
			if (move_list[i].score < -50)
			{
				i--;
				takeback();
				continue;
			}
		}
		count++;
		b_nodes++;
		c_nodes[ply - 1]++;
		PlyMove[ply] = c_nodes[ply - 1];
		extend[ply] = -r;

		if ((ply + depth > currentmax &&
			(to == game_list[hply - 2].to && game_list[hply - 2].capture < 6 && game_list[hply - 2].capture>0 &&
				!Attack(side, to))) ||
			(first_move[ply] - first_move[ply - 1] == 1))
		{
			d = depth;
			extend[ply] = 1;
		}
		else
		{
			d = depth - 1;
		}

		if (alpha == initial_alpha)
		{
			x = -search(-beta, -alpha, d - r, pvs, DO_NULL);
			if (r > 0 && x > alpha)
			{
				x = -search(-beta, -alpha, d, pvs, DO_NULL);
			}
		}
		else
		{
			x = -search(-alpha - 1, -alpha, d - r, NO_PV, DO_NULL);

			if (x > alpha)
			{
				x = -search(-beta, -alpha, d - r, PV, DO_NULL);
			}
			if (r > 0)
			{
				if (x > alpha)
				{
					extend[ply] = 0;
					x = -search(-alpha - 1, -alpha, d, NO_PV, DO_NULL);
					if (x > alpha)
						x = -search(-beta, -alpha, d, PV, DO_NULL);
				}
			}
		}
		takeback();

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

			if (x > root_score && currentmax > 6 && PlyMove[1] > 1)
			{
				break;
			}
		}
		if (move_list[i].score == KILLER_SCORE)
			stats_killers[1]++;
	}

	PlyMove[ply] = ALL;

	if (!f)
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

int quiesce(int alpha, int beta, int depth)
{
	nodes++;
	if (nodes % 4096 == 0)
		checkup();
	if (ply > MAX2)
		return eval(alpha, beta);

	int check = 0;
	int x;
	int count = 0;
	int top = HASH_SCORE;

	if (CheckAttack(xside, pieces[side][5][0]))
	{
		Evasion();

		if (first_move[ply] == first_move[ply + 1])
		{
			return ply - 10000;
		}
		if (depth<=-4)
		{
			return CaptureSearch(alpha, beta, depth);
		}
	    for (int i = first_move[ply]; i < first_move[ply + 1]; i++)
		{
			top = Sort(i, top, first_move[ply + 1]);

		if (!MakeCapture(move_list[i].from, move_list[i].to, move_list[i].flags))
		{
			continue;
		}
		count++;
		x = -quiesce(-beta, -alpha, depth-1);
		UnMakeCapture();

		if (x > alpha)
		{
			if (x >= beta)
			{
				AddHash(side, depth, x, BETA, move_list[i].from, move_list[i].to);
				return beta;
			}
			if (x > 9000)
			{
				AddHash(side, depth, x, BETA, move_list[i].from, move_list[i].to);
				return x;
			}
			alpha = x;

			AddHash(side, depth, alpha, ALPHA, move_list[i].from, move_list[i].to);
		}
		if (x > root_score && currentmax > 6 && PlyMove[1] > 1)
		{
			break;
		}
		}
		if (count == 0)
		{
			return -10000 + ply;
		}
		return alpha;
	}
	//end evasion
	if (piece_mat[side] == 0 && BlockedPawns3(side) == 1)
	{
		if (SafeKingMoves(side, xside) == 0)
			return 0;
	}
	int lookup = LookUp2(side);
	if (lookup == -1)
		x = eval(alpha, beta);
	else
		x = hash_score;

	if (x >= beta)
	{
		return beta;
	}
	if (x > alpha)
	{
		alpha = x;
	}
	if (drawn == 1)//7/4/13
	{
		drawn = 0;
		return 0;
	}

	if (depth > -4)
		gen_caps(alpha - x, depth);
	else
		gen_caps2(alpha - x);

	if (first_move[ply] == first_move[ply + 1])
	{
		return alpha;
	}
	if(depth<=-4)
	{
		return CaptureSearch(alpha, beta, depth);
	}
	count = 0;

		if (first_move[ply] + 1 == first_move[ply + 1])
		{
			if (MakeCapture(move_list[first_move[ply]].from, move_list[first_move[ply]].to, move_list[first_move[ply]].flags))
			{
				x = -quiesce(-beta, -alpha, depth);
				extend[ply] = 1;

				UnMakeCapture();
				if (x > alpha)
				{
					if (x >= beta)
					{
						AddHash(side, depth, x, BETA, move_list[first_move[ply]].from, move_list[first_move[ply]].to);//3/5/17
						return beta;
					}
					AddHash(side, depth, x, ALPHA, move_list[first_move[ply]].from, move_list[first_move[ply]].to);//6/5/17
					return x;
				}
				return alpha;//3/1/14
			}
			return alpha;//23/8/21
		}

		//more than 1 move
		if (lookup > -1)
		{
			SetHashMove();
		}
		for (int i = first_move[ply]; i < first_move[ply + 1]; i++)
		{
			top = Sort(i, top, first_move[ply + 1]);

			if (!MakeCapture(move_list[i].from, move_list[i].to, move_list[i].flags))
			{
				continue;
			}
			count++;
			if (piece_value[b[move_list[i].to]] < piece_value[game_list[hply - 1].capture] && game_list[hply - 1].capture < 6)
			{
				x = -quiesce(-beta, -alpha, depth);
			}
			else
				x = -quiesce(-beta, -alpha, depth - 1);

			UnMakeCapture();

			if (x > alpha)
			{
				if (x >= beta)
				{
					if (move_list[i].flags & CHECK)
					{
						if (check_history[b[move_list[i].from]][move_list[i].to] < HISTORY_LIMIT)
							check_history[b[move_list[i].from]][move_list[i].to] ++;
						else
							check_history[b[move_list[i].from]][move_list[i].to] /= 2;
						if (x > 9900 && check_history[b[move_list[i].from]][move_list[i].to] < CAPTURE_SCORE)
							check_history[b[move_list[i].from]][move_list[i].to] = CAPTURE_SCORE;
					}

					AddHash(side, depth, x, BETA, move_list[i].from, move_list[i].to);
					return beta;
				}
				if (x > 9000)
				{
					AddHash(side, depth, x, BETA, move_list[i].from, move_list[i].to);
					return x;
				}
				alpha = x;
			}
			if (x > root_score && currentmax > 6 && PlyMove[1] > 1)// && ply % 2 == 0)
			{
				break;
			}
		}
	return alpha;
}

int CaptureSearch(int alpha, int beta, int depth)
{
	int x;

	for (int i = first_move[ply]; i < first_move[ply + 1]; i++)
	{
		Sort2(i, first_move[ply + 1]);
		if (!MakeCapture(move_list[i].from, move_list[i].to, move_list[i].flags))
			continue;
		nodes++;

		if (ply > deep)
		{
			//z();
			deep = ply;
		}

		x = -RecaptureSearch(-beta, -alpha, depth - 1);
		UnMakeCapture();

		if (x > alpha)
		{
			if (x >= beta)
			{
				AddHash(side, depth, x, BETA, move_list[i].from, move_list[i].to);
				return beta;
			}
			alpha = x;
			if (drawn == 1)
			{
				drawn = 0;
				return 0;
			}
		}
		if (x > root_score && currentmax > 6 && PlyMove[1] > 1)
		{
			break;
		}
	}
	return alpha;
}

int RecaptureSearch(int alpha, int beta, int depth)
{
	nodes++;

	if (ply >= MAX1)
	{
		return eval(alpha, beta);
	}

	int x;
	int lookup = LookUp2(side);
	if (lookup == -1)
		x = eval(alpha, beta);
	else
		x = hash_score;

	if (drawn == 1)
	{
		drawn = 0;
		return 0;
	}
	if (x >= beta)
	{
		return beta;
	}

	if (x > alpha)
	{
		alpha = x;
	}

	int g = gen_recaptures(alpha - x, game_list[hply - 1].to);

	if (g == 0)
	{
		if (piece_mat[side] == 0 && BlockedPawns3(xside) == 1)
		{
			if (SafeKingMoves(side, xside) == 0)
			{
				return 0;
			}
		}
		return alpha;
	}

	if (g == 2)
	{
		return alpha + 1;
	}

	if (ply > deep)
	{
		//z();
		deep = ply;
	}

	MakeRecapture(move_list[first_move[ply]].from, move_list[first_move[ply]].to);

	x = -RecaptureSearch(-beta, -alpha, depth - 1);

	UnMakeRecapture();

	if (x > alpha)
	{
		if (x >= beta)
		{
			if (depth > -6)
				AddHash(side, depth, x, BETA, move_list[first_move[ply]].from, move_list[first_move[ply]].to); //4/5/17
			return beta;
		}
		alpha = x;
	}
	return alpha;
}

int reps2()
{
	for (int i = hply - 4; i >= hply - fifty; i -= 2)
	{
		if (game_list[i].hash == currentkey && game_list[i].lock == currentlock)
			return 1;
	}
	return 0;
}

int Sort(const int from, const int top, const int last)
{
	int bs = move_list[from].score;
	int bi = from;
	for (int i = from + 1; i < last; i++)
		if (move_list[i].score > bs)
		{
			bs = move_list[i].score;
			bi = i;
			if (bs >= top)
				break;
		}

	move_data g = move_list[from];
	move_list[from] = move_list[bi];
	move_list[bi] = g;

	return bs;
}

void Sort2(const int from, const int last)
{
	int bi = from;
	for (int i = from + 1; i < last; i++)
		if (move_list[i].score > move_list[bi].score)
		{
			bi = i;
		}

	move_data g = move_list[from];
	move_list[from] = move_list[bi];
	move_list[bi] = g;
}

void checkup()
{
	if (fixed_depth == 0 && ((GetTime() >= stop_time) || (max_time < 100 && ply>1)) || (max_time < 100))
	{
		stop_search = true;
		longjmp(env, 0);
	}
}

int Reduce(const int i)
{
	if (move_list[i].flags & (CAPTURE | CHECK | PROMOTE | MATETHREAT | COUNTER))// | KILLER))
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
			break;
		if (x > 1 && reps2())
			break;
		printf(" ");
		if (move.from == hash_start && move.to == hash_dest)
			break;
		move.from = hash_start;
		move.to = hash_dest;
		move.flags = 0;

		Alg(move.from, move.to);
		if (x == 0)
		{
			hash_start = root.from = move.from;
			hash_dest = root.to = move.to;
			hash_flags = root.flags = 0;
		}
		MakeMove(move.from, move.to, 0);
	}
	while (ply)
		takeback();
}

void UpdateKillers(const int i, const int from, const int to, const int x, const int depth)
{
	if (move_list[i].score == KILLER_SCORE)
		stats_killers[0]++;
	cut_nodes++;
	if (i - first_move[ply] == 0)
		first_nodes++;
	if (first_move[ply + 1] > first_move[ply])
		av_nodes += 100 * (i - first_move[ply]) / (first_move[ply + 1] - first_move[ply]);

	if (b[to] == 6 && !(move_list[i].flags & PROMOTE))
	{
		if (move_list[i].flags & CHECK)
		{
			if (check_history[b[from]][to] < HISTORY_LIMIT)
				check_history[b[from]][to] += depth;
			else
				check_history[b[from]][to] >>= 1;
			if (x > 9900 && check_history[b[from]][to] < CAPTURE_SCORE)
				check_history[b[from]][to] = CAPTURE_SCORE;
		}
		else if (history[from][to] < HISTORY_LIMIT)
		{
			history[from][to] += depth;
			counter[b[game_list[hply - 1].to]][game_list[hply - 1].to][0] = from;
			counter[b[game_list[hply - 1].to]][game_list[hply - 1].to][1] = to;
		}
		else
			history[from][to] >>= 1;
	}

	if (b[to] == 6 &&
		!(move_list[i].flags & CHECK) && !(killer[ply][0] == from && killer[ply][1] == to))
	{
		killer2[ply][0] = killer[ply][0];
		killer2[ply][1] = killer[ply][1];
		killer2[ply][2] = killer[ply][2];
		killer[ply][0] = from;
		killer[ply][1] = to;
		killer[ply][2] = b[from];
	}
}

void SetKillers()
{
	for (int x = 0; x < MAX2; x++)
	{
		killer[x][0] = killer[x + 1][0];
		killer[x][1] = killer[x + 1][1];
		killer[x][2] = killer[x + 1][2];
		killer2[x][0] = killer2[x + 1][0];
		killer2[x][1] = killer2[x + 1][1];
		killer2[x][2] = killer2[x + 1][2];
	}
}

void ClearKillers()
{
	memset(killer, 0, sizeof(killer));
	memset(killer2, 0, sizeof(killer2));
	memset(history, 0, sizeof(history));
}

int BlockedPawns3(const int s)
{
	BITBOARD b2;

	if (s == 0)
		b2 = bit_pieces[0][0] & (~(bit_units[0] | bit_units[1])) >> 8;
	else
		b2 = bit_pieces[1][0] & (~(bit_units[0] | bit_units[1])) << 8;
	if (b2 > 0)
		return 0;
	return 1;
}

int GetCurrentDepth()
{
	return currentdepth;
}

int SetHashMove()
{
	for (int i = first_move[ply]; i < first_move[ply + 1]; i++) 
	{
		if (move_list[i].from == hash_start && move_list[i].to == hash_dest)
		{
			move_list[i].score = HASH_SCORE;
			return 1;
		}
	}
	return 0;
}

