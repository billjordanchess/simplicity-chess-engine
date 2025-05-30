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
game game_list[HIST_STACK];

int first_move[MAX_PLY];

int history[64][64];

int max_time;
int max_depth;
int start_time;
int stop_time;

ULONG nodes;
int cut_nodes;
int first_nodes;
int av_nodes;

int centi_pawns[9] = { 0,100,200,300,400,500,600,700,800 };
int centi_pieces[104];

int castle_mask[64] = {
	13, 15, 15, 15, 12, 15, 15, 14,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	7, 15, 15, 15,  3, 15, 15, 11
};

char piece_char[7] = {
	'P', 'N', 'B', 'R', 'Q', 'K', 'X'
};

//bitboard.cpp
int pawnleft[2][64];
int pawnright[2][64];
int pawnplus[2][64];
int pawndouble[2][64];

int squares[2][64];
int difference[64][64];
int pawn_difference[64][64];

int rank[2][64];
int lastsquare[2][64];

//extend.cpp
int extend[MAX_PLY];
int PlyMove[MAX_PLY];
int PlyType[MAX_PLY];

//board.cpp
int done[1000];

int pieces[2][6][10];
int total[2][6];

//moves.cpp
int table_score[2];
int kingside[2];
int queenside[2];
int kingattack[2];
int queenattack[2];
int KingSide[2][64];
int QueenSide[2][64];
int KingSide2[2][64];
int QueenSide2[2][64];
int PawnBlocked[2][64];

void SetKnightMoves();
void SetKingMoves();
void SetLineMoves();

//eval.cpp
int KingPawn[2][64];
int pawn_eval[2][MAX_PLY];
int king_pawn[2][2][512];
int captures[MAX_PLY];
int Threat[MAX_PLY];
int KingEval[2][MAX_PLY];

int passed[2][64];
int adjacent_passed[2][64];
int defended_passed[2][64];

//Search.cpp
int currentmax;

int castle_start[64];
int castle_dest[64];

int piece_value[6] = { 1, 3, 3, 5, 9, 0 };

int pawn_score[64] = {
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   2,   4,  -8,  -8,   5,   2,   0,
	  0,   2,   4,   4,   4,   3,   2,   0,
	  0,   2,   4,   8,   8,   4,   2,   0,
	  0,   2,   4,   8,   8,   4,   2,   0,
	  4,   8,  10,  16,  16,  10,   8,   4,
	  100, 100, 100, 100, 100,100,100,100,
	  800, 800, 800, 800, 800, 800, 800, 800
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
	-10, -10, -14, -10, -10, -14, -10, -10,
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
	 20,  20, -20, -40,  10, -60,  -40,  20,
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
	 10,  20,  30,  35,  35,  30,  20,  10,
	 20,  30,  40,  45,  45,  40,  30,  20,
	 25,  40,  50,  60,  60,  50,  40,  25,
	 25,  40,  50,  60,  60,  50,  40,  25,
	 20,  30,  40,  45,  45,  40,  30,  20,
	 10,  20,  30,  35,  35,  30,  20,  10,
	  0,  10,  20,  25,  25,  20,  10,   0
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
	0, 0,  0,  0, 0,  4, 8, 8,
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

int scale[200];

int KingEndgame[2][64];

void SetBits();
void SetPawnless();

void SetCastle();
void BeforeCastle();

void SetBoard();
void ClearKillers();

void FreeAllHash();

void SetUp()
{
	SetKnightMoves();
	SetKingMoves();
	SetLineMoves();
	SetLinks();
	SetBits();
	SetScores();
	SetKingPawnTable();
	SetCastle();
	SetPassed();
	SetPawnless();
}

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
	ClearKillers();
	FreeAllHash();
	BeforeCastle();
}

