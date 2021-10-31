
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <windows.h>

#include "globals.h"

void RookMoves(const int);
void BishopMoves(const int);
void QueenMoves(const int);

int total_nodes;
int current_node;
int check;

int v[100], v2[100], vflag[100];

int color[64];

int Exchange(int side, int xside, int to);
int CanBlock(int s, int xs, int n, int k);
int BlockCount(const int s, const int xs, const int sq);
int PawnRestrained(int s, int xs, int sq);
int Pinned(int x, int xs, int sq);
int CounterCheck(int side, int xside);
int Check(int from, int to);
int AdjacentKing(int s, int xs, int i);
int SafeKingEvade(const int s, const int xs);


void ShowBestLine();
int sv(int n, int start, int dest, int from, int to);
int sv(int);

void ShowTime();

char cGetFile(int sq);
char cGetRank(int sq);
char cPiece(int n);
int PrintPV();
int LowestAttacker(const int s, const int sq);
int AttackCount(const int s, const int, const int sq);
int KingCaptures(const int s, const int xs);

int IsBestMove(int start, int dest, int match);
int IsBestMove2(int start, int dest);
int alpha = 0;
BITBOARD ba;

void StartGame();
extern int root_score;//tactics

int GetStart(char* move, int dest);
int GetDest(char* move);
void SaveAsc(int);

void ConvertFen(const char* file);//
void SaveDiagram(char* file, int num);

void SetReductions();

void SetKillers();
void ClearKillers();

void PrintTable();

int fixed_time;
int fixed_depth;

void Free();

void ShowAll(int);
void SetMaterial();

int Perft(int depth);
void RunPerf();

//diagram stuff
int LoadDiagrams(const char* file);
int LoadDiagram(const char* file, int);
int GetSquare(char file, char rank);
int GetPiece(char piece);

int CountRoot();

int pos[100][3][16][3];
int startpos[100];
int diag_number;

int ponder;

FILE* diagram_file;
FILE* input_file;
FILE* output_file;
char fen_name[256];

#include <sys/timeb.h>
bool ftime_ok = false;  /* does ftime return milliseconds? */

void SetBook(int n);
int flipboard = 0;
int turn = 0;
int puzzles = 0;

int PrintBoard[64] = {
	56, 57, 58, 59, 60, 61, 62, 63,
	48, 49, 50, 51, 52, 53, 54, 55,
	40, 41, 42, 43, 44, 45, 46, 47,
	32, 33, 34, 35, 36, 37, 38, 39,
	24, 25, 26, 27, 28, 29, 30, 31,
	16, 17, 18, 19, 20, 21, 22, 23,
	8, 9, 10, 11, 12, 13, 14, 15,
	0, 1, 2, 3, 4, 5, 6, 7
};

int GetTime()
{
	struct timeb timebuffer;
	ftime(&timebuffer);
	if (timebuffer.millitm != 0)
		ftime_ok = true;
	return (timebuffer.time * 1000) + timebuffer.millitm;
}

