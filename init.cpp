#include "globals.h"

#define MAX_PLY	48

#define BITBOARD unsigned __int64

#define WHITE_KING_CASTLE 1
#define WHITE_QUEEN_CASTLE 2
#define BLACK_KING_CASTLE 4
#define BLACK_QUEEN_CASTLE 8

int b[64];
int side;
int xside;
int castle;  
int fifty;

int ply;
int hply;

move_data move_list[GEN_STACK];

int first_move[MAX_PLY];

int history[64][64];
game game_list[HIST_STACK];

int max_time;
int max_depth;

int start_time;
int stop_time;

int nodes; 
int cut_nodes;
int first_nodes;
int av_nodes;

int mailbox[120] = {
	 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	 -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
	 -1,  8,  9, 10, 11, 12, 13, 14, 15, -1,
	 -1, 16, 17, 18, 19, 20, 21, 22, 23, -1,
	 -1, 24, 25, 26, 27, 28, 29, 30, 31, -1,
	 -1, 32, 33, 34, 35, 36, 37, 38, 39, -1,
	 -1, 40, 41, 42, 43, 44, 45, 46, 47, -1,
	 -1, 48, 49, 50, 51, 52, 53, 54, 55, -1,
	 -1, 56, 57, 58, 59, 60, 61, 62, 63, -1,
	 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

int mailbox64[64] = {
	21, 22, 23, 24, 25, 26, 27, 28,
	31, 32, 33, 34, 35, 36, 37, 38,
	41, 42, 43, 44, 45, 46, 47, 48,
	51, 52, 53, 54, 55, 56, 57, 58,
	61, 62, 63, 64, 65, 66, 67, 68,
	71, 72, 73, 74, 75, 76, 77, 78,
	81, 82, 83, 84, 85, 86, 87, 88,
	91, 92, 93, 94, 95, 96, 97, 98
};

bool slide[6] = {
	false, false, true, true, true, false
};

int offsets[6] = {
	0, 8, 4, 4, 8, 8
};

int offset[6][8] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	{ -21, -19, -12, -8, 8, 12, 19, 21 },
	{ -11, -9, 9, 11, 0, 0, 0, 0 },
	{ -10, -1, 1, 10, 0, 0, 0, 0 },
	{ -11, -10, -9, -1, 1, 9, 10, 11 },
	{ -11, -10, -9, -1, 1, 9, 10, 11 }
};

int castle_mask[64] = {//reversed
	13, 15, 15, 15, 12, 15, 15, 14,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	7, 15, 15, 15,  3, 15, 15, 11
};

char piece_char[6] = {
	'P', 'N', 'B', 'R', 'Q', 'K'
};

int ROOK_TRAPPED;

int kmoves[64][8];
int knightmoves[64][8];
int kingmoves[64][8];

//bitboard.cpp
int pawnleft[2][64];
int pawnright[2][64];
int pawnplus[2][64];
int pawndouble[2][64];

int getdir[64][64];
int squares[2][64];
int difference[64][64];
int pawn_difference[64][64];
int orthog[64][64];
int diagonal[64][64];

int rank[2][64];
int lastsquare[2][64];

//extend.cpp
int extend[MAX_PLY];
int PlyMove[MAX_PLY];
int PlyType[MAX_PLY];

//board.cpp
int cut;
int done[1000];
int drawn;

int pawns[2];
int pieces[2][6][10];
int total[2][6];

int Num[2];

int InCheck[MAX_PLY];

//moves.cpp
int indexpawn[64];
int table_score[2];
int table_bishop[2][2];
int table_rook[2][2];
int kingside[2];
int queenside[2];
int kingattack[2];
int queenattack[2];
int KingSide[2][64];
int QueenSide[2][64];
int KingSide2[2][64];
int QueenSide2[2][64];
int PawnBlocked[2][64];

//eval.cpp
int KingPawn[2][64];
int pawn_eval[2][MAX_PLY];
int king_pawn[2][2][512];
int captures[MAX_PLY];
int Threat[MAX_PLY];
int KingEval[2][MAX_PLY];

int start_piece_mat[2];
int start_pawn_mat[2];

int passed[2][64];
int adjacent_passed[2][64];
int defended_passed[2][64];