void NewPosition()
{
	piece_mat[0] = pawn_mat[0] = table_score[0] = 0;
	piece_mat[1] = pawn_mat[1] = table_score[1] = 0;

	kingside[0] = queenside[0] = 0;
	kingside[1] = queenside[1] = 0;
	kingattack[0] = queenattack[0] = 0;
	kingattack[1] = queenattack[1] = 0;

	bit_pieces[0][P] = bit_pieces[1][P] = 0;
	bit_pieces[0][N] = bit_pieces[1][N] = 0;
	bit_pieces[0][B] = bit_pieces[1][B] = 0;
	bit_pieces[0][R] = bit_pieces[1][R] = 0;
	bit_pieces[0][Q] = bit_pieces[1][Q] = 0;
	bit_pieces[0][K] = bit_pieces[1][K] = 0;
	bit_units[0] = bit_units[1] = 0;
	bit_all = 0;
	
	first_move[0] = 0;

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
		AddPiece(0, P, i);
	}
	for (int i = A7; i <= H7; i++)
	{
		AddPiece(1, P, i);
	}
	for (int i = A3; i <= H6; i++)
	{
		b[i] = EMPTY;
	}
}

void SetCastle()
{
	castle_start[G1] = H1;
	castle_dest[G1] = F1;
	castle_start[C1] = A1;
	castle_dest[C1] = D1;
	castle_start[G8] = H8;
	castle_dest[G8] = F8;
	castle_start[C8] = A8;
	castle_dest[C8] = D8;
	castle = 15;
}

void SetPassed()
{
	int white_score, black_score;
	for (int x = 0; x < 64; x++)
	{
		switch (row[x])
		{
		case 0: white_score = 0; black_score = 800; break;
		case 1: white_score = 10; black_score = 100; break;
		case 2: white_score = 15; black_score = 60; break;
		case 3: white_score = 24; black_score = 40; break;
		case 4: white_score = 40; black_score = 24; break;
		case 5: white_score = 60; black_score = 15; break;
		case 6: white_score = 100; black_score = 10; break;
		case 7: white_score = 800; black_score = 0; break;
		default:
			white_score = 0; black_score = 0;
		}
		passed[0][x] = white_score - PieceScore[0][0][x];
		passed[1][x] = black_score - PieceScore[1][0][x];
	}
}

void SetPawnless()
{
	memset(endmatrix, 0, sizeof(endmatrix));

	endmatrix[0][0][0][0] = DRAWN;//no pieces
	endmatrix[3][0][0][0] = DRAWN;//side has bishop
	endmatrix[0][0][3][0] = DRAWN;//opponent has bishop
	endmatrix[3][1][0][0] = DRAWN;//side has knight
	endmatrix[0][0][3][1] = DRAWN;//opponent has knight

	endmatrix[3][0][3][0] = DRAWN;//B v B
	endmatrix[3][1][3][0] = DRAWN;//N v N
	endmatrix[3][0][3][1] = DRAWN;//B v B
	endmatrix[3][1][3][1] = DRAWN;// N v N

	endmatrix[6][0][0][0] = 9900;//side has 2 bishops
	endmatrix[0][0][6][0] = -9900;//opponent has 2 bishops
	endmatrix[6][1][0][0] = 9900;//side has B + N
	endmatrix[0][0][6][1] = -9900;//opponent has B + N
	endmatrix[6][2][0][0] = DRAWN;//side has 2 knights
	endmatrix[0][0][6][2] = DRAWN;//opponent has 2 knights

	endmatrix[5][0][0][0] = 9900;//side has rook
	endmatrix[0][0][5][0] = -9900;//opponent has rook
	endmatrix[9][0][0][0] = 9900;//side has queen
	endmatrix[0][0][9][0] = -9900;//opponent has queen
}

void SetScores()
{
	for (int x = 0; x < 64; x++)
	{
		PieceScore[0][P][x] = pawn_score[x];
		PieceScore[0][N][x] = knight_score[x] + 10;
		PieceScore[0][B][x] = bishop_score[x] + 10;
		PieceScore[0][R][x] = rook_score[x];
		PieceScore[0][Q][x] = queen_score[x] + 60;
		PieceScore[0][K][x] = 0;
		PieceScore[1][P][x] = pawn_score[Flip[x]];
		PieceScore[1][N][x] = knight_score[Flip[x]] + 10;
		PieceScore[1][B][x] = bishop_score[Flip[x]] + 10;
		PieceScore[1][R][x] = rook_score[Flip[x]];
		PieceScore[1][Q][x] = queen_score[Flip[x]] + 60;
		PieceScore[1][K][x] = 0;

		KingScore[0][x] = king_score[x];
		KingScore[1][x] = king_score[Flip[x]];

		QueenSide2[1][x] = queen_side2[x];
		KingSide2[1][x] = king_side2[x];

		KingEndgame[0][x] = king_endgame_score[x];
		KingEndgame[1][x] = king_endgame_score[x];

		PawnBlocked[0][x] = pawn_blocked[x];
		PawnBlocked[1][x] = pawn_blocked[Flip[x]];
	}
	for (int x = 0; x < 100; x++)
		centi_pieces[x] = x * 100;
}