int main()
{
	HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	int computer_side;
	char s[256];
	char sFen[256];//
	char sText[256];//
	int m;
	float turns = 0;
	int editmode = 0;
	int analyze = 0;
	int np = 0;
	int edit_color = 0;
	int sq = 0;
	double nps;
	fixed_time = 0;
	int t;
	printf("\n");
	printf("Simplicity\n");
	printf("version 1.0 ,6/9/21\n");
	printf("Copyright 2018 Bill Jordan \n");
	printf("\n");
	printf("\"help\" displays a list of commands.\n");
	printf("\n");
	RandomizeHash();
	FreeAllHash();
	init_board();
	ClearKillers();
	//SetBook(1);

	if (first_move[0] != 0)
		first_move[0] = 0;
	SetReductions();

	gen(0);
	computer_side = EMPTY;
	max_time = 1 << 25;
	max_depth = 4;

	for (;;)
	{
		diag_number = 0;
		if (side == computer_side)
		{  
		//if(first_move[1] == 1)
			if (CountRoot() == 1)
			{
				hash_start = move_list[0].from;
				hash_dest = move_list[0].to;
				hash_flags = move_list[0].flags;
			}
			else
			{
				for (int x = 0; x < 2; x++)
				{
					start_pawn_mat[x] = pawn_mat[x];
					start_piece_mat[x] = piece_mat[x];
				}
				
				think();
				printf(" think\n");
				turns++;
				//
				currentkey = GetKey();
				currentlock = GetLock();
				currentpawnkey = GetPawnKey();//
				currentpawnlock = GetPawnLock();//
				int a = 0;

				int lookup = LookUp2(side);
				printf("\n lookup %d ", lookup);
				Alg(hash_start, hash_dest);
			
				/*
				printf("\n hash %d ", hashpositions[0]);
				printf(" hash %d ", hashpositions[1]);
				printf(" collisions %d ", collisions);
				printf(" pawn collisions %d ", pcollisions);
				*/
				printf("\n cut nodes %d ", cut_nodes);
				printf(" first nodes %d ", first_nodes);
				if (cut_nodes > 0)
					printf(" percent %d %", (first_nodes * 100 / cut_nodes));
				//if(cut_nodes>0)
				//printf(" av cut %d %",av_nodes/cut_nodes);
				if (a_nodes > 0)
					printf(" av sort %d %", (100 - b_nodes * 100 / a_nodes));
				//collisions = 0;
				printf("\n");
				currentmax = 0;//
			}

			if (hash_start == 0 && hash_dest == 0) {
				printf("(no legal moves)\n");
				computer_side = EMPTY;
				continue;
			}
			printf("Computer's move: %s\n", move_str(hash_start, hash_dest, 0, 0));
			//
			t = GetTime() - start_time;
			printf("\nTime: %d ms\n", t);
			if (t > 0)
				nps = (double)nodes / (double)t;
			else
				nps = 0;
			nps *= 1000.0;

			printf("\nNodes per second: %d (Score: %.3f)\n", (int)nps, (float)nps / 243169.0);
			//
			if (analyze == 0)
				MakeMove(hash_start, hash_dest, hash_flags);
			ply = 0;
			if (first_move[0] != 0)
				first_move[0] = 0;
			first_move[0] = 0;//11/12/12
			check = Attack(xside, pieces[side][5][0]);
			gen(check);
			print_result();
			printf(" turn "); printf("%d", turn++);
			print_board();
			if (analyze == 1)
				goto out;
			continue;
		}
	out:
		printf("Cheese> ");
		if (scanf("%s", s) == EOF)
			return 0;
		if (!strcmp(s, "a") || !strcmp(s, "analyze"))
		{
			analyze = 1;
			printf("starting analysis mode\n");
			continue;
		}
		if (!strcmp(s, "pt"))
		{
			printf("Print Table\n");
			PrintTable();
			continue;
		}
		if (!strcmp(s, "exit"))
		{
			analyze = 0;
			printf("ending analysis mode\n");
			continue;
		}
		if (!strcmp(s, "perf"))
		{
			RunPerf();
			continue;
		}
		if (!strcmp(s, "fen"))//21/4/18
		{
			ConvertFen("c:\\diagrams\\s6.js");
			continue;
		}
		if (!strcmp(s, "v"))
		{
			//TestEval();
			continue;
		}
		if (!strcmp(s, "edit"))
		{
			editmode = 1;
			printf("edit");
			continue;
		}
		if (!strcmp(s, "#"))
		{
			editmode = 1;
			for (int x = 0; x < 64; x++)
			{
				b[x] = EMPTY;
			}
			//SetBook(0);
			castle = 0;
			printf("#\n");
			continue;
		}
		//if(editmode==1)
		if (!strcmp(s, "Nc5"))
		{
			switch (s[0])
			{
			case 'P': np = 0; break;
			case 'N': np = 1; break;
			case 'B': np = 2; break;
			case 'R': np = 3; break;
			case 'Q': np = 4; break;
			case 'K': np = 5; break;
			case 'c':edit_color = 1; break;
			default:break;
			}
			sq = s[1] - 96 - 1 + (s[2] - 48 - 1) * 8;
			if (sq > -1 && sq<64 && np>-1 && np < 6)
			{
				if (np == 5) pieces[edit_color][5][0] = sq;
				b[sq] = np;
			}
			continue;
		}
		if (!strcmp(s, "."))
		{
			editmode = 0;
			continue;
		}
		/*
		  if (!strcmp(s, "#"))
		  {

		int edit_color=0;
		int sq=0;
		int np=0;
		  while(strcmp(s, "."))
		  {
		  scanf("%s", s);
		  printf(" %s ",s);
		  switch(s[0])
		  {
		  case 'P': np=0;break;
		  case 'N': np=1;break;
		  case 'B': np=2;break;
		  case 'R': np=3;break;
		  case 'Q': np=4;break;
		  case 'K': np=5;break;
		  case 'c':edit_color=1;break;
		  default:break;
		  }
		  sq= s[1]-96-1 + (s[2]-48-1)*8;
		if(sq>-1 && sq<64 && np>-1 && np<6)
		{
		  if(np==5) kingloc[edit_color]=sq;
		  b[sq]=np;
		}
		  }
		  continue;
		}
		*/
		if (!strcmp(s, "sb")) {
			//SetBook(0);
			sFen[0] = 0;
			strcat_s(sFen, "c:\\users\\bill\\desktop\\bscp\\");
			scanf("%s", sText);
			strcat_s(sFen, sText);//
			strcat_s(sFen, ".fen");
			LoadDiagram(sFen, 1);
			continue;
		}
		if (!strcmp(s, "np")) {
			scanf("%d", &diag_number);
			LoadDiagram(fen_name, diag_number);
			computer_side = EMPTY;
			continue;
		}
		if (!strcmp(s, "nd")) {
			LoadDiagram(fen_name, diag_number);
			computer_side = EMPTY;
			continue;
		}
		if (!strcmp(s, "on") || !strcmp(s, "p")) {
			computer_side = side;
			continue;
		}
		if (!strcmp(s, "off")) {
			computer_side = EMPTY;
			continue;
		}
		if (!strcmp(s, "easy")) {
			ponder = 0;
			continue;
		}
		if (!strcmp(s, "hard")) {
			ponder = 1;
			continue;
		}
		if (!strcmp(s, "random")) {
			continue;
		}
		if (!strcmp(s, "st")) {
			scanf("%d", &max_time);
			max_time *= 1000;
			max_depth = MAX_PLY;
			fixed_time = 1;
			continue;
		}
		if (!strcmp(s, "sd")) {
			scanf("%d", &max_depth);
			max_time = 1 << 25;
			fixed_depth = 1;//
			continue;
		}
		if (!strcmp(s, "undo")) {
			if (!hply)
				continue;
			computer_side = EMPTY;
			takeback();
			ply = 0;
			if (first_move[0] != 0)
				first_move[0] = 0;
			check = Attack(xside, pieces[side][5][0]);
			gen(check);
			continue;
		}
		if (!strcmp(s, "new")) {
			computer_side = EMPTY;
			init_board();
			if (first_move[0] != 0)
				first_move[0] = 0;
			check = Attack(xside, pieces[side][5][0]);
			gen(check);
			continue;
		}
		if (!strcmp(s, "d")) {
			print_board();
			printf("\n key %d ", currentkey);
			printf("\n lock %d ", currentlock);
			continue;
		}
		if (!strcmp(s, "bench") || !strcmp(s, "b")) {
			computer_side = EMPTY;
			bench();
			continue;
		}
		if (!strcmp(s, "f")) {
			Flip = 1 - Flip;
			print_board();
			continue;
		}
		if (!strcmp(s, "book") || !strcmp(s, "bk")) {
			//SetBook(0);
			continue;
		}
		if (!strcmp(s, "nb")) {
			//SetBook(0);
			continue;
		}
		if (!strcmp(s, "sw")) {
			side = 1 - side;
			xside = 1 - xside;
			continue;
		}
		if (!strcmp(s, "moves")) {     //wj
			printf("Moves ??\n");
			for (int i = 0; i < first_move[1]; ++i)
			{
				printf("%s", move_str(move_list[i].from, move_list[i].to, 0, 0));
				printf("\n");
			}
			continue;
		}
		if (!strcmp(s, "bye")) {
			printf("Share and enjoy!\n");
			break;
		}
		if (!strcmp(s, "xboard")) {
			xboard();
			break;
		}
		if (!strcmp(s, "help")) {
			printf("what - add random choices\n");
			printf("con - convert diagrams\n");
			printf("a - toggle analysis mode\n");
			printf("on - computer plays for the side to move\n");
			printf("off - computer stops playing\n");
			printf("st n - search for n seconds per move\n");
			printf("sd n - search n ply per move\n");
			printf("undo - takes back a move\n");
			printf("new - starts a new game\n");
			printf("d - display the board\n");
			printf("f - flip board\n");
			printf("bench - run the built-in benchmark\n");
			printf("book - turns on openong book\n");
			printf("nb - turns off opening book\n");
			printf("nd - next diagram\n");
			printf("np - load diagram\n eg. np 101 (For fen files which contain multiple diagrams)");
			printf("nperf - run perf test\n");
			printf("pt - print table\n");
			printf("v - test eval\n");
			printf("sb - set board fen diagram\n eg. sb lasker (for lasker.fen)");
			printf("sw - switch sides\n");
			printf("tac - tactics\n");
			printf("alf - alpha\n");
			printf("alp - alpha play\n");
			printf("bye - exit the program\n");
			printf("Enter moves in coordinate notation, e.g., e2e4, e7e8Q\n");
			continue;
		}

		/* maybe the user entered a move? */
		ply = 0;
		if (first_move[0] != 0)
			first_move[0] = 0;
		currentmax = 0;//
		check = Attack(xside, pieces[side][5][0]);
		gen(check);
		m = parse_move(s);
		if (m == -1 || !MakeMove(move_list[m].from, move_list[m].to, move_list[m].flags))
		{
			printf("Illegal move.\n");
		}
		else
			SetKillers();//
	}
	Free();
	return 0;
}

