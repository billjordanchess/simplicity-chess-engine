#include "globals.h"

#define N0 0
#define N1 1
#define N2 2
#define N3 3
#define N4 4
#define N5 5
#define EAST0 6
#define EAST1 7
#define EAST2 8
#define EAST3 9
#define EAST4 10
#define EAST5 11
#define S0 12
#define S1 13
#define S2 14
#define S3 15
#define S4 16
#define S5 17
#define W0 18
#define W1 19
#define W2 20
#define W3 21
#define W4 22
#define W5 23
#define NE0 24
#define NE1 25
#define NE2 26
#define NE3 27
#define NE4 28
#define NE5 29
#define SE0 30
#define SE1 31
#define SE2 32
#define SE3 33
#define SE4 34
#define SE5 35
#define SW0 36
#define SW1 37
#define SW2 38
#define SW3 39
#define SW4 40
#define SW5 41
#define NW0 42
#define NW1 43
#define NW2 44
#define NW3 45
#define NW4 46
#define NW5 47
#define TOP 48
#define BOTTOM 49
#define LEFT 50
#define RIGHT 51
#define TOP_LEFT 52
#define TOP_RIGHT 53
#define BOTTOM_LEFT 54
#define BOTTOM_RIGHT 55

int RookMoveCount(const int x);
int BishopMoveCount(const int x, const BITBOARD bp);

void SetBit(BITBOARD& bb, int square);
void SetBitFalse(BITBOARD& bb, int square);

void SetReductions();

void RookMoves(const int sq);
void BishopMoves(const int sq);
void QueenMoves(const int sq);

void PushBishop(const int from, const int to);
void PushRook(const int from, const int to);
void PushQueen(const int from, const int to);
void CapBishop(const int from, const int to);
void CapRook(const int from, const int to);
void CapQueen(const int from, const int to);

typedef struct
{
    int first;
    int last;
    int next;
} link;

typedef struct
{
    int sq;
    int next;
}list1;

list1 queenlist[64][28];
list1 rooklist[64][15];
list1 bishoplist[64][14];

int kingqueen[64][64];
int kingknight[64][64];
int taxi[] = { 0,16,12,8,4,0,0,0 };

extern int mailbox[120];
extern int mailbox64[64];
extern bool slide[6];
extern int offsets[6];
extern int offset[6][8];

int NextBit2(BITBOARD bb);

int NextBitWest(BITBOARD bb);

const int col[64] =
{
0,1,2,3,4,5,6,7,
0,1,2,3,4,5,6,7,
0,1,2,3,4,5,6,7,
0,1,2,3,4,5,6,7,
0,1,2,3,4,5,6,7,
0,1,2,3,4,5,6,7,
0,1,2,3,4,5,6,7,
0,1,2,3,4,5,6,7
};
const int row[64] =
{
0,0,0,0,0,0,0,0,
1,1,1,1,1,1,1,1,
2,2,2,2,2,2,2,2,
3,3,3,3,3,3,3,3,
4,4,4,4,4,4,4,4,
5,5,5,5,5,5,5,5,
6,6,6,6,6,6,6,6,
7,7,7,7,7,7,7,7
};

const BITBOARD debruijn64 = 0x07EDD5E59A4E28C2;

const int index64[64] = {
   63,  0, 58,  1, 59, 47, 53,  2,
   60, 39, 48, 27, 54, 33, 42,  3,
   61, 51, 37, 40, 49, 18, 28, 20,
   55, 30, 34, 11, 43, 14, 22,  4,
   62, 57, 46, 52, 38, 26, 32, 41,
   50, 36, 17, 19, 29, 10, 13, 21,
   56, 45, 25, 31, 35, 16,  9, 12,
   44, 24, 15,  8, 23,  7,  6,  5
};

const int index642[64] = {
    0, 47,  1, 56, 48, 27,  2, 60,
   57, 49, 41, 37, 28, 16,  3, 61,
   54, 58, 35, 52, 50, 42, 21, 44,
   38, 32, 29, 23, 17, 11,  4, 62,
   46, 55, 26, 59, 40, 36, 15, 53,
   34, 51, 20, 43, 31, 22, 10, 45,
   25, 39, 14, 33, 19, 30,  9, 24,
   13, 18,  8, 12,  7,  6,  5, 63
};

//matts
const int lsb_64_table[64] =
{
   63, 30,  3, 32, 59, 14, 11, 33,
   60, 24, 50,  9, 55, 19, 21, 34,
   61, 29,  2, 53, 51, 23, 41, 18,
   56, 28,  1, 43, 46, 27,  0, 35,
   62, 31, 58,  4,  5, 49, 54,  6,
   15, 52, 12, 40,  7, 42, 45, 16,
   25, 57, 48, 13, 10, 39,  8, 44,
   20, 47, 38, 22, 17, 37, 36, 26
};

const int nwdiag[64] = {
     14,13,12,11,10, 9, 8, 7,
     13,12,11,10, 9, 8, 7, 6,
     12,11,10, 9, 8, 7, 6, 5,
     11,10, 9, 8, 7, 6, 5, 4,
     10, 9, 8, 7, 6, 5, 4, 3,
      9, 8, 7, 6, 5, 4, 3, 2,
      8, 7, 6, 5, 4, 3, 2, 1,
      7, 6, 5, 4, 3, 2, 1, 0 };