//search.cpp

int currentmax;
int threatdepth;

int Alpha, Beta;//threats

int castle_start[64];
int castle_dest[64];

int piece_value[6] = { 100, 300, 300, 500, 900, 0 };

int pawn_score[64] = {
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   2,   4, -12, -12,   4,   2,   0,//-10 was -8
	  0,   2,   4,   4,   6,   4,   2,   0,//d3 was 6 12/5/14
	  0,   2,   4,   8,   8,   4,   2,   0,
	  0,   2,   4,   8,   8,   4,   2,   0,
	  4,   8,  10,  16,  16,  10,   8,   4,//a6 h6 was 4
	  0,  0,  0,   0,   0,  0,  0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0
};

int knight_score[64] = {
	-30, -16, -10,  -8,  -8, -10, -20, -30,
	-16, -6,   -2,   0,   0,   -2, -6, -16,
	-8,   -2,   4,   6,   6,   4,   -2, -8,
	-5,   0,   6,   8,   8,   6,   0, -5,
	-5,   0,   6,   8,   8,   6,   0, -5,
	-10,   -2,   4,   6,   6,   4,   -2, -10,
	-20, -10,   -2,   0,   0,   -2, -10, -20,
	-150, -20, -10, -5, -5, -10, -20, -150
};

int bishop_score[64] = {
	-10, -10, -12, -10, -10, -12, -10, -10,//kb -14
	  0,   4,   4,   4,   4,   4,   6,   0,
	  2,   4,   6,   6,   6,   6,   4,   2,
	  2,   4,   6,   8,   8,   6,   4,   2,
	  2,   4,   6,   8,   8,   6,   4,   2,
	  2,   4,   6,   6,   6,   6,   4,   2,
	-10,   4,   4,   4,   4,   4,   4, -10,
	-10, -10, -10, -10, -10, -10, -10, -10
};

int rook_score[64] = {
	4, 4,  4,  6, 6,  4, 4, 4,
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0,
	20, 20, 20, 20, 20, 20, 20, 20,
	10, 10, 10, 10, 10, 10, 10, 10
};

int queen_score[64] = {
	-10, -10,  -6,  -4,  -4,  -6, -10, -10,
	-10,   2,   2,   2,   2,   2,   2, -10,
	  2,   2,   2,   3,   3,   2,   2,   2,
	  2,   2,   3,   4,   4,   3,   2,   2,
	  2,   2,   3,   4,   4,   3,   2,   2,
	  2,   2,   2,   3,   3,   2,   2,   2,
	-10,   2,   2,   2,   2,   2,   2, -10,
	-10, -10,   2,   2,   2,   2, -10, -10
};

int king_score[64] = {
	 20,  20, -20, -40,  10, -60,  -20,  20,
	 10,  20, -25, -30, -30, -45,  20,  10,    
	-24, -24, -24, -24, -24, -24, -24, -24,
	-24, -24, -24, -24, -24, -24, -24, -24,
	-48, -48, -48, -48, -48, -48, -48, -48,
	-48, -48, -48, -48, -48, -48, -48, -48,
	-48, -48, -48, -48, -48, -48, -48, -48,
	-48, -48, -48, -48, -48, -48, -48, -48
};

int king_endgame_score[64] = {
	  0,  10,  20,  25,  25,  20,  10,   0,
	 10,  20,  30,  40,  40,  30,  20,  10,
	 20,  30,  40,  50,  50,  40,  30,  20,
	 25,  40,  50,  60,  60,  50,  40,  25,
	 25,  40,  50,  60,  60,  50,  40,  25,
	 20,  30,  40,  50,  50,  40,  30,  20,
	 10,  20,  30,  40,  40,  30,  20,  10,
	  0,  10,  20,  25,  25,  20,  10,   0
};

int LeftTable[64] = {
	  0, 25,  25,  20,  10,   0, 0, 0,
	 10, 40,  40,  30,  20,  10, 5, 0,
	 20, 50,  50,  40,  30,  20, 5, 0,
	 25, 60,  60,  50,  40,  25, 5, 0,
	 25, 60,  60,  50,  40,  25, 5, 0,
	 20, 50,  50,  40,  30,  20, 5, 0,
	 10, 40,  40,  30,  20,  10, 5, 0,
	  0, 25,  25,  20,  10,   0,  0, 0
};