int parse_move(char* s)
{
	int from, to, i;

	if (s[0] < 'a' || s[0] > 'h' ||
		s[1] < '0' || s[1] > '9' ||
		s[2] < 'a' || s[2] > 'h' ||
		s[3] < '0' || s[3] > '9')
		return -1;

	from = s[0] - 'a';
	from += ((s[1] - '0') - 1) * 8;
	to = s[2] - 'a';
	to += ((s[3] - '0') - 1) * 8;

	for (i = 0; i < first_move[1]; ++i)
		if (move_list[i].from == from && move_list[i].to == to)
		{
			if (b[from] == 0 && rank[side][to] == 7)
			{
				/*	if(s[4]=='n' || s[4]=='N')if(move_list[i].promote==1) return i; else continue;
		if(s[4]=='b' || s[4]=='B')if(move_list[i].promote==2) return i; else continue;
		if(s[4]=='r' || s[4]=='r')if(move_list[i].promote==3) return i; else continue;
		 move_list[i].promote = 4;*/
			}
			return i;
		}
	return -1;
}

char* move_str(int from, int to, int flags, int promote)
{
	static char str[6];

	char c;

	if (flags & PROMOTE) {
		switch (promote) {
		case KNIGHT:
			c = 'n';
			break;
		case BISHOP:
			c = 'b';
			break;
		case ROOK:
			c = 'r';
			break;
		default:
			c = 'q';
			break;
		}
		sprintf(str, "%c%d%c%d%c",
			col[from] + 'a',
			row[from] + 1,
			col[to] + 'a',
			row[to] + 1,
			c);
	}
	else
		sprintf_s(str, "%c%d%c%d",
			col[from] + 'a',
			row[from] + 1,
			col[to] + 'a',
			row[to] + 1);
	return str;
}