const int colors[64] = {
     1,0,1,0,1,0,1,0,
     0,1,0,1,0,1,0,1,
     1,0,1,0,1,0,1,0,
     0,1,0,1,0,1,0,1,
     1,0,1,0,1,0,1,0,
     0,1,0,1,0,1,0,1,
     1,0,1,0,1,0,1,0,
     0,1,0,1,0,1,0,1 };

const int nediag[64] = {
     7, 8,9,10,11,12,13,14,
     6, 7,8, 9,10,11,12,13,
     5, 6,7, 8, 9,10,11,12,
     4, 5,6, 7, 8, 9,10,11,
     3, 4,5, 6, 7, 8, 9,10,
     2, 3,4, 5, 6, 7, 8, 9,
     1, 2,3, 4, 5, 6, 7, 8,
     0, 1,2, 3, 4, 5, 6, 7 };

int h_check[64][64];
int v_check[64][64];
int l_check[64][64];
int r_check[64][64];
int q_check[64][64][13];

int adjfile[64][64];

int diag[64][64];

void SetRanks();
void SetKingMoves();

void SetDir();
void SetBetweenVector();
void SetRowCol();

void SetMaskDiags();
int GetEdge(int sq, int plus);

BITBOARD bishop_a7[2];
BITBOARD bishop_h7[2];
BITBOARD knight_a7[2];
BITBOARD knight_h7[2];

BITBOARD passed_list[2];
BITBOARD mask_left_col[64];
BITBOARD mask_right_col[64];

BITBOARD bit_top[2];
BITBOARD bit_adjacent[64];

BITBOARD mask_zone;
BITBOARD mask_centre;
BITBOARD mask_d3e4[2];
BITBOARD mask_d5e4[2];
BITBOARD mask_short_zone[2][64];
BITBOARD mask_long_zone[2][64];
BITBOARD mask_short_3[2][64];
BITBOARD mask_long_3[2][64];
BITBOARD mask_bishop3[2];

BITBOARD mask_moves[64][56];
BITBOARD mask_nwdiag[64];
BITBOARD mask_nediag[64];

BITBOARD bit_pinned;
BITBOARD bit_partial;
BITBOARD bit_captured;

BITBOARD mask_abc2;
BITBOARD mask_abc3;
BITBOARD mask_abc4;
BITBOARD mask_fgh2;
BITBOARD mask_fgh3;
BITBOARD mask_fgh4;

BITBOARD bit_between[64][64];

//legal moves from each square
BITBOARD bit_pawncaptures[2][64];
BITBOARD bit_pawndefends[2][64];
BITBOARD bit_left[2][64];
BITBOARD bit_right[2][64];
BITBOARD bit_pawnmoves[2][64];
BITBOARD bit_knightmoves[64];
BITBOARD bit_bishopmoves[64];
BITBOARD bit_rookmoves[64];
BITBOARD bit_queenmoves[64];
BITBOARD bit_kingmoves[64];
BITBOARD bit_rook_one[64];
BITBOARD bit_bishop_one[64];

//current position
BITBOARD bit_pieces[2][7];
BITBOARD bit_units[2];//pieces+pawns
BITBOARD bit_color[2];
BITBOARD bit_all;

//current attacks
BITBOARD bit_leftcaptures[2];
BITBOARD bit_rightcaptures[2];
BITBOARD bit_pawnattacks[2];
BITBOARD bit_knightattacks[2];
BITBOARD bit_bishopattacks[2];
BITBOARD bit_rookattacks[2];
BITBOARD bit_queenattacks[2];
BITBOARD bit_kingattacks[2];
BITBOARD bit_attacks[2];

BITBOARD bit_qnattack[2];
BITBOARD bit_knattack[2];
BITBOARD bit_qrattack[2];
BITBOARD bit_krattack[2];

BITBOARD bit_colors;

BITBOARD vectorbits[64][64];
BITBOARD mask_vectors[64][8];
int vector[64][8];
int start[64];
int end[64];

BITBOARD mask_passed[2][64];
BITBOARD mask_path[2][64];
BITBOARD mask_backward[2][64];
BITBOARD mask_ranks[2][8];

BITBOARD mask_files[8];
BITBOARD mask_rows[8];

BITBOARD mask_cols[64];

BITBOARD mask[64];
BITBOARD mask_isolated[64];
BITBOARD mask_adjacent[64];
BITBOARD mask_squarepawn[2][64];
BITBOARD mask_squareking[2][64];
BITBOARD mask_kingpawns[2];
BITBOARD mask_queenpawns[2];

BITBOARD mask_rookfiles;
BITBOARD mask_edge;
BITBOARD mask_corner;

BITBOARD not_mask_rookfiles;
BITBOARD not_mask_edge;
BITBOARD not_mask_corner;
BITBOARD not_mask_files[8];
BITBOARD not_mask_rows[8];

BITBOARD mask_abc;
BITBOARD mask_def;

BITBOARD mask_queenside;

BITBOARD not_a_file;
BITBOARD not_h_file;

BITBOARD not_mask[64];

BITBOARD mask_2_squares[64];

int PopCount(unsigned long bb);

void SetBit(BITBOARD& bb, int square);
void SetBitFalse(BITBOARD& bb, int square);
void PrintBitBoard(BITBOARD bb);
void PrintCell(int x, BITBOARD bb);

//BITBOARD GetNextBit(BITBOARD bb);

void SetBit(BITBOARD& bb, int square)
{
    bb |= (1ui64 << square);
}

