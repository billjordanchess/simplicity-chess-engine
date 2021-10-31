
#include <memory.h>

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <dos.h>
#include <time.h>
#include <assert.h>

extern unsigned int CAPTURE;
extern unsigned int INCHECK;

extern unsigned int ATTACK;
extern unsigned int CASTLE;
extern unsigned int QUIET;
extern unsigned int KILLER;
extern unsigned int PASSED6;
extern unsigned int PASSED7;
extern unsigned int PROMOTE;
extern unsigned int CHECK;
extern unsigned int MATETHREAT;
extern unsigned int DEFEND;
extern unsigned int PAWNMOVE;
extern unsigned int COUNTER;
extern unsigned int EP;

#define A1	0
#define B1	1
#define C1	2
#define D1	3
#define E1	4
#define F1  5
#define G1	6
#define H1	7

#define A2	8
#define B2	9
#define C2	10
#define D2	11
#define E2	12
#define F2	13
#define G2	14
#define H2	15

#define A3	16
#define B3	17
#define C3	18
#define D3	19
#define E3	20
#define F3	21
#define G3	22
#define H3	23

#define A4	24
#define B4	25
#define C4	26
#define D4	27
#define E4	28
#define F4	29
#define G4	30
#define H4	31

#define A5	32
#define B5	33
#define C5	34
#define D5	35
#define E5	36
#define F5	37
#define G5	38
#define H5	39

#define A6	40
#define B6	41
#define C6	42
#define D6	43
#define E6	44
#define F6	45
#define G6	46
#define H6	47

#define A7	48
#define B7	49
#define C7	50
#define D7	51
#define E7	52
#define F7	53
#define G7	54
#define H7	55

#define A8	56
#define B8	57
#define C8	58
#define D8	59
#define E8	60
#define F8	61
#define G8	62
#define H8	63

#define DEBUG 1
#define NULLMOVE 1

#define BITBOARD unsigned __int64

#define BISHOP_PAIR 25

#define KINGSIDE 1
#define QUEENSIDE 2
#define KINGOPPOSITE 3
#define QUEENOPPOSITE 4

#define PAWNLESS 1
#define PAWN_ENDING 2
#define QUEEN_ENDING 3
#define ONE_PAWN 3
#define OPPOSITE 5
#define NORMAL 6

#define HASH_SCORE    25000000
#define HASH_SCORE2   22222222
#define CAPTURE_SCORE 10000000
#define KILLER_LIMIT  9000000
#define KILLER_SCORE  8888888
#define HISTORY_LIMIT 8000000
#define COUNTER_SCORE 7999000
#define PV_SCORE 100000000

const int UPPER = 3;
const int LOWER = 1;
const int EXACT = 2;
const int ALPHA = 3;
const int BETA = 1;

#define GEN_STACK		2400//2000//1120 16/11/12
#define MAX_PLY			48//10/4/14 was 40
#define HIST_STACK		400

#define LIGHT			0
#define DARK			1

#define PAWN			0
#define KNIGHT			1
#define BISHOP			2
#define ROOK			3
#define QUEEN			4 
#define KING			5

#define P 0
#define N 1
#define B 2
#define R 3
#define Q 4
#define K 5

#define EMPTY 6

#define NORTH 0
#define NE 1
#define EAST 2
#define SE 3
#define SOUTH 4
#define SW 5
#define WEST 6
#define NW 7

#define DRAWN 77

extern int fixed_time;
extern int fixed_depth;

extern int list[2][8];
extern int pawnlist[2][8];
extern int pawns[2];
extern int table_score[2];
extern int kingside[2];
extern int queenside[2];
extern int kingattack[2];
extern int queenattack[2];
extern int KingSide[2][64];
extern int QueenSide[2][64];
extern int KingSide2[2][64];
extern int QueenSide2[2][64];
extern int king_side2[64];
extern int queen_side2[64];

typedef struct
{
	int from;
	int to;
	int score;
	unsigned int flags;
} move_data;