void print_board()
{
	HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	int Flip = 0;
	int i, x = 0;
	int text = 15;
	int c;

	printf("\n8 ");
	if (Flip == 0)
	{
		for (int j = 0; j < 64; ++j)
		{
			i = PrintBoard[j];
				c = 6;
				if (bit_units[0] & mask[i]) c = 0;
				if (bit_units[1] & mask[i]) c = 1;
				switch (c)
				{
				case EMPTY:
					if (colors[i] == 0)
						text = 127;
					else
						text = 34;
					SetConsoleTextAttribute(hConsole, text);
					printf(" .");
					SetConsoleTextAttribute(hConsole, 15);
					break;
				case 0:
					if (colors[i] == 0)
						text = 126;
					else
						text = 46;
					SetConsoleTextAttribute(hConsole, text);
					printf(" %c", piece_char[b[i]]);
					SetConsoleTextAttribute(hConsole, 15);
					break;
				case 1:
					if (colors[i] == 0)
						text = 112;
					else
						text = 32;
					SetConsoleTextAttribute(hConsole, text);
					printf(" %c", piece_char[b[i]] + ('a' - 'A'));
					SetConsoleTextAttribute(hConsole, 15);
					break;
				default:
					printf(" %d.", c);
					break;
				}
			if ((c == 0 || c == 1) && b[i] == 6)
				printf(" %d", c);
			if (b[i] < 0 || b[i]>6)
				printf(" %d.", b[i]);
			if ((j + 1) % 8 == 0 && j != 63)
			printf("\n%d ", row[i]);
		}
		printf("\n\n   a b c d e f g h\n\n");
	}

	if (Flip == 3)
	{
		for (int j = 0; j < 64; ++j) {
			i = PrintBoard[j];
			c = 6;
			if (bit_units[0] & mask[i]) c = 0;
			if (bit_units[1] & mask[i]) c = 1;
			switch (c) {
			case EMPTY:
				printf(" .");
				break;
			case LIGHT:
				printf(" %c", piece_char[b[i]]);
				break;
			case DARK:
				printf(" %c", piece_char[b[i]] + ('a' - 'A'));
				break;
			}
			if ((j + 1) % 8 == 0 && j != 63)
				printf("\n%d ", row[i]);
		}
		printf("\n\n   a b c d e f g h\n\n");
	}

	if (Flip == 4)
	{
		for (int j = 0; j < 64; ++j) {
			i = PrintBoard[j];
			//??	    if(Attack(0,i))
						//printf(" %s","X");
					//else
			{
				//??     if(Attack(1,i))
						 //printf(" %s","x");
					 //else
				printf(" .");
			}
			if ((j + 1) % 8 == 0 && j != 63)
				printf("\n%d ", row[i]);
		}
		printf("\n\n   a b c d e f g h\n\n");
	}

	if (Flip == 5)
	{
		for (int x = 0; x < 64; x++)
		{
			for (int y = 0; y < 64; y++)
			{
				if (bit_between[x][y])
				{
					printf("\n%d ", x);
					printf("%d ", y);
					printf("\n");
					printf("\n8 ");
					for (int j = 0; j < 64; ++j)
					{
						i = PrintBoard[j];
						printf(" .");
						if ((j + 1) % 8 == 0 && j != 63)
							printf("\n%d ", row[i]);
					}
					printf("\n\n   a b c d e f g h\n\n");
					// getch();
				}
			}
		}
		//getch();
	}

	if (Flip == 6)
	{
		for (int j = 0; j < 64; ++j) {
			i = PrintBoard[j];
			printf(" .");
			if ((j + 1) % 8 == 0 && j != 63)
				printf("\n%d ", row[i]);
		}
		printf("\n\n   a b c d e f g h\n\n");
	}

	if (Flip == 7)
	{
		for (int j = 0; j < 64; ++j) {
			i = PrintBoard[j];
			printf(" .");
			if ((j + 1) % 8 == 0 && j != 63)
				printf("\n%d ", row[i]);
		}
		printf("\n\n   a b c d e f g h\n\n");
	}

	if (Flip == 1)
	{
		for (int j = 0; j < 64; ++j) {
			i = 63 - PrintBoard[j];
			c = 6;
			if (bit_units[0] & mask[i]) c = 0;
			if (bit_units[1] & mask[i]) c = 1;
			switch (c) {
			case EMPTY:
				printf(" .");
				break;
			case LIGHT:
				printf(" %c", piece_char[b[i]]);
				break;
			case DARK:
				printf(" %c", piece_char[b[i]] + ('a' - 'A'));
				break;
			}
			if ((j + 1) % 8 == 0 && row[i] != 7)
				printf("\n%d ", row[j] + 2);//7-
		}
		printf("\n\n   h g f e d c b a\n\n");
	}
}