void SetBitFalse(BITBOARD& bb, int square)
{
    bb &= ~mask[square];
}

//x & ~(x-1)
//c & -c or c & (~c + 1) from bitwise tricks

void PrintBitBoard(BITBOARD bb)
{
    printf("\n");
    int x;
    for (x = 56; x < 64; x++)
        PrintCell(x, bb);
    for (x = 48; x < 56; x++)
        PrintCell(x, bb);
    for (x = 40; x < 48; x++)
        PrintCell(x, bb);
    for (x = 32; x < 40; x++)
        PrintCell(x, bb);
    for (x = 24; x < 32; x++)
        PrintCell(x, bb);
    for (x = 16; x < 24; x++)
        PrintCell(x, bb);
    for (x = 8; x < 16; x++)
        PrintCell(x, bb);
    for (x = 0; x < 8; x++)
        PrintCell(x, bb);
}

void PrintCell(int x, BITBOARD bb)
{
    if (mask[x] & bb)
        printf(" X");
    else
        printf(" -");
    if ((x + 1) % 8 == 0)printf("\n");
}

int PopCount(unsigned long bb)
{
    int count = 0;
    while (bb)
    {
        ++count;

    }
    return count;
}

int PopCnt(register BITBOARD a)
{
    register int c = 0;

    while (a) {
        c++;
        a &= a - 1;
    }
    return(c);
}