typedef struct
{
	int from;
	int to;
	unsigned int flags;
	int capture;
	int castle;
	int fifty;
	BITBOARD hash;
	BITBOARD lock;
} game;

extern move_data move_list[GEN_STACK];

/* Gen.cpp */
void Gen(const int);
void GenPromote(const int from, const int to);
bool MakeMove(const int from, const int to, const int flags);
void UnMakeMove();

/* search.cpp */
void think();
int search(int alpha, int beta, int depth, int pvflag, int nullflag);
int quiesce(int alpha, int beta, int depth);
int reps();
int reps2();

/* eval.cpp */
int eval(const int alpha, const int beta);

/* main.cpp */
int GetTime();
int main();
int ParseMove(char* s);
char* MoveString(int start, int dest, int promote);
void print_board();
void xboard();

extern int adjfile[64][64];
extern int kingqueen[64][64];
extern int kingknight[64][64];
extern int kingking[64][64];

extern BITBOARD bishop_a7[2];
extern BITBOARD bishop_h7[2];
extern BITBOARD knight_a7[2];
extern BITBOARD knight_h7[2];

extern BITBOARD bit_adjacent[64];

extern BITBOARD passed_list[2];

extern int lostflag;

extern int side;
extern int xside;
extern int castle;
extern int fifty;
extern int ply;
extern int hply;
extern move_data move_list[GEN_STACK];
extern int first_move[MAX_PLY];

extern game game_list[HIST_STACK];
extern int max_time;
extern int max_depth;
extern int start_time;
extern int stop_time;
extern int nodes;
extern int qnodes;
extern int cut_nodes;
extern int first_nodes;
extern int cut_nodes;
extern int av_nodes;
extern int a_nodes;
extern int b_nodes;

extern int hash_piece[2][6][64];
extern int hash_side;
extern int hash_ep[64];
extern int castle_mask[64];
extern char piece_char[6];
extern int startmat[2];

extern BITBOARD currentkey, currentlock;
extern BITBOARD currentpawnkey, currentpawnlock;

//bitboard.cpp
extern BITBOARD mask_squarepawn[2][64];
extern BITBOARD mask_squareking[2][64];
extern BITBOARD mask_queenside;
extern BITBOARD mask_edge;
extern BITBOARD mask_corner;

extern int difference[64][64];
extern int pawn_difference[64][64];

extern BITBOARD mask_squarepawn[2][64];
extern BITBOARD bit_between[64][64];

//legal moves from each square
extern BITBOARD bit_pawncaptures[2][64];
extern BITBOARD bit_pawndefends[2][64];
extern BITBOARD bit_left[2][64];
extern BITBOARD bit_right[2][64];
extern BITBOARD bit_knightmoves[64];
extern BITBOARD bit_bishopmoves[64];
extern BITBOARD bit_rookmoves[64];
extern BITBOARD bit_queenmoves[64];
extern BITBOARD bit_kingmoves[64];
//current position

extern BITBOARD bit_pieces[2][7];
extern BITBOARD bit_units[2];
extern BITBOARD bit_all;

extern BITBOARD bit_color[2];
extern BITBOARD mask_centre;

//current Attacks
extern BITBOARD bit_leftcaptures[2];
extern BITBOARD bit_rightcaptures[2];
extern BITBOARD bit_pawnattacks[2];
extern BITBOARD bit_kingattacks[2];
extern BITBOARD bit_attacks[2];

extern BITBOARD mask_passed[2][64];
extern BITBOARD mask_path[2][64];
extern BITBOARD mask_backward[2][64];
extern BITBOARD mask_ranks[2][8];
extern BITBOARD mask_files[8];
extern BITBOARD mask_rookfiles;
extern BITBOARD mask_cols[64];

extern BITBOARD mask_rows[8];
extern BITBOARD mask[64];
extern BITBOARD mask_isolated[64];
extern BITBOARD mask_left_col[64];
extern BITBOARD mask_right_col[64];
extern BITBOARD mask_adjacent[64];
extern BITBOARD mask_nwdiag[64];
extern BITBOARD mask_nediag[64];