void xboard()
{
	int computer_side;
	char line[256], command[256];
	int m;
	int post = 0;
	int analyze = 0;

	signal(SIGINT, SIG_IGN);
	printf("\n");
	init_board();
	ClearKillers();
	gen(0);
	fixed_time = 0;
	computer_side = EMPTY;

	for (;;) {
		fflush(stdout);
		if (side == computer_side) {
			for (int x = 0; x < 2; x++)
			{
				start_pawn_mat[x] = pawn_mat[x];
				start_piece_mat[x] = piece_mat[x];
			}
			think();
			//-mSearch();
			currentmax = 0;//
			if (hash_start == 0 && hash_dest == 0) {
				computer_side = EMPTY;
				continue;
			}
			printf("move %s\n", move_str(hash_start, hash_dest, 0, 0));//pv[0][0].flags
			if (analyze == 0)//
				MakeMove(hash_start, hash_dest, hash_flags);
			else goto out;

			ply = 0;
			check = Attack(xside, pieces[side][5][0]);
			gen(check);
			print_result();
			continue;
		}
	out:
		if (!fgets(line, 256, stdin))
			return;
		if (line[0] == '\n')
			continue;
		sscanf(line, "%s", command);
		if (!strcmp(command, "xboard"))
			continue;
		if (!strcmp(command, "new")) {
			init_board();
			gen(0);
			ClearKillers();
			computer_side = DARK;
			continue;
		}
		if (!strcmp(command, "quit"))
			return;
		if (!strcmp(command, "force")) {
			computer_side = EMPTY;
			continue;
		}
		//
		if (!strcmp(command, "analyze"))
		{
			analyze = 1;
			printf("starting analysis mode\n");
			continue;
		}
		if (!strcmp(command, "exit"))
		{
			analyze = 0;
			printf("ending analysis mode\n");
			continue;
		}
		//
		if (!strcmp(command, "white")) {
			side = LIGHT;
			xside = DARK;
			gen(0);
			computer_side = DARK;
			continue;
		}
		if (!strcmp(command, "black")) {
			side = DARK;
			xside = LIGHT;
			gen(0);
			computer_side = LIGHT;
			continue;
		}
		if (!strcmp(command, "st")) {
			sscanf_s(line, "st %d", &max_time);
			max_time *= 1000;
			max_depth = MAX_PLY;
			fixed_time = 1;
			continue;
		}
		if (!strcmp(command, "sd")) {
			sscanf_s(line, "sd %d", &max_depth);
			max_time = 1 << 25;
			continue;
		}
		if (!strcmp(command, "time")) {
			sscanf_s(line, "time %d", &max_time);
			max_time *= 10;
			max_time /= 20;//30 28/4/13
			max_depth = MAX_PLY;
			//printf(" max time %d \n",max_time);//
			continue;
		}
		if (!strcmp(command, "otim")) {
			continue;
		}
		if (!strcmp(command, "go")) {
			computer_side = side;
			continue;
		}
		if (!strcmp(command, "hint")) {
			think();
			currentmax = 0;//
			if (hash_start == 0 && hash_dest == 0)
				continue;
			printf("Hint: %s\n", move_str(hash_start, hash_dest, 0, 0));
			continue;
		}
		if (!strcmp(command, "undo")) {
			if (!hply)
				continue;
			takeback();
			ply = 0;
			gen(0);
			continue;
		}
		if (!strcmp(command, "remove")) {
			if (hply < 2)
				continue;
			takeback();
			takeback();
			ply = 0;
			gen(0);
			continue;
		}
		if (!strcmp(command, "post")) {
			post = 2;
			continue;
		}
		if (!strcmp(command, "nopost")) {
			post = 0;
			continue;
		}
		//
		if (!strcmp(command, "setboard")) {
			//SetBook(0);
			LoadDiagram("smothered.fen", 1);
			continue;
		}
		//
		m = parse_move(line);
		if (m == -1 || !MakeMove(move_list[m].from, move_list[m].to, move_list[m].flags))
			printf("Error (unknown command): %s\n", command);
		else {
			ply = 0;
			SetKillers();
			gen(0);
			print_result();
		}
	}
}