int RightTable[64] = {
	 0, 0,  0,  10,  20,  25,  25,  0,
	 0, 5, 10,  20,  30,  40,  40,  10,
	 0, 5, 20,  30,  40,  50,  50,  20,
	 0, 5, 25,  40,  50,  60,  60,  25,
	 0, 5, 25,  40,  50,  60,  60,  25,
	 0, 5, 20,  30,  40,  50,  50,  20,
	 0, 5, 10,  20,  30,  40,  40,  10,
	 0, 0,  0,  10,  20,  25,  25,  0
};

int KingPawnLess[64] = {
	-10, 0,  0,  0, 0,  0, 0, -10,
	0, 0,  5,   5,  5,   5,  5, 0,
	0, 5,  20,  20, 20,  20, 5, 0,
	0, 5,  20,  10, 10,  20, 5, 0,
	0, 5,  20,  10, 10,  20, 5, 0,
	0, 5,  20,  20, 20,  20, 5, 0,
	0, 0,  5,   5,  5,   5,  5, 0,
	-10, 0,  0,  0, 0,  0, 0, -10
};

int king_side2[64] = {
0, 0,  0,  0, 0,  0, 0, 0,
0, 0,  0,  0, 0,  0, 0, 0,
0, 0,  0,  0, 0,-20,-10, 0,
0, 0,  0,  0,-2,  2, 2, 2,
0, 0,  0,  0, 0,  4, 8, 8,//was 4 not 8 on h-file
0, 0,  0,  0, 0,  4, 8, 8,
0, 0,  0,  0, 0,  4, 8, 8,
0, 0,  0,  0, 0,  0, 0, 0
};

int queen_side2[64] = {
0, 0,  0,  0, 0,  0, 0, 0,
0, 0,  0,  0, 0,  0, 0, 0,
0,-10,-20, 0, 0,  0, 0, 0,
2, 2,  2, -2, 0,  0, 0, 0,
8, 8,  4,  0, 0,  0, 0, 0,
8, 8,  4,  0, 0,  0, 0, 0,
8, 8,  4,  0, 0,  0, 0, 0,
0, 0,  0,  0, 0,  0, 0, 0
};

int Flip[64] = {
	 56,  57,  58,  59,  60,  61,  62,  63,
	 48,  49,  50,  51,  52,  53,  54,  55,
	 40,  41,  42,  43,  44,  45,  46,  47,
	 32,  33,  34,  35,  36,  37,  38,  39,
	 24,  25,  26,  27,  28,  29,  30,  31,
	 16,  17,  18,  19,  20,  21,  22,  23,
	  8,   9,  10,  11,  12,  13,  14,  15,
	  0,   1,   2,   3,   4,   5,   6,   7
};

int knight_total[64] = {
   2, 3,  4,  4, 4,  4, 3, 2,
   3, 4,  6,  6, 6,  6, 4, 3,
   4, 6,  8,  8, 8,  8, 6, 4,
   4, 6,  8,  8, 8,  8, 6, 4,
   4, 6,  8,  8, 8,  8, 6, 4,
   4, 6,  8,  8, 8,  8, 6, 4,
   3, 4,  6,  6, 6,  6, 4, 3,
   2, 3,  4,  4, 4,  4, 3, 2
};

int king_total[64] = {
   3, 5,  5,  5, 5,  5, 5, 3,
   5, 8,  8,  8, 8,  8, 8, 5,
   5, 8,  8,  8, 8,  8, 8, 5,
   5, 8,  8,  8, 8,  8, 8, 5,
   5, 8,  8,  8, 8,  8, 8, 5,
   5, 8,  8,  8, 8,  8, 8, 5,
   5, 8,  8,  8, 8,  8, 8, 5,
   3, 5,  5,  5, 5,  5, 5, 3
};