void SetKingPawnTable()
{
	memset(KingSide, 0, sizeof(KingSide));
	memset(QueenSide, 0, sizeof(QueenSide));
	QueenSide[0][A2] = KingSide[0][H2] = QueenSide[1][A7] = KingSide[1][H7] = 25;
	QueenSide[0][B2] = KingSide[0][G2] = QueenSide[1][B7] = KingSide[1][G7] = 25;
	QueenSide[0][C2] = KingSide[0][F2] = QueenSide[1][C7] = KingSide[1][F7] = 15;

	QueenSide[0][A3] = KingSide[0][H3] = QueenSide[1][A6] = KingSide[1][H6] = 25;
	QueenSide[0][B3] = KingSide[0][G3] = QueenSide[1][B6] = KingSide[1][G6] = 15;
	QueenSide[0][C3] = KingSide[0][F3] = QueenSide[1][C6] = KingSide[1][F6] = 10;

	QueenSide[0][A4] = KingSide[0][H4] = QueenSide[1][A5] = KingSide[1][H5] = 5;
	QueenSide[0][B4] = KingSide[0][G4] = QueenSide[1][B5] = KingSide[1][G5] = 5;
	QueenSide[0][C4] = KingSide[0][F4] = QueenSide[1][C5] = KingSide[1][F5] = 5;

	QueenSide[0][A5] = KingSide[0][H5] = QueenSide[1][A4] = KingSide[1][H4] = 1;
	QueenSide[0][B5] = KingSide[0][G5] = QueenSide[1][B4] = KingSide[1][G4] = 1;
	QueenSide[0][C5] = KingSide[0][F5] = QueenSide[1][C4] = KingSide[1][F4] = 1;

	memset(KingSide2, 0, sizeof(KingSide2));
	memset(QueenSide2, 0, sizeof(QueenSide2));
	QueenSide2[0][A2] = KingSide2[0][H2] = QueenSide2[1][A7] = KingSide2[1][H7] = 10;
	QueenSide2[0][B2] = KingSide2[0][G2] = QueenSide2[1][B7] = KingSide2[1][G7] = 10;
	QueenSide2[0][C2] = KingSide2[0][F2] = QueenSide2[1][C7] = KingSide2[1][F7] = 6;

	QueenSide2[0][A3] = KingSide2[0][H3] = QueenSide2[1][A6] = KingSide2[1][H6] = 10;
	QueenSide2[0][B3] = KingSide2[0][G3] = QueenSide2[1][B6] = KingSide2[1][G6] = 10;
	QueenSide2[0][C3] = KingSide2[0][F3] = QueenSide2[1][C6] = KingSide2[1][F6] = 6;

	QueenSide2[0][A4] = KingSide2[0][H4] = QueenSide2[1][A5] = KingSide2[1][H5] = 10;
	QueenSide2[0][B4] = KingSide2[0][G4] = QueenSide2[1][B5] = KingSide2[1][G5] = 10;
	QueenSide2[0][C4] = KingSide2[0][F4] = QueenSide2[1][C5] = KingSide2[1][F5] = 6;

	QueenSide2[0][A5] = KingSide2[0][H5] = QueenSide2[1][A4] = KingSide2[1][H4] = 5;
	QueenSide2[0][B5] = KingSide2[0][G5] = QueenSide2[1][B4] = KingSide2[1][G4] = 5;
	QueenSide2[0][C5] = KingSide2[0][F5] = QueenSide2[1][C4] = KingSide2[1][F4] = 2;

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
	}
}