void SetBits()
{
    int x, y;

    for (x = 0; x < 64; x++)
    {
        SetBit(mask[x], x);
    }
    SetRanks();
    SetRowCol();

    for (x = 0; x < 64; x++)
    {
        if (row[x] > 1 && row[x] < 6 && col[x]>1 && col[x] < 6)
            SetBit(mask_zone, x);
    }

    for (x = 0; x < 64; x++)
    {
        if (row[x] > 3)
            SetBit(bit_top[0], x);
        else
            SetBit(bit_top[1], x);

        if (col[x] > 0)
            SetBit(bit_adjacent[x], x - 1);
        if (col[x] < 7)
            SetBit(bit_adjacent[x], x + 1);
    }

    SetBit(mask_centre, D4);
    SetBit(mask_centre, E4);
    SetBit(mask_centre, D5);
    SetBit(mask_centre, E4);

    bit_colors = 0;
    for (x = 0; x < 64; x++)
    {
        if (colors[x] == 1)
            SetBit(bit_colors, x);
    }

    for (x = 0; x < 64; x++)
    {
        if (colors[x] == 0)
            SetBit(bit_color[0], x);
        if (colors[x] == 1)
            SetBit(bit_color[1], x);
    }
    memset(bit_pawncaptures, 0, sizeof(bit_pawncaptures));
    memset(bit_pawnmoves, 0, sizeof(bit_pawnmoves));
    memset(bit_knightmoves, 0, sizeof(bit_knightmoves));
    memset(bit_bishopmoves, 0, sizeof(bit_bishopmoves));
    memset(bit_rookmoves, 0, sizeof(bit_rookmoves));
    memset(bit_queenmoves, 0, sizeof(bit_queenmoves));
    memset(bit_kingmoves, 0, sizeof(bit_kingmoves));

    //current attacks
    memset(bit_pawnattacks, 0, sizeof(bit_pawnattacks));
    memset(bit_knightattacks, 0, sizeof(bit_knightattacks));
    memset(bit_bishopattacks, 0, sizeof(bit_bishopattacks));
    memset(bit_rookattacks, 0, sizeof(bit_rookattacks));
    memset(bit_queenattacks, 0, sizeof(bit_queenattacks));
    memset(bit_kingattacks, 0, sizeof(bit_kingattacks));

    int i, j, n, p;
    for (i = 0; i < 64; i++)
    {
        for (p = KNIGHT; p <= KING; p++)
        {
            for (j = 0; j < offsets[p]; ++j)
                for (n = i;;)
                {
                    n = mailbox[mailbox64[n] + offset[p][j]];
                    if (n == -1)
                        break;
                    if (n > -1)
                    {
                        switch (p)
                        {
                        case KNIGHT:
                            SetBit(bit_knightmoves[i], n);
                            break;
                        case BISHOP:
                            SetBit(bit_bishopmoves[i], n);
                            break;
                        case ROOK:
                            SetBit(bit_rookmoves[i], n);
                            break;
                        case QUEEN:
                            SetBit(bit_queenmoves[i], n);
                            break;
                        case KING:
                            SetBit(bit_kingmoves[i], n);
                            break;
                        }
                    }
                    if (!slide[p])
                        break;
                }
        }
    }

    for (x = 0; x < 64; x++)
    {
        bit_rook_one[x] = bit_kingmoves[x] & bit_rookmoves[x];
        bit_bishop_one[x] = bit_kingmoves[x] & bit_bishopmoves[x];
    }

    for (x = 0; x < 64; x++)
    {
        pawnleft[0][x] = -1;//16/5/12
        pawnleft[1][x] = -1;
        pawnright[0][x] = -1;
        pawnright[1][x] = -1;
        if (col[x] > 0)
        {
            if (row[x] < 7) { pawnleft[0][x] = x + 7; }
            if (row[x] > 0) { pawnleft[1][x] = x - 9; }
        }
        if (col[x] < 7)
        {
            if (row[x] < 7) { pawnright[0][x] = x + 9; }
            if (row[x] > 0) { pawnright[1][x] = x - 7; }
        }
    }

    for (x = 0; x < 64; x++)
    {
        if (col[x] > 0)
        {
            if (row[x] < 7)
            {
                SetBit(bit_pawncaptures[0][x], pawnleft[0][x]);
                SetBit(bit_left[0][x], pawnleft[0][x]);
            }
            if (row[x] > 0)
            {
                SetBit(bit_pawncaptures[1][x], pawnleft[1][x]);
                SetBit(bit_left[1][x], pawnleft[1][x]);
            }
        }
        if (col[x] < 7)
        {
            if (row[x] < 7)
            {
                SetBit(bit_pawncaptures[0][x], pawnright[0][x]);
                SetBit(bit_right[0][x], pawnright[0][x]);
            }
            if (row[x] > 0)
            {
                SetBit(bit_pawncaptures[1][x], pawnright[1][x]);
                SetBit(bit_right[1][x], pawnright[1][x]);
            }
        }
    }
    for (x = 0; x < 64; x++)
    {
        bit_pawndefends[0][x] = bit_pawncaptures[1][x];
        bit_pawndefends[1][x] = bit_pawncaptures[0][x];
    }
    //bit_pawndouble could be added
    for (x = 0; x < 64; x++)
    {
        if (row[x] < 7)
        {
            pawnplus[0][x] = x + 8;
            SetBit(bit_pawnmoves[0][x], x + 8);
        }
        if (row[x] < 6)
        {
            pawndouble[0][x] = x + 16;
            //SetBit(bit_pawnmoves[0][x],x+16);
        }
        if (row[x] > 0)
        {
            pawnplus[1][x] = x - 8;
            SetBit(bit_pawnmoves[1][x], x - 8);
        }
        if (row[x] > 1)
        {
            pawndouble[1][x] = x - 16;
            //SetBit(bit_pawnmoves[1][x],x-16);
        }
    }

    for (y = 0; y < 8; y++)
        for (x = 0; x < 64; x++)
        {
            if (col[x] == y)
                SetBit(mask_files[y], x);
            if (row[x] == y)
                SetBit(mask_rows[y], x);
            if (row[x] == y)
            {
                SetBit(mask_ranks[0][y], x);
            }
            if (row[x] == 7 - y)
            {
                SetBit(mask_ranks[1][y], x);      //?
            }
        }

    for (x = 0; x < 64; x++)
        for (y = 0; y < 64; y++)
        {
            if (col[x] == col[y])
                SetBit(mask_cols[x], y);
        }

    for (x = 0; x < 64; x++)
    {
        not_mask[x] = ~mask[x];
    }

    for (x = 0; x < 8; x++)
    {
        not_mask_files[x] = ~mask_files[x];
    }
    not_a_file = ~mask_files[0];
    not_h_file = ~mask_files[7];

    mask_rookfiles = mask_files[0] | mask_files[7];

    mask_bishop3[0] = mask_rows[rank[0][A3]] & mask_zone;
    mask_bishop3[1] = mask_rows[rank[1][A3]] & mask_zone;

    SetBetweenVector();
    SetKingMoves();

    SetDir();

    SetMaskDiags();

    for (x = 0; x < 64; x++)
    {
        if (row[x] == 0 || row[x] == 7 || col[x] == 0 || col[x] == 7)
            SetBit(mask_edge, x);
    }
    for (x = 0; x < 64; x++)
    {
        if (col[x] < 3)
            SetBit(mask_abc, x);
        if (col[x] > 4)
            SetBit(mask_def, x);
    }

    SetBit(mask_queenpawns[0], A2);
    SetBit(mask_queenpawns[0], A3);
    SetBit(mask_queenpawns[0], A4);
    SetBit(mask_queenpawns[0], A5);
    SetBit(mask_queenpawns[0], B2);
    SetBit(mask_queenpawns[0], B3);
    SetBit(mask_queenpawns[0], B4);
    SetBit(mask_queenpawns[0], B5);
    SetBit(mask_queenpawns[0], C2);
    SetBit(mask_queenpawns[0], C3);
    SetBit(mask_queenpawns[0], C4);
    SetBit(mask_queenpawns[0], C5);

    SetBit(mask_queenpawns[1], A7);
    SetBit(mask_queenpawns[1], A6);
    SetBit(mask_queenpawns[1], A5);
    SetBit(mask_queenpawns[1], A4);
    SetBit(mask_queenpawns[1], B7);
    SetBit(mask_queenpawns[1], B6);
    SetBit(mask_queenpawns[1], B5);
    SetBit(mask_queenpawns[1], B4);
    SetBit(mask_queenpawns[1], C7);
    SetBit(mask_queenpawns[1], C6);
    SetBit(mask_queenpawns[1], C5);
    SetBit(mask_queenpawns[1], C4);

    SetBit(mask_kingpawns[0], F2);
    SetBit(mask_kingpawns[0], F3);
    SetBit(mask_kingpawns[0], F4);
    SetBit(mask_kingpawns[0], F5);
    SetBit(mask_kingpawns[0], G2);
    SetBit(mask_kingpawns[0], G3);
    SetBit(mask_kingpawns[0], G4);
    SetBit(mask_kingpawns[0], G5);
    SetBit(mask_kingpawns[0], H2);
    SetBit(mask_kingpawns[0], H3);
    SetBit(mask_kingpawns[0], H4);
    SetBit(mask_kingpawns[0], H5);

    SetBit(mask_kingpawns[1], F7);
    SetBit(mask_kingpawns[1], F6);
    SetBit(mask_kingpawns[1], F5);
    SetBit(mask_kingpawns[1], F4);
    SetBit(mask_kingpawns[1], G7);
    SetBit(mask_kingpawns[1], G6);
    SetBit(mask_kingpawns[1], G5);
    SetBit(mask_kingpawns[1], G4);
    SetBit(mask_kingpawns[1], H7);
    SetBit(mask_kingpawns[1], H6);
    SetBit(mask_kingpawns[1], H5);
    SetBit(mask_kingpawns[1], H4);

    SetBit(mask_corner, A1);
    SetBit(mask_corner, A8);
    SetBit(mask_corner, H1);
    SetBit(mask_corner, H8);

    not_mask_corner = ~mask_corner;
    not_mask_rookfiles = ~mask_rookfiles;
    not_mask_edge = ~mask_edge;

    BITBOARD b1, b2;
    int sq = -1;
    int z;

    for (x = 0; x < 64; x++)
        for (y = 0; y < 64; y++)
        {
            l_check[x][y] = -1;
            r_check[x][y] = -1;
            h_check[x][y] = col[y] + row[x] * 8;
            v_check[x][y] = row[y] * 8 + col[x];
            if ((row[x] == row[y]) || (col[x] == col[y]))
            {
                h_check[x][y] = -1;
                v_check[x][y] = -1;
                continue;
            }
            b1 = bit_bishopmoves[x] & bit_bishopmoves[y] & mask_nwdiag[x];
            sq = -1;
            if (b1 && !(bit_bishopmoves[x] & mask[y]))
            {
                sq = NextBit(b1);
                l_check[x][y] = sq;
            }

            b1 = bit_bishopmoves[x] & bit_bishopmoves[y] & mask_nediag[x];;
            if (b1 && !(bit_bishopmoves[x] & mask[y]))
            {
                sq = NextBit(b1);
                r_check[x][y] = sq;
            }
        }

    memset(q_check, -1, sizeof(q_check));

    for (x = 0; x < 64; x++)
        for (y = 0; y < 64; y++)
        {
            if (x == y)
                continue;
            z = 0;
            b1 = bit_queenmoves[x] & bit_queenmoves[y];
            while (b1)
            {
                sq = NextBit(b1);
                b1 &= not_mask[sq];
                q_check[x][y][z] = sq;
                z++;
            }
        }
}