extern BITBOARD bit_colors;

extern BITBOARD mask_abc;
extern BITBOARD mask_def;

extern BITBOARD mask_kingpawns[2];
extern BITBOARD mask_queenpawns[2];

extern BITBOARD mask_moves[64][56];

extern BITBOARD not_mask[64];
extern BITBOARD not_mask_rookfiles;
extern BITBOARD not_mask_edge;
extern BITBOARD not_mask_corner;
extern BITBOARD not_mask_files[8];
extern BITBOARD not_mask_rows[8];

extern BITBOARD not_a_file;
extern BITBOARD not_h_file;

#define BITBOARD unsigned __int64

extern int kmoves[64][8];
extern int knightmoves[64][8];
extern int kingmoves[64][8];

extern int piece_mat[2];
extern int pawn_mat[2];
extern int start_piece_mat[2];
extern int start_pawn_mat[2];

extern int captures[MAX_PLY];
extern int extend[MAX_PLY];
extern int Threat[MAX_PLY];
extern int InCheck[MAX_PLY];

extern int color[64];
extern int b[64];

extern int ply;
extern int currentmax;
extern int threatdepth;

extern int drawn;
extern int Num[2];
extern int List[2][12];

extern int KingPawn[2][64];
extern int pawn_score[64];
extern int knight_score[64];
extern int bishop_score[64];
extern int rook_score[64];
extern int queen_score[64];
extern int king_score[64];
extern int king_endgame_score[64];
extern int Flip[64];

//global variables
extern const int colors[64];

extern const int col[64];
extern const int row[64];
extern const int nwdiag[64];
extern const int nediag[64];
extern const int board[64];

extern int rank[2][64];
extern int lastsquare[2][64];

extern int turn;

extern int Alpha, Beta;

extern int PieceScore[2][6][64];
extern int KingScore[2][64];

extern int KingPawnLess[64];
extern int KingEndgame[2][64];

extern int pieces[2][6][10];
extern int total[2][6];

extern int table_score[2];

extern int piece_value[6];
extern int done[1000];

extern int passed[2][64];
extern int adjacent_passed[2][64];
extern int defended_passed[2][64];

extern int isolated[64];
extern int pawn_blocked[64];

extern int PawnBlocked[2][64];

extern int pawnleft[2][64];
extern int pawnright[2][64];
extern int pawnplus[2][64];
extern int pawndouble[2][64];

extern int difference[64][64];
extern int squares[2][64];
extern int start[64];
extern int end[64];

extern int castle_start[64];
extern int castle_dest[64];

extern int hash_start;
extern int hash_dest;
extern int hash_score;
extern int hash_flags;

extern int knight_total[64];
extern int king_total[64];
extern int queen_total[64];

extern int history[64][64];
extern int check_history[6][64];

//functions
void PrintBitBoard(BITBOARD bb);
int NextBit(BITBOARD bb);

//hash.cpp
void RandomizeHash();
void FreeAllHash();
BITBOARD GetKey();
BITBOARD GetLock();
BITBOARD GetPawnKey();
BITBOARD GetPawnLock();
void AddKey(const int, const int, const int);
void AddKeys(const int s, const int p, const int x, const int y);
void AddHash(const int, const int, const int, const int, const int, const int);

void AddPawnHash(const int s1, const int s2, const BITBOARD, const BITBOARD);
int GetHashPawn0();
int GetHashPawn1();
bool LookUpPawn();
void AddPawnKey(const int s, const int x);
void AddPawnKeys(const int s, const int x, const int y);
void AddKingHash(const int s, const int value);
void AddQueenHash(const int s, const int value);

int GetHashQueenside(const int);
int GetHashKingside(const int);

int GetHashQueenside0();
int GetHashQueenside1();
int GetHashKingside0();
int GetHashKingside1();

void SetRanks();
void SetBits();
void SetScores();
void SetPassed();
void SetKingPawnTable();

void Alg(int a, int b);
void Algebraic(int a);
void Alg1(int a);
void Alg2(int a, int b);

int Debug(const int);

