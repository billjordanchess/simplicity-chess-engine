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

void GenCheck();
void GenCaptures();
void GenNon();

void ShowHelp();
void SetUp();
void xboard();

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

void DisplayResult();
void StartGame();
void SetMaterial();
void SetBits();
void Free();

int main()
{
	printf("Simplicity Chess Engine 1.5\n");
	printf("Version new reduce , 21/3/25\n");
	printf("Bill Jordan 2025\n");
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

	double nps;

	fixed_time = 0;

	RandomizeHash();
	SetBits();
	NewPosition();
	SetUp();
	srand(time(NULL));


	GenCheck();
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
			Think(fixed_time);
			turns++;

			currentkey = GetKey();
			currentlock = GetLock();
			lookup = LookUp2(side);

			if (hash_move.from != 0 || hash_move.to != 0)
			{
				hash_move.from = hash_move.from;
				hash_move.to = hash_move.to;
				Alg(hash_move.from, hash_move.to); printf("\n");
			}
			else
			{
				printf("(no legal moves)\n");
				computer_side = EMPTY;
				DisplayBoard();
				
				GenCheck();
				continue;
			}
			printf(" collisions %d ", collisions);
			printf("\n");
			collisions = 0;

			//if (ply == 0)
				;//	ShowAllEval(0);

			printf("Computer's move: %s\n", MoveString(hash_move.from, hash_move.to, 0)); printf("\n");
			MakeMove(hash_move.from, hash_move.to,0);
			
			printf("\n cut nodes %d ", cut_nodes);
			printf(" first nodes %d ", first_nodes);
			if (cut_nodes > 0)
				printf(" percent %d ", (first_nodes * 100 / cut_nodes));
			if(cut_nodes>0)
			printf(" av cut %d ",av_nodes/cut_nodes);
			if (a_nodes > 0)
				printf(" av sort %d \n", (100 - b_nodes * 100 / a_nodes));

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
			
			GenCheck();
			DisplayResult();
			printf(" turn "); printf("%d", turn++);
			DisplayBoard();
			continue;
		}
		printf("Enter move or command> ");
		if (scanf("%s", s) == EOF)
			return 0;

		if (!strcmp(s, "d"))
		{
			DisplayBoard();
			continue;
		}
		if (!strcmp(s, "f"))
		{
			flip = 1 - flip;
			DisplayBoard();
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
			
			GenCheck();
			for (int i = 0; i < first_move[1]; i++)
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
			UnMakeMove();
			ply = 0;
			if (first_move[0] != 0)
				first_move[0] = 0;
			
			GenCheck();
			continue;
		}
		if (!strcmp(s, "xboard"))
		{
			xboard();
			break;
		}

		ply = 0;
		first_move[0] = 0;
		
		GenCheck();
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

	for (i = 0; i < first_move[1]; i++)
		if (move_list[i].from == start && move_list[i].to == dest)
		{
			return i;
		}
	return -1;
}

void DisplayBoard()
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
				text = 124;
			else
				text = 36;
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
		if ((bit_all & mask[i]) && b[i] == EMPTY)
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
	char sFen[256];
	char sText[256];

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
			Think(fixed_time);
			SetMaterial();
			
			GenCheck();
			currentkey = GetKey();
			currentlock = GetLock();
			lookup = LookUp2(side);

			if (hash_move.from != 0 || hash_move.to != 0)
			{
				hash_move.from = hash_move.from;
				hash_move.to = hash_move.to;
			}
			else
				printf(" lookup=0 ");
			move_list[0].from = hash_move.from;
			move_list[0].to = hash_move.to;
			printf("move %s\n", MoveString(hash_move.from, hash_move.to, 0));

			MakeMove(hash_move.from, hash_move.to,0);

			ply = 0;
			
			GenCheck();
			DisplayResult();
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
			
			GenCheck();
			computer_side = 1;
			continue;
		}
		if (!strcmp(command, "black"))
		{
			side = 1;
			xside = 0;
			
			GenCheck();
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
			fixed_depth = 1;
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
			Think(fixed_time);
			currentkey = GetKey();
			currentlock = GetLock();
			lookup = LookUp2(side);
			if (hash_move.from == 0 && hash_move.to == 0)
				continue;
			printf("Hint: %s\n", MoveString(hash_move.from, hash_move.to, 0));
			continue;
		}
		if (!strcmp(command, "undo"))
		{
			if (!hply)
				continue;
			UnMakeMove();
			ply = 0;
			
			GenCheck();
			continue;
		}
		if (!strcmp(command, "remove"))
		{
			if (hply < 2)
				continue;
			UnMakeMove();
			UnMakeMove();
			ply = 0;
			
			GenCheck();
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
		if (!strcmp(command, "d"))
		{
			DisplayBoard();
			continue;
		}
		if (!strcmp(command, "sb"))
		{
			sFen[0] = 0;
			strcat_s(sFen, "c:\\users\\bill\\desktop\\bscp\\");//change this
			scanf("%s", sText);
			strcat_s(sFen, sText);
			strcat_s(sFen, ".fen");
			LoadDiagram(sFen, 1);
			DisplayBoard();
			continue;
		}
		
		GenCheck();

		m = ParseMove(line);
		if (m == -1 || !MakeMove(move_list[m].from, move_list[m].to,0))
			printf("Error (unknown command): %s\n", command);
		else
		{
			ply = 0;
			
			GenCheck();
			DisplayResult();
		}
	}
}