void SetDir()
{
    int x, y;
    for (x = 0; x < 64; x++)
    {
        for (y = 0; y < 64; y++)
        {
            if (row[x] == row[y])
                if (y > x)
                    getdir[x][y] = 1;
                else
                    getdir[x][y] = -1;
            if (col[x] == col[y])
                if (y > x)
                    getdir[x][y] = 8;
                else
                    getdir[x][y] = -8;

            if (nwdiag[x] == nwdiag[y])
                if (y > x)
                    getdir[x][y] = 7;
                else
                    getdir[x][y] = -7;

            if (nediag[x] == nediag[y])
                if (y > x)
                    getdir[x][y] = 9;
                else
                    getdir[x][y] = -9;
        }
    }

    for (x = 0; x < 64; x++)
    {
        for (y = 0; y < 64; y++)
        {
            if (nwdiag[x] == nwdiag[y] || nediag[x] == nediag[y])
                diag[x][y] = 1;
        }
    }

    for (x = 0; x < 64; x++)
        for (y = 0; y < 64; y++)
        {
            if (row[x] == row[y] || col[x] == col[y])
                orthog[x][y] = 1;
        }

    int c1, r1;
    for (x = 0; x < 64; x++)
    {
        for (y = 0; y < 64; y++)//.69
        {
            c1 = abs(col[x] - col[y]);
            r1 = abs(row[x] - row[y]);
            if (c1 > r1)
            {
                difference[x][y] = c1;
                pawn_difference[x][y] = c1 * 20;
            }
            else
            {
                difference[x][y] = r1;
                pawn_difference[x][y] = r1 * 20;
            }
        }
    }
}