void print_result()
{
	int i;
	int flag = 0;//
	/* is there a legal move? */
	for (i = 0; i < first_move[1]; ++i)
		if (MakeMove(move_list[i].from, move_list[i].to, move_list[i].flags))
		{
			takeback();
			flag = 1;
			break;
		}
	if (pawn_mat[0] == 0 && pawn_mat[1] == 0 && piece_mat[0] <= 300 && piece_mat[1] <= 300)
	{
		printf("1/2-1/2 {Stalemate}\n");
		return;
	}
	if (i == first_move[1] && flag == 0)
	{
		//
		gen(0);
		printf(" end of game ");
		//
		if (Attack(xside, pieces[side][5][0]))
		{
			if (side == LIGHT)
				printf("0-1 {Black mates}\n");
			else
				printf("1-0 {White mates}\n");
		}
		else
			printf("1/2-1/2 {Stalemate}\n");
	}
	else if (reps() >= 3)
		printf("1/2-1/2 {Draw by repetition}\n");
	else if (fifty >= 100)
		printf("1/2-1/2 {Draw by fifty move rule}\n");
}

void bench()
{
	int i;
	int t[3];
	double nps;

	LoadDiagram("bench.fen", 1);//
	pieces[0][5][0] = G1;
	pieces[1][5][0] = G8;
	side = LIGHT;
	xside = DARK;
	castle = 0;
	fifty = 0;
	ply = 0;
	hply = 0;
	print_board();
	max_time = 1 << 25;
	max_depth = 10;//?5;

//*
	int z = 0;
	nodes = 100000;
	int tim;
	start_time = GetTime();

	for (int x = 0; x < 1000000; x++)
	{
		QueenMoves(E4);
	}
	tim = GetTime() - start_time;
	printf("Time: %d ms\n", tim);
	start_time = GetTime();

	for (int x = 0; x < 1000000; x++)
	{
		//RookMoves2(E4);
	}
	tim = GetTime() - start_time;
	printf("Time: %d ms\n", tim);
	return;
	//*/
	for (i = 0; i < 3; ++i)
	{
		FreeAllHash();
		think();
		currentmax = 0;//
		t[i] = GetTime() - start_time;
		printf("Time: %d ms\n", t[i]);
	}

	if (t[1] < t[0])
		t[0] = t[1];
	if (t[2] < t[0])
		t[0] = t[2];
	printf("\n");
	printf("Nodes: %d\n", nodes);

	printf("Best time: %d ms\n", t[0]);

	if (!ftime_ok) {
		printf("\n");
		printf("Your compiler's ftime() function is apparently only accurate\n");
		printf("to the second. Please change the GetTime() function in main.c\n");
		printf("to make it more accurate.\n");
		printf("\n");
		return;
	}
	if (t[0] == 0) {
		printf("(invalid)\n");
		return;
	}
	nps = (double)nodes / (double)t[0];
	nps *= 1000.0;

	/* Score: 1.000 = my Athlon XP 2000+ */
	printf("Nodes per second: %d (Score: %.3f)\n", (int)nps, (float)nps / 243169.0);

	//NewPosition();
	//SetBook(1);//
	gen(0);
}