int queen_total[64] = {
  3, 3,  5,  5, 5,  5, 3, 3,
  3, 3,  5,  5, 5,  5, 3, 3,
  5, 5,  8,  8, 8,  8, 5, 5,
  5, 5,  8,  8, 8,  8, 5, 5,
  5, 5,  8,  8, 8,  8, 5, 5,
  5, 5,  8,  8, 8,  8, 5, 5,
  3, 3,  5,  5, 5,  5, 3, 3,
  3, 3,  5,  5, 5,  5, 3, 3
};
//   score -= rank[s][sq]*5+10;
int knight_trapped[64] = {
	15, 15,  15,  15, 15,  15, 15, 15,
	15, 15,  15,  15, 15,  15, 15, 15,
	25, 25,  25,  25, 25,  25, 25, 25,
	35, 35,  35,  35, 35,  35, 35, 35,
	45, 45,  45,  45, 45,  45, 45, 45,
	55, 55,  55,  55, 55,  55, 55, 55,
	65, 65,  65,  65, 65,  65, 65, 65,
	75, 75,  75,  75, 75,  75, 75, 75
};

int bishop_trapped[64] = {
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0
};

int rook_trapped[64] = {
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0
};

int queen_trapped[64] = {
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0
};

int pawn_blocked[64] = {
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  6,  8, 8,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0,
	4, 4,  4,  4, 4,  4, 4, 4,
	40, 40,  40,  40, 40,  40, 40, 40,
	60, 60,  60,  60, 60,  60, 60, 60
};

int isolated[64] =
{
0, 0, 0, 0, 0, 0, 0, 0,
5, 10, 10, 15, 15, 10, 10, 5,
6, 10, 10, 15, 15, 10, 10, 6,
10, 10, 10, 12, 12, 10, 10, 10,
10, 10, 10, 12, 12, 10, 10, 10,
6, 10, 10, 15, 15, 10, 10, 6,
5, 10, 10, 15, 15, 10, 10, 5,
0, 0, 0, 0, 0, 0, 0, 0
};

int kingloc[64] = {
0, 0, 0, 2, 2, 1, 1, 1,
0, 0, 0, 2, 2, 1, 1, 1,
2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2,
0, 0, 0, 2, 2, 1, 1, 1,
0, 0, 0, 2, 2, 1, 1, 1
};

int defended_pp[64] = {//25; 10 40 35 45 300
	0, 0,  0,  0, 0,  0, 0, 0,
	45, 45,  45,  45, 45,  45, 45, 45,
	35, 35,  35,  35, 35,  35, 35, 35,
	10, 10,  10,  10, 10,  10, 10, 10,
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0
};

int adjacent_pp[64] = {
	0, 0,  0,  0, 0,  0, 0, 0,
	300, 300,  300,  300, 300,  300, 300, 300,
	40, 40,  40,  40, 40,  40, 40, 40,
	25, 25,  25,  25, 25,  25, 25, 25,
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0,
	0, 0,  0,  0, 0,  0, 0, 0
};

/*
more tables:
isolated
doubled
backward
rams
supported passed
etc
*/

int piece_mat[2];
int pawn_mat[2];

int PieceScore[2][6][64];
int PawnScore[2][64];
int KnightScore[2][64];
int BishopScore[2][64];
int RookScore[2][64];
int QueenScore[2][64];
int KingScore[2][64];

int a_nodes;
int b_nodes;
int check_history[6][64];
int sac_history[6][64];//

int behind_queen;
int king_defends;
int Swap[6][100][100][3][3][2][2];
int List[2][12];

int scale[200];

int KingEndgame[2][64]; 

extern int pawnless[3][3][3][2];
extern int reduce[2][64][64];

void SetBits();
int popCount(BITBOARD x);
void SetPawnless();
int GetBest(int ply);
int GetBest2(int ply);

void SetBoard();

void StartGame()
{
	side = 0;
	xside = 1;
	castle = 15;
	fifty = 0;
	ply = 0;
	hply = 0;
	first_move[0] = 0;
	NewPosition();
	SetBoard();
}

void SetUp()
{
	side = 0;
	xside = 1;
	castle = 15;
	fifty = 0;
	ply = 0;
	hply = 0;
	first_move[0] = 0;
	castle_start[G1] = H1;
	castle_dest[G1] = F1;
	castle_start[C1] = A1;
	castle_dest[C1] = D1;
	castle_start[G8] = H8;
	castle_dest[G8] = F8;
	castle_start[C8] = A8;
	castle_dest[C8] = D8;
	SetBits();
	SetScores();
	SetKingPawnTable();
	SetPassed();
	PopSwap();
	SetPawnless();
	SetNullDepth();
	SetBoard();
}