void DisplayResult()
{
	int i = 0;
	int flag = 0;

	SetMaterial();
	
	GenCheck();
	for (i = 0; i < first_move[1]; i++)
		if (MakeMove(move_list[i].from,move_list[i].to, move_list[i].flags))
		{
			UnMakeMove();
			flag=1;
			break;
		}
	if (pawn_mat[0] == 0 && pawn_mat[1] == 0 && piece_mat[0] <= 3 && piece_mat[1] <= 3)
	{
		printf("1/2-1/2 {Stalemate}\n");

		StartGame();
		computer_side = EMPTY;
		return;
	}
	if (i == first_move[1] && flag == 0)
	{
		DisplayBoard();
		printf(" end of game ");

		if (Attack(xside, pieces[side][K][0]))
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
	else if (Reps() >= 3)
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

int Reps()
{
	int r = 0;

	for (int i = hply; i >= hply - fifty; i -= 2)
		if (game_list[i].hash == currentkey && game_list[i].lock == currentlock)
			r++;
	return r;
}

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
		b[i] = EMPTY;
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
		case 'K': AddPiece(0, K, j); i++; break;
		case 'Q': AddPiece(0, Q, j); i++; break;
		case 'R': AddPiece(0, R, j); i++; break;
		case 'B': AddPiece(0, B, j); i++; break;
		case 'N': AddPiece(0, N, j); i++; break;
		case 'P': AddPiece(0, P, j); i++; break;
		case 'k': AddPiece(1, K, j); i++; break;
		case 'q': AddPiece(1, Q, j); i++; break;
		case 'r': AddPiece(1, R, j); i++; break; 
		case 'b': AddPiece(1, B, j); i++; break;
		case 'n': AddPiece(1, N, j); i++; break;
		case 'p': AddPiece(1, P, j); i++; break;
		default:
			break;
		}
		c++;
		if (ts[c] == ' ')
			break;
		if (i > 63)
			break;
	}
	printf(" ts %s ", ts);
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
	DisplayBoard();
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
	printf("sd - Sets the Search depth.\n");
	printf("st - Sets the time limit per move in seconds.\n");
	printf("sw - Switches sides.\n");
	printf("quit - Quits the program.\n");
	printf("undo - Takes back the last move.\n");
	printf("xboard - Starts xboard.\n");
}

void SetMaterial()
{
	int c;
	pawn_mat[0] = 0;
	pawn_mat[1] = 0;
	piece_mat[0] = 0;
	piece_mat[1] = 0;
	for (int x = 0; x < 64; x++)
	{
		if (b[x] != EMPTY)
		{
			if (bit_units[0] & mask[x])
				c = 0;
			else
				c = 1;
			if (b[x] == P)
				pawn_mat[c]++;
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

void GenCheck()
{
	int check = Check(xside, pieces[side][K][0]);
	first_move[ply + 1] = first_move[ply];
	if(check > -1)
		Evasion(check);
	else
	{
		GenCaptures();
		GenNon();
	}
}

/*
#include <iostream>
#include <sstream>
#include <string>

void SetBoardToStartingPosition();
void SetBoardFromFEN(const std::string& fen);
void MakeMove(const std::string& move);
std::string SearchBestMove(int depth, int movetime);
void WinBoardLoop(); // Your existing WinBoard code

void HandleUciPosition(const std::string& command) {
    std::istringstream ss(command);
    std::string token;
    ss >> token; // "position"
    
    std::string positionType;
    ss >> positionType; // "startpos" or "fen"

    if (positionType == "startpos") {
        SetBoardToStartingPosition();
    } else if (positionType == "fen") {
        std::string fen;
        while (ss >> token && token != "moves") {
            fen += token + " ";
        }
        SetBoardFromFEN(fen);
    }

    // Read moves (if any)
    while (ss >> token) {
        MakeMove(token);
    }
}

void HandleUciGo(const std::string& command) {
    int depth = -1;
    int movetime = -1;

    std::istringstream ss(command);
    std::string token;
    ss >> token; // "go"

    while (ss >> token) {
        if (token == "depth") {
            ss >> depth;
        } else if (token == "movetime") {
            ss >> movetime;
        }
    }

    std::string bestMove = SearchBestMove(depth, movetime);
    std::cout << "bestmove " << bestMove << std::endl;
}

void UciLoop() {
    std::string input;
    while (std::getline(std::cin, input)) {
        if (input == "uci") {
            std::cout << "id name MyEngine" << std::endl;
            std::cout << "id author MyName" << std::endl;
            std::cout << "uciok" << std::endl;
        } else if (input == "isready") {
            std::cout << "readyok" << std::endl;
        } else if (input.rfind("position", 0) == 0) {
            HandleUciPosition(input);
        } else if (input.rfind("go", 0) == 0) {
            HandleUciGo(input);
        } else if (input == "stop") {
            // Handle stopping search if necessary
        } else if (input == "quit") {
            break;
        }
    }
}

int main() {
    std::string firstCommand;
    std::getline(std::cin, firstCommand);

    if (firstCommand == "uci") {
        UciLoop();
    } else {
        WinBoardLoop(); // Default to WinBoard mode
    }
    return 0;
}

*/