void CloseDiagram()
{
	if (diagram_file)
		fclose(diagram_file);
	diagram_file = NULL;
}

int LoadDiagrams(const char* file)
{
	memset(pos, 0, sizeof(pos));
	diagram_file = fopen(file, "r");
	if (!diagram_file)
	{
		printf("Diagram missing.\n");
		return -1;
	}
	return 0;
}

int LoadDiagram(const char* file, int num)
{
	int x;
	static int count = 1;
	char ts[256];

	diagram_file = fopen(file, "r");
	if (!diagram_file)
	{
		printf("Diagram missing.\n");
		return -1;
	}

	strcpy_s(fen_name, file);

	for (x = 0; x < num; x++)
	{
		fgets(ts, 256, diagram_file);
		if (!ts) break;
	}
	for (x = 0; x < 64; x++)
	{
		b[x] = EMPTY;
	}
	int c = 0, i = 0, j;
	while (ts)
	{
		if (ts[c] >= '0' && ts[c] <= '8')
			i += ts[c] - 48;
		if (ts[c] == '\\')
			continue;
		j = Flip[i];
		switch (ts[c])
		{
		case 'K': b[j] = KING; color[j] = 0; i++;
			pieces[0][5][0] = j; break;
		case 'Q': b[j] = QUEEN; color[j] = 0; i++; break;
		case 'R': b[j] = ROOK; color[j] = 0; i++; break;
		case 'B': b[j] = BISHOP; color[j] = 0; i++; break;
		case 'N': b[j] = KNIGHT; color[j] = 0; i++; break;
		case 'P': b[j] = PAWN; color[j] = 0; i++; break;
		case 'k': b[j] = KING; color[j] = 1; i++;
			pieces[1][5][0] = j; break;
		case 'q': b[j] = QUEEN; color[j] = 1; i++; break;
		case 'r': b[j] = ROOK; color[j] = 1; i++; break;
		case 'b': b[j] = BISHOP; color[j] = 1; i++; break;
		case 'n': b[j] = KNIGHT; color[j] = 1; i++; break;
		case 'p': b[j] = PAWN; color[j] = 1; i++; break;
		}
		c++;
		if (ts[c] == ' ')
			break;
		if (i > 63)
			break;
	}
	if (ts[c] == ' ' && ts[c + 2] == ' ')
	{
		if (ts[c + 1] == 'w')
		{
			side = 0; xside = 1;
		}
		if (ts[c + 1] == 'b')
		{
			side = 1; xside = 0;
		}
	}
	castle = 15;
	while (ts[c])
	{
		switch (ts[c])
		{
		case '-': break;
		case 'K':if (b[E1] == 5 && mask[E1] & bit_units[0])castle += 1; break;
		case 'Q':if (b[E1] == 5 && mask[E1] & bit_units[0])castle += 2; break;
		case 'k':if (b[E8] == 5 && mask[E1] & bit_units[1])castle += 4; break;
		case 'q':if (b[E8] == 5 && mask[E1] & bit_units[1])castle += 8; break;
		default:break;
		}
		c++;
	}
	castle = 0;
	CloseDiagram();
	NewPosition();
	gen(0);

	printf(" diagram # %d \n", num);
	count++;
	if (side == 0)
		printf("White to move\n");
	else
		printf("Black to move\n");
	printf(" %s \n", ts);
	hply = 10;//
	return 0;
}