void NewPosition()
{
	piece_mat[0] = pawn_mat[0] = table_score[0] = 0;
	piece_mat[1] = pawn_mat[1] = table_score[1] = 0;

	kingside[0] = queenside[0] = 0;
	kingside[1] = queenside[1] = 0;
	kingattack[0] = queenattack[0] = 0;
	kingattack[1] = queenattack[1] = 0;

	bit_pieces[0][0] = bit_pieces[1][0] = 0;
	bit_pieces[0][1] = bit_pieces[1][1] = 0;
	bit_pieces[0][2] = bit_pieces[1][2] = 0;
	bit_pieces[0][3] = bit_pieces[1][3] = 0;
	bit_pieces[0][4] = bit_pieces[1][4] = 0;
	bit_pieces[0][5] = bit_pieces[1][5] = 0;

	bit_units[0] = bit_units[1] = 0;
	bit_all = 0;
/*
	pieces[0][1][0] = pieces[0][1][1] = -1;
	pieces[1][1][0] = pieces[1][1][1] = -1;
	pieces[0][2][0] = pieces[0][2][1] = -1;
	pieces[1][2][0] = pieces[1][2][1] = -1;
	pieces[0][3][0] = pieces[0][3][1] = -1;
	pieces[1][3][0] = pieces[1][3][1] = -1;//?
*/
	/*
	int s1 = 0;
	int s2 = 0;
	passed_list[0] = 0;
	passed_list[1] = 0;
	s1 = EvalPawns(0, 1);
	s2 = EvalPawns(1, 0);
	AddPawnHash(s1, s2, passed_list[0], passed_list[1]);
	*/
	currentkey = GetKey();
	currentlock = GetLock();
	currentpawnkey = GetPawnKey();
	currentpawnlock = GetPawnLock();
}

void SetBoard()
{
	memset(bit_pieces, 0, sizeof(bit_pieces));
	memset(bit_units, 0, sizeof(bit_units));
	memset(total, 0, sizeof(total));
	memset(pieces, 0, sizeof(pieces));
	bit_all = 0;

	AddPiece(0, R, A1);
	AddPiece(0, N, B1);
	AddPiece(0, B, C1);
	AddPiece(0, Q, D1);
	AddPiece(0, K, E1);
	AddPiece(0, B, F1);
	AddPiece(0, N, G1);
	AddPiece(0, R, H1);

	AddPiece(1, R, A8);
	AddPiece(1, N, B8);
	AddPiece(1, B, C8);
	AddPiece(1, Q, D8);
	AddPiece(1, K, E8);
	AddPiece(1, B, F8);
	AddPiece(1, N, G8);
	AddPiece(1, R, H8);

	for (int i = A2; i <= H2; i++)
	{
		AddPiece(0, 0, i);
	}
	for (int i = A7; i <= H7; i++)
	{
		AddPiece(1, 0, i);
	}
	for (int i = A3; i <= H6; i++)
	{
		b[i] = 6;
	}
	currentkey = GetKey();
	currentlock = GetLock();
	currentpawnkey = GetPawnKey();
	currentpawnlock = GetPawnLock();
}

void SetPassed()
{
	int v, w;
	for (int x = 0; x < 64; x++)
	{
		switch (row[x]) 
		{
		case 0:v = 0; w = 800; break;
		case 1:v = 10; w = 120; break;
		case 2:v = 15; w = 70; break;
		case 3:v = 24; w = 40; break;
		case 4:v = 40; w = 24; break; 
		case 5:v = 70; w = 15; break; 
		case 6:v = 120; w = 10; break;
		case 7:v = 800; w = 0; break;  
		default:
			v = 0; w = 0;
		}
		passed[0][x] = v - PieceScore[0][0][x];
		passed[1][x] = w - PieceScore[1][0][x];
	}
}