void SetBetweenVector()
{
    int x, y;
    int z;
    for (x = 0; x < 64; x++)
        for (y = 0; y < 64; y++)
        {
            if (row[x] == row[y])
            {
                if (y > x)
                    for (z = x + 1; z < y; z++)
                        SetBit(bit_between[x][y], z);
                else
                    for (z = y + 1; z < x; z++)
                        SetBit(bit_between[x][y], z);
            }

            if (col[x] == col[y])
            {
                if (y > x)
                    for (z = x + 8; z < y; z += 8)
                        SetBit(bit_between[x][y], z);
                else
                    for (z = y + 8; z < x; z += 8)
                        SetBit(bit_between[x][y], z);
            }

            if (nwdiag[x] == nwdiag[y])
            {
                if (y > x)
                    for (z = x + 7; z < y; z += 7)
                        SetBit(bit_between[x][y], z);
                else
                    for (z = y + 7; z < x; z += 7)
                        SetBit(bit_between[x][y], z);
            }

            if (nediag[x] == nediag[y])
            {
                if (y > x)
                    for (z = x + 9; z < y; z += 9)
                        SetBit(bit_between[x][y], z);
                else
                    for (z = y + 9; z < x; z += 9)
                        SetBit(bit_between[x][y], z);
            }

        }

    //vectorbits
    for (x = 0; x < 64; x++)
        for (y = 0; y < 64; y++)
        {
            if (row[x] == row[y])
            {
                if (y > x)
                    for (z = x + 1; z <= y; z++)
                        SetBit(vectorbits[x][y], z);
                else
                    for (z = y; z < x; z++)
                        SetBit(vectorbits[x][y], z);
            }

            if (col[x] == col[y])
            {
                if (y > x)
                    for (z = x + 8; z <= y; z += 8)
                        SetBit(vectorbits[x][y], z);
                else
                    for (z = y; z < x; z += 8)
                        SetBit(vectorbits[x][y], z);
            }

            if (nwdiag[x] == nwdiag[y])
            {
                if (y > x)
                    for (z = x + 7; z <= y; z += 7)
                        SetBit(vectorbits[x][y], z);
                else
                    for (z = y; z < x; z += 7)
                        SetBit(vectorbits[x][y], z);
            }

            if (nediag[x] == nediag[y])
            {
                if (y > x)
                    for (z = x + 9; z <= y; z += 9)
                        SetBit(vectorbits[x][y], z);
                else
                    for (z = y; z < x; z += 9)
                        SetBit(vectorbits[x][y], z);
            }

        }
    //vectors
    for (x = 0; x < 64; x++)
    {
        mask_vectors[x][NORTH] = vectorbits[x][56 + col[x]];
        mask_vectors[x][SOUTH] = vectorbits[x][col[x]];
        mask_vectors[x][WEST] = vectorbits[x][row[x] * 8];
        mask_vectors[x][EAST] = vectorbits[x][row[x] * 8 + 7];
        if (col[x] > 0 && row[x] < 7)mask_vectors[x][NW] = vectorbits[x][GetEdge(x, 7)];
        if (col[x] < 7 && row[x] < 7)mask_vectors[x][NE] = vectorbits[x][GetEdge(x, 9)];
        if (row[x] > 0 && col[x] > 0)mask_vectors[x][SW] = vectorbits[x][GetEdge(x, -9)];
        if (row[x] > 0 && col[x] < 7)mask_vectors[x][SE] = vectorbits[x][GetEdge(x, -7)];
    }

}

//masks for ranks/file by square?
void SetMaskDiags()
{
    for (int x = 0; x < 64; x++)
        for (int y = 0; y < 64; y++)
        {
            if (nediag[x] == nediag[y])
                SetBit(mask_nediag[x], y);
            if (nwdiag[x] == nwdiag[y])
                SetBit(mask_nwdiag[x], y);
        }
}

int GetEdge(int sq, int plus)
{
    do
    {
        sq += plus;
    } while (col[sq] > 0 && col[sq] < 7 && row[sq]>0 && row[sq] < 7);

    return sq;
}

void SetRowCol()
{
    int x, y;
    for (x = 0; x < 64; x++)
    {
        if (col[x] < 4)
            SetBit(mask_queenside, x);
    }
    //make mask_pawn usable for p v k or k v p
    for (x = 0; x < 64; x++)
    {
        for (y = 0; y < 64; y++)
        {

            if (row[x] != 0 && row[x] != 7)
            {
                if (row[x] > 1)
                {
                    if (row[y] >= row[x] && abs(col[x] - col[y]) <= 7 - row[x])
                        SetBit(mask_squarepawn[0][x], y);
                }
                else
                {
                    if (row[y] > row[x] && abs(col[x] - col[y]) <= 6 - row[x])
                        SetBit(mask_squarepawn[0][x], y);
                }
                if (row[x] < 6)
                {
                    if (row[y] <= row[x] && abs(col[x] - col[y]) <= row[x])
                        SetBit(mask_squarepawn[1][x], y);
                }
                {
                    if (row[y] < row[x] && abs(col[x] - col[y]) < row[x])
                        SetBit(mask_squarepawn[1][x], y);
                }

            }

            if (abs(col[x] - col[y]) < 2)
            {
                if (row[x] < row[y] && row[y] < 7)
                    SetBit(mask_passed[0][x], y);
                if (row[x] > row[y] && row[y] > 0)
                    SetBit(mask_passed[1][x], y);
            }

            if (abs(col[x] - col[y]) == 1 && row[x] != 0 && row[x] != 7)
            {
                if (row[x] >= row[y] || (row[x] == 1 && row[y] == 2))
                    SetBit(mask_backward[0][x], y);
                if (row[x] <= row[y] || (row[x] == 6 && row[y] == 5))
                    SetBit(mask_backward[1][x], y);
            }

            if (abs(col[x] - col[y]) == 1)
            {
                SetBit(mask_isolated[x], y);
            }
            if (col[x] - col[y] == 1)
            {
                SetBit(mask_left_col[x], y);
            }
            if (col[y] - col[x] == 1)
            {
                SetBit(mask_right_col[x], y);
            }

            if (col[x] == col[y])
            {
                if (row[x] < row[y])
                    SetBit(mask_path[0][x], y);
                if (row[x] > row[y])
                    SetBit(mask_path[1][x], y);
            }
        }
    }
    for (x = 0; x < 64; x++)
    {
        if (col[x] > 0)
            SetBit(mask_adjacent[x], x - 1);
        if (col[x] < 7)
            SetBit(mask_adjacent[x], x + 1);
    }
    for (int x = 0; x < 64; x++)
    {
        for (y = 0; y < 64; y++)
        {
            if (row[y]<row[x] || abs(col[x] - col[y])>row[y])
                SetBit(mask_squareking[0][x], y);
            if (row[y] > row[x] || abs(col[x] - col[y]) > rank[1][y])
                SetBit(mask_squareking[1][x], y);
        }
    }

    memset(bishop_a7, 0, sizeof(bishop_a7));
    memset(bishop_h7, 0, sizeof(bishop_h7));
    memset(knight_a7, 0, sizeof(knight_a7));
    memset(knight_h7, 0, sizeof(knight_h7));

    SetBit(bishop_a7[0], C7);
    SetBit(bishop_a7[0], B6);
    SetBit(bishop_a7[1], C2);
    SetBit(bishop_a7[1], B3);
    SetBit(bishop_h7[0], F7);
    SetBit(bishop_h7[0], G6);
    SetBit(bishop_h7[1], F2);
    SetBit(bishop_h7[1], G3);

    SetBit(knight_a7[0], B7);
    SetBit(knight_a7[0], C6);
    SetBit(knight_a7[1], B2);
    SetBit(knight_a7[1], C3);
    SetBit(knight_h7[0], G7);
    SetBit(knight_h7[0], F6);
    SetBit(knight_h7[1], F3);
    SetBit(knight_h7[1], G2);
}