int NextBit(BITBOARD x);

void Alg(int a, int b);
void Algebraic(int a);

void ShowAll2();
void ShowAll(int);

void SetPassed();

//bitboard.cpp
int CountBits(register BITBOARD a);

void SetBoardBits();
void CaptureBits(int x);

int FirstOne(BITBOARD arg1);

int PassedPawnScore(const int s, const int xs);

void z();

int BitsGreaterThanOne(BITBOARD x);

int EvalPawns(const int s, const int);

BITBOARD GetHashPassed(const int s);
BITBOARD GetHashPassed0();
BITBOARD GetHashPassed1();

BITBOARD GetHashPawnAttacks(const int s);
void AddPawnAttackHash(const int s, const BITBOARD value);

void GenCapsChecks(const int, const int);
void GenCaps(const int);
int gen_recaptures(const int alpha, const int);

bool Attack(const int s, const int sq);
bool CheckAttack(const int s, const int sq);
bool LineAttack(const int s, const int sq);
bool LineAttack1(const int s, const int sq, const int pinned);
bool LineAttack2(const int s, const int sq, const int pinned, const int);

void UpdatePawn(const int s, const int start, const int dest);
void UpdatePiece(const int s, const int p, const int start, const int dest);
void RemovePiece(const int s, const int p, const int sq);
void AddPiece(const int s, const int p, const int sq);

void NewPosition();

void SetKingPawnTable();

int LookUp2(const int s);
int BestCapture(const int, const int, BITBOARD);
int BestCaptureSquare(const int s, const int xs, const int sq, const int p);
int BestCapture2(const int s, const int xs, BITBOARD bu);

int GetThreatMove(const int s, const int xs, int&, int&, const int, const int);
int MakeThreat(const int s, const int sx, const int threat_start, const int threat_dest);

int Blunder(const int, const int);
int BlunderCheck(const int, const int);
int BlunderThreat(const int, const int, const int, const int, const int, const int);
int BlunderCapture(const int cv, const int to, const int flags);

extern int PlyMove[MAX_PLY];
extern int PlyType[MAX_PLY];

int LookUp(const int side, const int depth, const int alpha, const int beta);

int MakeCapture(const int, const int, const int);
void UnMakeCapture();

void MakeRecapture(const int, const int);
void UnMakeRecapture();

int BlockedPawns(const int s, const int x);
int SafeKingMoves(const int, const int);

void MoveAttacked(const int xs,const int sq, const int, const int ply);

int BlunderCapture();

void PopSwap();
int GetScore2(const int s, const int xs, const int n);
int Score(const int s, const int xs);
int GenKey(const int s, const int xs, const int n);

void Evasion(const int);

BITBOARD GetHashbp0();
BITBOARD GetHashbp1();
void AddHashbp0(const BITBOARD bp);
void AddHashbp1(const BITBOARD bp);

extern int behind_queen;
extern int king_defends;
extern int Swap[6][100][100][3][3][2][2];
extern int List[2][12];

extern int scale[200];
extern int h_check[64][64];
extern int v_check[64][64];
extern int l_check[64][64];
extern int r_check[64][64];
extern int q_check[64][64][13];

extern int stats_depth[20];
extern int stats_count[100];
extern int stats_killers[2];
extern int total_killers[2];

extern int null_depth[48];

int BestThreat(const int s, const int xs, const int diff);

extern int endmatrix[10][3][10][3];
extern int kingloc[64];

int GetHashDefence(const int s, const int n);

int Disco(const int s, const int sq);

int BlunderCapture(const int to, const int cv, const int flags);

void SetNullDepth();

void SetUp();

extern BITBOARD ep_hash[64];

bool IsCheck(const int s, const int p, const int to, const int sq);

void ShowAllEval(int ply);

int EvalPawnless();

int Check(const int s, const int sq);

extern int centi_pawns[9];
extern int centi_pieces[104];

#define PVAL 1
#define BVAL 300
#define RVAL 500
#define QVAL 900
#define BBVAL 600

extern int p_value[6];