void SetPawnless()
{
	memset(endmatrix, 0, sizeof(endmatrix));

	////draw by material reduction
	endmatrix[3][0][0][0] = 77;
	endmatrix[0][0][3][0] = 77;
	endmatrix[3][1][0][0] = 77;
	endmatrix[0][0][3][1] = 77;

	endmatrix[3][0][3][0] = 77;
	endmatrix[3][1][3][0] = 77;
	endmatrix[3][0][3][1] = 77;
	endmatrix[3][1][3][1] = 77;

	endmatrix[6][0][0][0] = 9900;
	endmatrix[0][0][6][0] = -9900;
	endmatrix[6][1][0][0] = 9900;
	endmatrix[0][0][6][1] = -9900;
	endmatrix[6][2][0][0] = 77;
	endmatrix[0][0][6][2] = -77;

	endmatrix[5][0][0][0] = 9900;
	endmatrix[0][0][5][0] = -9900;
	endmatrix[9][0][0][0] = 9900;
	endmatrix[0][0][9][0] = -9900;
}

void SetScores()
{
	for (int x = 0; x < 64; x++)
	{
		PieceScore[0][0][x] = pawn_score[x];
		PieceScore[0][1][x] = knight_score[x] + 10;
		PieceScore[0][2][x] = bishop_score[x] + 10;
		PieceScore[0][3][x] = rook_score[x];
		PieceScore[0][4][x] = queen_score[x] + 60;
		PieceScore[0][5][x] = 0;
		PieceScore[1][0][x] = pawn_score[Flip[x]];
		PieceScore[1][1][x] = knight_score[Flip[x]] + 10;
		PieceScore[1][2][x] = bishop_score[Flip[x]] + 10;
		PieceScore[1][3][x] = rook_score[Flip[x]];
		PieceScore[1][4][x] = queen_score[Flip[x]] + 60;
		PieceScore[1][5][x] = 0;

		KingScore[0][x] = king_score[x];
		KingScore[1][x] = king_score[Flip[x]];

		QueenSide2[1][x] = queen_side2[x];
		KingSide2[1][x] = king_side2[x];

		KingEndgame[0][x] = king_endgame_score[x];
		KingEndgame[1][x] = king_endgame_score[x];

		PawnBlocked[0][x] = pawn_blocked[x];
		PawnBlocked[1][x] = pawn_blocked[Flip[x]];

		defended_passed[0][x] = defended_pp[x];
		defended_passed[1][x] = defended_pp[Flip[x]];
		
		adjacent_passed[0][x] = defended_pp[x];
		adjacent_passed[1][x] = defended_pp[Flip[x]];
	}
	/*
	KnightTrapped[0][x] = knight_trapped[x];
	BishopTrapped[0][x] = bishop_trapped[x];
	RookTrapped[0][x] = rook_trapped[x];
	QueenTrapped[0][x] = queen_trapped[x];
	KnightTrapped[1][x] = knight_trapped[Flip[x]];
	BishopTrapped[1][x] = bishop_trapped[Flip[x]];
	RookTrapped[1][x] = rook_trapped[Flip[x]];
	QueenTrapped[1][x] = queen_trapped[Flip[x]];
	*/
	ROOK_TRAPPED = PieceScore[0][5][G1] + 10;
}