void SetRanks()
{
    for (int x = 0; x < 64; x++)
    {
        squares[0][x] = x;
        squares[1][x] = (7 - row[x]) * 8 + col[x];
        rank[0][x] = row[x];
        rank[1][x] = 7 - row[x];
    }

    for (int s = 0; s < 2; s++)
        for (int x = 0; x < 64; x++)
        {
            lastsquare[0][x] = col[x] + A8;
            lastsquare[1][x] = col[x];
        }

    memset(adjfile, 0, sizeof(adjfile));
    for (int x = 0; x < 64; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            if (abs(col[x] - col[y]) < 2)
                adjfile[x][y] = 1;
        }
    }

    memset(kingqueen, 0, sizeof(kingqueen));
    for (int x = 0; x < 64; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            kingqueen[x][y] = taxi[difference[x][y]];
        }
    }

    memset(kingknight, 0, sizeof(kingknight));
    for (int x = 0; x < 64; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            if (bit_knightmoves[x] & bit_kingmoves[y])
                kingknight[x][y] = 5;
        }
    }
}

void SetKingMoves()
{
    int j;

    int k = 0;
    int sq;
    int y;
    BITBOARD bn;
    for (int x = 0; x < 64; x++)
    {
        bn = 0;
        k = 0;
        for (j = 0; j < 8; ++j)
        {
            sq = mailbox[mailbox64[x] + offset[KNIGHT][j]];
            if (sq > -1)
            {
                knightmoves[x][k++] = sq;
            }
        }
        for (int z = 0; z < 8; z++)
        {
            kmoves[x][z] = -1;
        }

        if (col[x] > 0) kmoves[x][WEST] = x - 1;
        if (col[x] < 7) kmoves[x][EAST] = x + 1;
        if (row[x] > 0) kmoves[x][SOUTH] = x - 8;
        if (row[x] < 7) kmoves[x][NORTH] = x + 8;
        if (col[x] < 7 && row[x] < 7) kmoves[x][NE] = x + 9;
        if (col[x] > 0 && row[x] < 7) kmoves[x][NW] = x + 7;
        if (col[x] > 0 && row[x] > 0) kmoves[x][SW] = x - 9;
        if (col[x] < 7 && row[x]>0) kmoves[x][SE] = x - 7;

        y = 0;
        if (col[x] > 0)
            kingmoves[x][y++] = x - 1;
        if (col[x] < 7)
            kingmoves[x][y++] = x + 1;
        if (row[x] > 0)
            kingmoves[x][y++] = x - 8;
        if (row[x] < 7)
            kingmoves[x][y++] = x + 8;
        if (col[x] < 7 && row[x] < 7)
            kingmoves[x][y++] = x + 9;
        if (col[x] > 0 && row[x] < 7)
            kingmoves[x][y++] = x + 7;
        if (col[x] > 0 && row[x] > 0)
            kingmoves[x][y++] = x - 9;
        if (col[x] < 7 && row[x]>0)
            kingmoves[x][y++] = x - 7;
    }

    link Links[8];
    int nc;

    int n;
    int c = 0;
    BITBOARD b1;

    for (int x = 0; x < 64; x++)
    {
        b1 = 0; c = 0; nc = 0;
        for (int k = 0; k < 14; k++)
            bishoplist[x][k].next = -1;
        for (int j = 1; j < 8; j += 2)
        {
            n = x;
            Links[nc].first = c;
            while (true)
            {
                n = kmoves[n][j];
                if (n == -1)
                    break;
                bishoplist[x][c].sq = n;
                c++;
                if (c < 13)
                    Links[nc].next = c;
                else
                    Links[nc].next = -1;
                b1 |= mask[n];
            }
            Links[nc].last = c - 1;
            nc++;
        }
        bishoplist[x][c].sq = -1;
        for (int num = 0; num < nc; num++)
            for (int z = Links[num].first; z <= Links[num].last; z++)
            {
                bishoplist[x][z].next = Links[num].next;
            }
    }

    for (int x = 0; x < 64; x++)
    {
        b1 = 0; c = 0; nc = 0;
        for (int k = 0; k < 15; k++)
            rooklist[x][k].next = -1;//
        for (int j = 0; j < 8; j += 2)
        {
            n = x;
            Links[nc].first = c;
            while (true)
            {
                n = kmoves[n][j];
                if (n == -1)
                    break;
                rooklist[x][c].sq = n;
                c++;
                if (c < 14)
                    Links[nc].next = c;
                else
                    Links[nc].next = -1;
                b1 |= mask[n];
            }
            Links[nc].last = c - 1;
            nc++;
        }
        rooklist[x][c].sq = -1;
        for (int num = 0; num < nc; num++)
            for (int z = Links[num].first; z <= Links[num].last; z++)
            {
                rooklist[x][z].next = Links[num].next;
            }
    }
    for (int x = 0; x < 64; x++)
    {
        b1 = 0; c = 0; nc = 0;
        for (int k = 0; k < 28; k++)
            queenlist[x][k].next = -1;
        for (int j = 0; j < 8; j++)
        {
            n = x;
            Links[nc].first = c;
            while (true)
            {
                n = kmoves[n][j];
                if (n == -1)
                    break;
                queenlist[x][c].sq = n;
                c++;
                if (c < 28)
                    Links[nc].next = c;
                else
                    Links[nc].next = -1;
                b1 |= mask[n];
            }
            Links[nc].last = c - 1;
            nc++;
        }
        queenlist[x][c].sq = -1;
        for (int num = 0; num < nc; num++)
            for (int z = Links[num].first; z <= Links[num].last; z++)
            {
                queenlist[x][z].next = Links[num].next;
            }
    }
}