void SetMaterial()
{
	int x;

	for (x = 0; x < 2; x++)
	{
		start_pawn_mat[x] = pawn_mat[x];
		start_piece_mat[x] = piece_mat[x];
	}
	for (x = 0; x < MAX_PLY; x++)
	{
		captures[x] = 6;
	}
}

void PrintTable()
{
	for (int x = 0; x < 64; x++)
	{
		if (knight_score[x] != 0)
		{
			printf("\n"); Alg1(x); printf(" %d ", knight_score[x]);
		}
	}
	_getch();
}

void RunPerf()
{
	castle = 0;
	NewPosition();
	first_move[0] = 0;
	int n = Perft(1);
	printf("\n perf %d ", n);
	z();
}

int Perft(int depth)
{
	int n_moves, i;
	int nodes = 0;

	if (depth == 0) return 1;

	gen(0);
	n_moves = first_move[ply + 1] - first_move[ply];
	for (i = 0; i < n_moves; i++)
	{
		if (!MakeMove(move_list[i].from, move_list[i].to, move_list[i].flags))
			continue;
		z();
		nodes += Perft(depth - 1);
		takeback();
	}
	printf(" %d ", nodes);
	return nodes;
}

void ConvertFen(const char* file)
{
	;
}

void SaveDiagram(char* file, int num)
{
	printf("save");
	diagram_file = fopen(file, "w");
	if (!diagram_file)
	{
		printf("Diagram missing.\n");
		return;
	}

	char b[256];
	strcpy(b, "mate");
	printf(" b %s", b);

	//char snum[256];
	//itoa(num,snum,10);

	fwrite(b, sizeof(char), sizeof(b), diagram_file);
	//fwrite(snum,sizeof(char), sizeof(snum),diagram_file);
	if (diagram_file)
		fclose(diagram_file);
	diagram_file = NULL;

}

char cGetFile(int sq)
{
	char c = col[sq] + 97;
	return c;
}

char cGetRank(int sq)
{
	char c = row[sq] + 49;
	return c;
}

char cPiece(int n)
{
	char c = 0;
	switch (n)
	{
	case 0: c = 'P'; break;
	case 1: c = 'N'; break;
	case 2: c = 'B'; break;
	case 3: c = 'R'; break;
	case 4: c = 'Q'; break;
	case 5: c = 'K'; break;
	}
	return c;
}

void ShowTime()
{
	int t;
	double nps;
	t = GetTime() - start_time;
	printf("\nTime: %d ms\n", t);
	if (t > 0)
		nps = (double)nodes / (double)t;
	else
		nps = 0;
	nps *= 1000.0;

	printf("\nNodes per second: %d (Score: %.3f)\n", (int)nps, (float)nps / 243169.0);

}

int CountRoot()
{
	int c = 0;
	int ch = Attack(xside, pieces[side][5][0]);

	ply = 0;
	gen(ch);
	for (int x = 0; x < first_move[1]; x++)
	{
		if (!MakeMove(move_list[x].from, move_list[x].to, move_list[x].flags))
			continue;
		else
		{
			c++;
			takeback();
			move_list[0].from = move_list[x].from;
			move_list[0].to = move_list[x].to;
			move_list[0].flags = move_list[x].flags;
		}
	}
	return c;
}