void SetKingPawnTable()
{
	memset(KingSide, 0, sizeof(KingSide));
	memset(QueenSide, 0, sizeof(QueenSide));
	QueenSide[0][A2] = KingSide[0][H2] = QueenSide[1][A7] = KingSide[1][H7] = 25;
	QueenSide[0][B2] = KingSide[0][G2] = QueenSide[1][B7] = KingSide[1][G7] = 25;
	QueenSide[0][C2] = KingSide[0][F2] = QueenSide[1][C7] = KingSide[1][F7] = 15;//12

	QueenSide[0][A3] = KingSide[0][H3] = QueenSide[1][A6] = KingSide[1][H6] = 25;
	QueenSide[0][B3] = KingSide[0][G3] = QueenSide[1][B6] = KingSide[1][G6] = 15;
	QueenSide[0][C3] = KingSide[0][F3] = QueenSide[1][C6] = KingSide[1][F6] = 10;//6; 21/8

	QueenSide[0][A4] = KingSide[0][H4] = QueenSide[1][A5] = KingSide[1][H5] = 5;
	QueenSide[0][B4] = KingSide[0][G4] = QueenSide[1][B5] = KingSide[1][G5] = 5;
	QueenSide[0][C4] = KingSide[0][F4] = QueenSide[1][C5] = KingSide[1][F5] = 5;

	QueenSide[0][A5] = KingSide[0][H5] = QueenSide[1][A4] = KingSide[1][H4] = 1;//2;
	QueenSide[0][B5] = KingSide[0][G5] = QueenSide[1][B4] = KingSide[1][G4] = 1;//2;
	QueenSide[0][C5] = KingSide[0][F5] = QueenSide[1][C4] = KingSide[1][F4] = 1;//2;

	memset(KingSide2, 0, sizeof(KingSide2));
	memset(QueenSide2, 0, sizeof(QueenSide2));
	QueenSide2[0][A2] = KingSide2[0][H2] = QueenSide2[1][A7] = KingSide2[1][H7] = 10;
	QueenSide2[0][B2] = KingSide2[0][G2] = QueenSide2[1][B7] = KingSide2[1][G7] = 10;
	QueenSide2[0][C2] = KingSide2[0][F2] = QueenSide2[1][C7] = KingSide2[1][F7] = 6;

	QueenSide2[0][A3] = KingSide2[0][H3] = QueenSide2[1][A6] = KingSide2[1][H6] = 10;//8 was 4
	QueenSide2[0][B3] = KingSide2[0][G3] = QueenSide2[1][B6] = KingSide2[1][G6] = 10;
	QueenSide2[0][C3] = KingSide2[0][F3] = QueenSide2[1][C6] = KingSide2[1][F6] = 6;

	QueenSide2[0][A4] = KingSide2[0][H4] = QueenSide2[1][A5] = KingSide2[1][H5] = 10;
	QueenSide2[0][B4] = KingSide2[0][G4] = QueenSide2[1][B5] = KingSide2[1][G5] = 10;
	QueenSide2[0][C4] = KingSide2[0][F4] = QueenSide2[1][C5] = KingSide2[1][F5] = 6;

	QueenSide2[0][A5] = KingSide2[0][H5] = QueenSide2[1][A4] = KingSide2[1][H4] = 5;//-2;
	QueenSide2[0][B5] = KingSide2[0][G5] = QueenSide2[1][B4] = KingSide2[1][G4] = 5;//-4;
	QueenSide2[0][C5] = KingSide2[0][F5] = QueenSide2[1][C4] = KingSide2[1][F4] = 2;//-8;

	QueenSide2[0][A6] = KingSide2[0][H6] = QueenSide2[1][A3] = KingSide2[1][H3] = 0;
	QueenSide2[0][B6] = KingSide2[0][G6] = QueenSide2[1][B3] = KingSide2[1][G3] = -10;
	QueenSide2[0][C6] = KingSide2[0][F6] = QueenSide2[1][C3] = KingSide2[1][F3] = -20;

	QueenSide2[0][D5] = KingSide2[0][E5] = QueenSide2[1][D4] = KingSide2[1][E4] = -2;

	for (int x = 0; x < 200; x++)
	{
		scale[x] = x;
		if (x < 20)
			scale[x] -= 40;
		else if (x < 30)
			scale[x] -= 20;
		else if (x < 40)
			scale[x] -= 10;
		//else if(x>65)
	  //	  scale[x] = 65;
	}
}

void SetReductions()
{
	int best = 0;
	for (int hd = 1; hd < 64; ++hd) for (int mc = 1; mc < 64; ++mc)
	{
		double    pvRed = 0.00 + log(double(hd)) * log(double(mc)) / 3.00;
		double nonPVRed = 0.33 + log(double(hd)) * log(double(mc)) / 2.25;
		reduce[1][hd][mc] = int(pvRed >= 1.0 ? pvRed + 0.5 : 0);
		reduce[0][hd][mc] = int(nonPVRed >= 1.0 ? nonPVRed + 0.5 : 0);


		if (reduce[0][hd][mc] >= 2)
			reduce[0][hd][mc] += 1;
		if (reduce[0][hd][mc] > best)
			best = reduce[0][hd][mc];
	}
}