int NextBit(BITBOARD bb)//folded - used for ages
{
    unsigned int folded;
    //assert (bb != 0);
    bb ^= bb - 1;
    folded = (int)bb ^ (bb >> 32);
    return lsb_64_table[folded * 0x78291ACF >> 26];
}
//*/
int NextBit2(BITBOARD bb)//number 2  crashed
{
    if (bb == 0) return 0;
    const BITBOARD debruijn64 = (unsigned char)(0x03f79d71b4cb0a89);
    assert(bb != 0);
    return index64[((bb ^ (bb - 1)) * debruijn64) >> 58];
}

int NextBit1(BITBOARD bb)//slow
{
    int r = 0;
    while (bb >>= 1ui64) 
    {
        r++;
    }
    return r;
    while (bb = bb >> 8)
        r++;
}

int NextBitWest(BITBOARD bb)
{
    int r = 0;
    while (bb >>= 1)
    {
        r++;
    }
    return r;
}

int NextBitSouth(BITBOARD bb)
{
    int r = 0;
    while (bb >>= 8)
    {
        r++;
    }
    return r;
}

int NextBitNorth(BITBOARD bb)
{
    int r = 0;
    while (bb >>= 1)
    {
        r++;
    }
    return r;
}

int NextBitEast(BITBOARD bb)
{
    int r = 0;
    while (bb >>= 1)
    {
        r++;
    }
    return r;
}

int popCount(BITBOARD x)
{
    int count = 0;
    BITBOARD b2 = x;
    while (x)
    {
        count++;
        x &= x - 1; // reset LS1B  x = x & x-1
    }
    return count;
}

void RookMoves(const int x)
{
    int nc = 0;
    int sq = rooklist[x][nc].sq;
    do
    {
        if (mask[sq] & bit_all)
        {
            if (mask[sq] & bit_units[xside])
                CapRook(x, sq);
            nc = rooklist[x][nc].next;
        }
        else
        {
            nc++;
            PushRook(x, sq);
        }
        sq = rooklist[x][nc].sq;
    } while (sq > -1 && nc > -1);
}

void BishopMoves(const int x)
{
    int nc = 0;
    int sq = bishoplist[x][nc].sq;
    do
    {
        if (mask[sq] & bit_all)
        {
            if (mask[sq] & bit_units[xside])
                CapBishop(x, sq);
            nc = bishoplist[x][nc].next;
        }
        else
        {
            nc++;
            PushBishop(x, sq);
        }
        sq = bishoplist[x][nc].sq;
    } while (sq > -1 && nc > -1);
}

void QueenMoves(const int x)
{
    int nc = 0;
    int sq = queenlist[x][nc].sq;
    do
    {
        if (mask[sq] & bit_all)
        {
            if (mask[sq] & bit_units[xside])
                CapQueen(x, sq);
            nc = queenlist[x][nc].next;
        }
        else
        {
            nc++;
            PushQueen(x, sq);
        }
        sq = queenlist[x][nc].sq;
    } while (sq > -1);
}

int RookMoveCount(const int x)
{
    int nc = 0;
    int sq = rooklist[x][nc].sq;
    int count = 0;

    do
    {
        if (mask[sq] & bit_all)
        {
            count++;
            nc = rooklist[x][nc].next;
        }
        else
        {
            nc++;
            count++;
        }
        sq = rooklist[x][nc].sq;
    } while (sq > -1 && nc > -1);
    return count;
}

int BishopMoveCount(const int x, const BITBOARD bp)
{
    int nc = 0;
    int sq = bishoplist[x][nc].sq;
    int count = 0;

    do
    {
        if (mask[sq] & bit_all)
        {
			if(!(mask[sq] & bp))
              count++;
            nc = bishoplist[x][nc].next;
        }
        else
        {
            nc++;
            count++;
        }
        sq = bishoplist[x][nc].sq;
    } while (sq > -1 && nc > -1);
    return count;
}