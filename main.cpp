#include "stdafx.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <windows.h>
#include <iostream>
#include <sys/timeb.h>

#include "globals.h"

void ShowHelp();
void SetUp();
void xboard();

void ConvertFen(char* file);//
void SaveAsc(int side);//

int board_color[64] =
{
	1, 0, 1, 0, 1, 0, 1, 0,
	0, 1, 0, 1, 0, 1, 0, 1,
	1, 0, 1, 0, 1, 0, 1, 0,
	0, 1, 0, 1, 0, 1, 0, 1,
	1, 0, 1, 0, 1, 0, 1, 0,
	0, 1, 0, 1, 0, 1, 0, 1,
	1, 0, 1, 0, 1, 0, 1, 0,
	0, 1, 0, 1, 0, 1, 0, 1
};

int LoadDiagram(char* file, int);
void CloseDiagram();

FILE* diagram_file;
FILE* output_file;
char fen_name[256];

int flip = 0;

int computer_side;
int player[2];

int fixed_time;
int fixed_depth;
int turn = 0;

int collisions;

void PrintResult();
void StartGame();
void SetMaterial();
void SetBits();
void Free();

extern int hash_start, hash_dest;

int main()
{
	printf("Simplicity Chess Engine no null\n");
	printf("Version 1.0, 6/9/21\n");
	printf("Bill Jordan 2021\n");
	printf("FIDE Master and 2021 state champion.\n");
	printf("I have published a number of chess books\n");
	printf("including books on chess programming.\n");
	printf("My author page at Amazon is at billjordanchess\n");
	printf("https://www.amazon.com/-/e/B07F5WSFHZ \n");
	printf("\n");
	printf("\"help\" displays a list of commands.\n");
	printf("\n");

	char s[256];
	char sFen[256];
	char sText[256];

	int m;
	int turns = 0;
	int t;
	int lookup;
	int check;

	double nps;

	fixed_time = 0;

	RandomizeHash();
	SetBits();
	NewPosition();
	SetUp();

	//ConvertFen("c:\\diagrams\\pos.txt");

	check = Attack(xside, pieces[side][5][0]);
	gen(check);
	computer_side = EMPTY;
	player[0] = 0;
	player[1] = 0;
	max_time = 1 << 25;
	max_depth = 4;

	while (true)
	{
		if (side == computer_side)
		{
			player[side] = 1;
			think();
			turns++;

			currentkey = GetKey();
			currentlock = GetLock();
			lookup = LookUp2(side);

			if (hash_start != 0 || hash_dest != 0)
			{
				hash_start = hash_start;
				hash_dest = hash_dest;
				Alg(hash_start, hash_dest); printf("\n");
			}
			else
			{
				printf("(no legal moves)\n");
				computer_side = EMPTY;
				print_board();
				check = Attack(xside, pieces[side][5][0]);
				gen(check);
				continue;
			}
			printf(" collisions %d ", collisions);
			printf("\n");
			collisions = 0;

			printf("Computer's move: %s\n", MoveString(hash_start, hash_dest, 0)); printf("\n");
			MakeMove(hash_start, hash_dest,0);

			SetMaterial();

			t = GetTime() - start_time;
			printf("\nTime: %d ms\n", t);
			if (t > 0)
				nps = (double)nodes / (double)t;
			else
				nps = 0;
			nps *= 1000.0;

			printf("\nNodes per second: %d\n", (int)nps);
			ply = 0;

			first_move[0] = 0;
			check = Attack(xside, pieces[side][5][0]);
			gen(check);
			PrintResult();
			printf(" turn "); printf("%d", turn++);
			print_board();
			continue;
		}
		printf("Enter move or command> ");
		if (scanf("%s", s) == EOF)
			return 0;

		if (!strcmp(s, "d"))
		{
			print_board();
			continue;
		}
		if (!strcmp(s, "f"))
		{
			flip = 1 - flip;
			print_board();
			continue;
		}
		if (!strcmp(s, "go"))
		{
			computer_side = side;
			continue;
		}
		if (!strcmp(s, "help"))
		{
			ShowHelp();
			continue;
		}
		if (!strcmp(s, "moves"))
		{
			printf("Moves \n");
			check = Attack(xside, pieces[side][5][0]);
			gen(check);
			for (int i = 0; i < first_move[1]; ++i)
			{
				printf("%s\n", MoveString(move_list[i].from, move_list[i].to, 0));
			}
			continue;
		}
		if (!strcmp(s, "new"))
		{
			StartGame();
			computer_side = EMPTY;
			continue;
		}
		if (!strcmp(s, "on") || !strcmp(s, "p"))
		{
			computer_side = side;
			continue;
		}
		if (!strcmp(s, "off"))
		{
			computer_side = EMPTY;
			continue;
		}
		if (!strcmp(s, "quit"))
		{
			printf("Program exiting\n");
			break;
		}
		if (!strcmp(s, "sb"))
		{
			sFen[0] = 0;
			strcat_s(sFen, "c:\\users\\bill\\desktop\\bscp\\");//
			scanf("%s", sText);
			strcat_s(sFen, sText);
			strcat_s(sFen, ".fen");
			LoadDiagram(sFen, 1);
			continue;
		}
		if (!strcmp(s, "sd"))
		{
			scanf("%d", &max_depth);
			max_time = 1 << 25;
			fixed_depth = 1;
			continue;
		}
		if (!strcmp(s, "st"))
		{
			scanf("%d", &max_time);
			max_time *= 1000;
			max_depth = MAX_PLY;
			fixed_time = 1;
			continue;
		}
		if (!strcmp(s, "sw"))
		{
			side ^= 1;
			xside ^= 1;
			continue;
		}
		if (!strcmp(s, "undo"))
		{
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
		if (!strcmp(s, "xboard"))
		{
			xboard();
			break;
		}

		ply = 0;
		first_move[0] = 0;
		check = Attack(xside, pieces[side][5][0]);
		gen(check);
		m = ParseMove(s);
		//Attack 26/4/21
		if (m == -1 || !MakeMove(move_list[m].from, move_list[m].to,0))
		{
			printf("Illegal move. \n");
			printf(s); printf(" \n");
			MoveString(move_list[m].from, move_list[m].to, 0);
			if (m == -1)printf(" m = -1 \n");
		}
		if (game_list[hply - 1].flags & PROMOTE && (row[move_list[m].to] == 0 || row[move_list[m].to] == 7))
		{
			RemovePiece(xside, Q, move_list[m].to);
			if (s[4] == 'n' || s[4] == 'N')
				AddPiece(xside, N, move_list[m].to);
			else if (s[4] == 'b' || s[4] == 'B')
				AddPiece(xside, B, move_list[m].to);
			else if (s[4] == 'r' || s[4] == 'R')
				AddPiece(xside, R, move_list[m].to);
			else AddPiece(xside, Q, move_list[m].to);
		}
	}
	Free();
	return 0;
}

int ParseMove(char* s)
{
	int start, dest, i;

	if (s[0] < 'a' || s[0] > 'h' ||
		s[1] < '0' || s[1] > '9' ||
		s[2] < 'a' || s[2] > 'h' ||
		s[3] < '0' || s[3] > '9')
		return -1;

	start = s[0] - 'a';
	start += ((s[1] - '0') - 1) * 8;
	dest = s[2] - 'a';
	dest += ((s[3] - '0') - 1) * 8;

	for (i = 0; i < first_move[1]; ++i)
		if (move_list[i].from == start && move_list[i].to == dest)
		{
			/*
			if (s[4] == 'n' || s[4] == 'N')
				move_list[i].promote = 1;
			if (s[4] == 'b' || s[4] == 'B')
				move_list[i].promote = 2;
			else if (s[4] == 'r' || s[4] == 'R')
				move_list[i].promote = 3;
				*/
			return i;
		}
	return -1;
}
/*
print_board() displays the b
The console object is only used to display in colour.
*/
void print_board()
{
	HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	int text = 15;

	int i;
	int x = 0;
	int c;

	if (flip == 0)
		printf("\n8 ");
	else
		printf("\n1 ");

	for (int j = 0; j < 64; ++j)
	{
		if (flip == 0)
			i = Flip[j];
		else
			i = 63 - Flip[j];
		c = 6;
		if (bit_units[0] & mask[i]) c = 0;
		if (bit_units[1] & mask[i]) c = 1;
		switch (c)
		{
		case EMPTY:
			if (board_color[i] == 0)
				text = 127;
			else
				text = 34;
			SetConsoleTextAttribute(hConsole, text);

			printf("  ");
			SetConsoleTextAttribute(hConsole, 15);
			break;
		case 0:
			if (board_color[i] == 0)
				text = 126;
			else
				text = 46;
			SetConsoleTextAttribute(hConsole, text);
			printf(" %c", piece_char[b[i]]);
			SetConsoleTextAttribute(hConsole, 15);
			break;

		case 1:
			if (board_color[i] == 0)
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
		if ((bit_all & mask[i]) && b[i] == 6)
			if (x == 0)
				printf(" %d", c);
			else
				printf("%d ", c);
		if (b[i] < 0 || b[i]>6)
			if (x == 0)
				printf(" %d.", b[i]);
			else
				printf("%d ", b[i]);
		if (flip == 0)
		{
			if ((j + 1) % 8 == 0 && j != 63)
				printf("\n%d ", row[i]);
		}
		else
		{
			if ((j + 1) % 8 == 0 && row[i] != 7)
				printf("\n%d ", row[j] + 2);
		}
	}
	if (flip == 0)
		printf("\n\n   a b c d e f g h\n\n");
	else
		printf("\n\n   h g f e d c b a\n\n");
}

void xboard()
{
	int computer_side;
	char line[256], command[256];
	int m;
	int post = 0;
	int analyze = 0;
	int lookup;
	int check;

	signal(SIGINT, SIG_IGN);
	printf("\n");
	StartGame();
	fixed_time = 0;
	computer_side = EMPTY;

	while (true)
	{
		fflush(stdout);
		if (side == computer_side)
		{
			think();
			SetMaterial();
			check = Attack(xside, pieces[side][5][0]);
			gen(check);
			currentkey = GetKey();
			currentlock = GetLock();
			lookup = LookUp2(side);

			if (hash_start != 0 || hash_dest != 0)
			{
				hash_start = hash_start;
				hash_dest = hash_dest;
			}
			else
				printf(" lookup=0 ");

			move_list[0].from = hash_start;
			move_list[0].to = hash_dest;
			printf("move %s\n", MoveString(hash_start, hash_dest, 0));

			MakeMove(hash_start, hash_dest,0);

			ply = 0;
			check = Attack(xside, pieces[side][5][0]);
			gen(check);
			PrintResult();
			continue;
		}
		if (!fgets(line, 256, stdin))
			return;
		if (line[0] == '\n')
			continue;
		sscanf(line, "%s", command);
		if (!strcmp(command, "xboard"))
			continue;
		if (!strcmp(command, "new"))
		{
			StartGame();
			computer_side = 1;
			continue;
		}
		if (!strcmp(command, "quit"))
			return;
		if (!strcmp(command, "force"))
		{
			computer_side = EMPTY;
			continue;
		}
		if (!strcmp(command, "white"))
		{
			side = 0;
			xside = 1;
			check = Attack(xside, pieces[side][5][0]);
			gen(check);
			computer_side = 1;
			continue;
		}
		if (!strcmp(command, "black"))
		{
			side = 1;
			xside = 0;
			check = Attack(xside, pieces[side][5][0]);
			gen(check);
			computer_side = 0;
			continue;
		}
		if (!strcmp(command, "st"))
		{
			sscanf_s(line, "st %d", &max_time);
			max_time *= 1000;
			max_depth = MAX_PLY;
			fixed_time = 1;
			continue;
		}
		if (!strcmp(command, "sd"))
		{
			sscanf_s(line, "sd %d", &max_depth);
			max_time = 1 << 25;
			continue;
		}
		if (!strcmp(command, "time"))
		{
			sscanf_s(line, "time %d", &max_time);
			if (max_time < 200)
				max_depth = 1;
			else
			{
				max_time /= 2;
				max_depth = MAX_PLY;
			}
			continue;
		}
		if (!strcmp(command, "otim"))
		{
			continue;
		}
		if (!strcmp(command, "go"))
		{
			computer_side = side;
			continue;
		}
		if (!strcmp(command, "random"))
			continue;
		if (!strcmp(command, "level"))
			continue;
		if (!strcmp(command, "hard"))
			continue;
		if (!strcmp(command, "easy"))
			continue;
		if (!strcmp(command, "hint"))
		{
			think();
			currentkey = GetKey();
			currentlock = GetLock();
			lookup = LookUp2(side);
			if (hash_start == 0 && hash_dest == 0)
				continue;
			printf("Hint: %s\n", MoveString(hash_start, hash_dest, 0));
			continue;
		}
		if (!strcmp(command, "undo"))
		{
			if (!hply)
				continue;
			takeback();
			ply = 0;
			check = Attack(xside, pieces[side][5][0]);
			gen(check);
			continue;
		}
		if (!strcmp(command, "remove"))
		{
			if (hply < 2)
				continue;
			takeback();
			takeback();
			ply = 0;
			check = Attack(xside, pieces[side][5][0]);
			gen(check);
			continue;
		}
		if (!strcmp(command, "post"))
		{
			post = 2;
			continue;
		}
		if (!strcmp(command, "nopost"))
		{
			post = 0;
			continue;
		}

		first_move[0] = 0;
		check = Attack(xside, pieces[side][5][0]);
		gen(check);

		m = ParseMove(line);
		if (m == -1 || !MakeMove(move_list[m].from, move_list[m].to,0))
			printf("Error (unknown command): %s\n", command);
		else
		{
			ply = 0;
			check = Attack(xside, pieces[side][5][0]);
			gen(check);
			PrintResult();
		}
	}
}

void PrintResult()
{
	int i; i = 0;
	int flag = 0;
	int check;

	SetMaterial();
	check = Attack(xside, pieces[side][5][0]);
	gen(check);
	/*
	for (i = 0; i < first_move[1]; ++i)
		if (MakeMove(move_list[i].from,move_list[i].to))
		{
			takeback();
			flag=1;
			break;
		}
*/
	if (pawn_mat[0] == 0 && pawn_mat[1] == 0 && piece_mat[0] <= 300 && piece_mat[1] <= 300)
	{
		printf("1/2-1/2 {Stalemate}\n");

		StartGame();
		computer_side = EMPTY;
		return;
	}
	if (i == first_move[1] && flag == 0)
	{
		check = Attack(xside, pieces[side][5][0]);
		gen(check);
		print_board();
		printf(" end of game ");

		if (Attack(xside, NextBit(bit_pieces[side][K])))
		{
			if (side == 0)
			{
				printf("0-1 {Black mates}\n");
			}
			else
			{
				printf("1-0 {White mates}\n");
			}
		}
		else
		{
			printf("1/2-1/2 {Stalemate}\n");
		}
		StartGame();
		computer_side = EMPTY;
	}
	else if (reps() >= 3)
	{
		printf("1/2-1/2 {Draw by repetition}\n");
		StartGame();
		computer_side = EMPTY;
	}
	else if (fifty >= 100)
	{
		printf("1/2-1/2 {Draw by fifty move rule}\n");
		StartGame();
		computer_side = EMPTY;
	}
}

int reps()
{
	int r = 0;

	for (int i = hply; i >= hply - fifty; i -= 2)
		if (game_list[i].hash == currentkey && game_list[i].lock == currentlock)
			r++;
	return r;
}
/*
int LoadDiagram2(char* file, int num)
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
	gen(0);//deprec

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
*/
int LoadDiagram(char* file, int num)
{
	int x, n = 0;
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
	NewPosition();
	for (int i = 0; i < 64; i++)
	{
		b[i] = 6;
	}
	memset(bit_pieces, 0, sizeof(bit_pieces));
	memset(bit_units, 0, sizeof(bit_units));
	memset(total, 0, sizeof(total));
	memset(pieces, 0, sizeof(pieces));
	bit_all = 0;

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
		case 'K': AddPiece(0, 5, j); i++; break;
		case 'Q': AddPiece(0, 4, j); i++; break;
		case 'R': AddPiece(0, 3, j); i++; break;
		case 'B': AddPiece(0, 2, j); i++; break;
		case 'N': AddPiece(0, 1, j); i++; break;
		case 'P': AddPiece(0, 0, j); i++; break;
		case 'k': AddPiece(1, 5, j); i++; break;
		case 'q': AddPiece(1, 4, j); i++; break;
		case 'r': AddPiece(1, 3, j); i++; break;
		case 'b': AddPiece(1, 2, j); i++; break;
		case 'n': AddPiece(1, 1, j); i++; break;
		case 'p': AddPiece(1, 0, j); i++; break;
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

	castle = 0;
	while (ts[c])
	{
		switch (ts[c])
		{
		case '-': break;
		case 'K':if (bit_pieces[0][K] & mask[E1]) castle |= 1; break;
		case 'Q':if (bit_pieces[0][K] & mask[E1]) castle |= 2; break;
		case 'k':if (bit_pieces[1][K] & mask[E8]) castle |= 4; break;
		case 'q':if (bit_pieces[1][K] & mask[E8]) castle |= 8; break;
		default:break;
		}
		c++;
	}

	CloseDiagram();
	print_board();
	printf(" diagram # %d \n", num + count);
	count++;
	if (side == 0)
		printf("White to move\n");
	else
		printf("Black to move\n");
	printf(" %s \n", ts);
	currentkey = GetKey();
	currentlock = GetLock();
	currentpawnkey = GetPawnKey();
	currentpawnlock = GetPawnLock();
	hply = 10;
	return 0;
}

void CloseDiagram()
{
	if (diagram_file)
		fclose(diagram_file);
	diagram_file = NULL;
}

void ShowHelp()
{
	printf("d - Displays the b.\n");
	printf("f - Flips the b.\n");
	printf("go - Starts the engine.\n");
	printf("help - Displays help on the commands.\n");
	printf("moves - Displays of list of possible moves.\n");
	printf("new - Starts a new game .\n");
	printf("off - Turns the computer player off.\n");
	printf("on or p - The computer plays a move.\n");
	printf("sb - Loads a fen diagram.\n");
	printf("sd - Sets the search depth.\n");
	printf("st - Sets the time limit per move in seconds.\n");
	printf("sw - Switches sides.\n");
	printf("quit - Quits the program.\n");
	printf("undo - Takes back the last move.\n");
	printf("xboard - Starts xboard.\n");
}
/*
void SetUp()
{
	RandomizeHash();
	SetTables();
	SetMoves();
	InitBoard();
	computer_side = EMPTY;
	player[0] = 0;
	player[1] = 0;
	max_time = 1 << 25;
	max_depth = 4;
}

void StartGame()
{
	InitBoard();
	//Gen(side, xside);
}
*/
void SetMaterial()
{
	int c;
	pawn_mat[0] = 0;
	pawn_mat[1] = 0;
	piece_mat[0] = 0;
	piece_mat[1] = 0;
	for (int x = 0; x < 64; x++)
	{
		if (b[x] < 6)
		{
			if (bit_units[0] & mask[x])
				c = 0;
			else
				c = 1;
			if (b[x] == 0)
				pawn_mat[c] += 100;
			else
				piece_mat[c] += piece_value[b[x]];
		}
	}
}

int GetTime()
{
	struct timeb timebuffer;
	ftime(&timebuffer);
	return (timebuffer.time * 1000) + timebuffer.millitm;
}

char* MoveString(int start, int dest, int promote)
{
	static char str[6];

	char c;

	if (promote > 0) {
		switch (promote) {
		case N:
			c = 'n';
			break;
		case B:
			c = 'b';
			break;
		case R:
			c = 'r';
			break;
		default:
			c = 'q';
			break;
		}
		sprintf_s(str, "%c%d%c%d%c",
			col[start] + 'a',
			row[start] + 1,
			col[dest] + 'a',
			row[dest] + 1,
			c);
	}
	else
		sprintf_s(str, "%c%d%c%d",
			col[start] + 'a',
			row[start] + 1,
			col[dest] + 'a',
			row[dest] + 1);
	return str;
}

void ConvertFen(char* file)
{
	nodes = 1;//

	printf("convert");
	diagram_file = fopen(file, "r");
	if (!diagram_file)
	{
		printf("Diagram missing.\n");
		return;
	}
	//SaveTactics("c:\\diagrams\\output.txt", ts);
	//
	char a[256000] = "";
	char line[256];
	char text[500][256];
	int c = 0;

	if (diagram_file == NULL) perror("Error opening file");
	else {
		while (fgets(line, 256, diagram_file) != NULL)
		{
			strcat(a, line);
			strcpy(text[c], line);
			c++;
		}
		fclose(diagram_file);
	}

	output_file = fopen("c:\\diagrams\\output.txt", "w");
	for (int x = 1; x <= c; x++)
	{
		LoadDiagram(file, x);
		printf(" x %d ", x);
		SaveAsc(0);
		fwrite(text[x-1], sizeof(char), sizeof(text[x-1]), output_file);
		fwrite("\n", 1, 1, output_file);
	}
	if (output_file)
		fclose(output_file);
	output_file = NULL;
	printf("done");
	z();
	return;
	
	char p[2], n1[2], f[2], r[2];
	p[1] = 0;
	n1[1] = 0;
	f[1] = 0;
	r[1] = 0;

	char b[64000] = "";

	int kings = 0;
	int count;
	int flag = 0;

	for (int x = 0; x < strlen(a); x++)
	{
		flag = 0;
		if (a[x] == '[' && a[x + 1] != ']')
		{
			flag = 1;
			kings = 0;
			count = 0;
			x++;
			//strcat(b,"kg1,");//3/6/18 missing kings

			while (a[x] != ']')
			{
				if (a[x] == ',')//
				{
					x++;
					strcat(b, "/b");
					count = 0;
				}
				if (a[x + 1] == ']' || a[x + 2] == ']')
					break;
				p[0] = a[x] - 32;
				f[0] = a[x + 1];
				r[0] = a[x + 2];
				p[1] = 0;
				f[1] = 0;
				r[1] = 0;
				if (count > 0)
					strcat(b, ",");
				if (p[0] == 75)
				{
					kings++;
					if (kings > 1)
						strcat(b, "/b");
				}
				strcat(b, p);
				strcat(b, f);
				strcat(b, r);
				x += 3;
				count++;
			}
		}
		//5/6/18 if(flag==1)
		{
			while (a[x] != 10 && a[x] != 13)
			{
				p[0] = a[x];
				p[1] = 0;
				strcat(b, p);
				x++;
			}
			strcat(b, "\n");
		}
	}
	printf("%s", a);
	printf(" b %s", b);

	if (diagram_file)
		fclose(diagram_file);
	diagram_file = NULL;
	diagram_file = fopen("c:\\diagrams\\output.txt", "w");
	fwrite(b, sizeof(char), sizeof(b), diagram_file);
	if (diagram_file)
		fclose(diagram_file);
	diagram_file = NULL;
	printf("done");
	z();
}

void SaveAsc(int side)
{
	BITBOARD bb = 0;
	int sq;

	char a, b;
	char a2[2], b2[2];

	int c1;

	char position[256] = "$";

	if (side == 1)
		strcat(position, "w");

	for (int x = 0; x < 2; x++)
	{
		for (int y = 5; y >= 0; y--)
		{
			bb = bit_pieces[x][y];
			while (bb)
			{
				sq = NextBit(bb);
				if (x == 1 && y == 5)
					strcat(position, "/b");
				if (x == 0)
					switch (y)
					{
					case 0: strcat(position, "P"); break;
					case 1: strcat(position, "N"); break;
					case 2: strcat(position, "B"); break;
					case 3: strcat(position, "R"); break;
					case 4: strcat(position, "Q"); break;
					case 5: strcat(position, "K"); break;
					default:break;
					}
				else
				{
					switch (y)
					{
					case 0: strcat(position, "p"); break;
					case 1: strcat(position, "n"); break;
					case 2: strcat(position, "b"); break;
					case 3: strcat(position, "r"); break;
					case 4: strcat(position, "q"); break;
					case 5: strcat(position, "k"); break;
					default:break;
					}
				}
				a = col[sq] + 'a';
				b = row[sq] + 1;
				c1 = col[sq];
				a2[0] = a;
				a2[1] = 0;
				b2[0] = b + 48;
				b2[1] = 0;
				strcat(position, a2);
				strcat(position, b2);
				strcat(position, ",");
				bb = bb & ~mask[sq];
			}
		}
	}
	int last = strlen(position);
	if (last > 1)
	{
		position[last - 1] = ']';
		strcat(position, "\n");
	}
	
	//puzzles++;
	//printf(" %d \n",puzzles);
	//printf(" %s \n",position);

	//if (puzzles < 1000)
	{
		fwrite(position, sizeof(char), sizeof(position), output_file);
		fwrite("\n",1,1, output_file);
	}

}