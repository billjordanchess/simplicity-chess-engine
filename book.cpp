#include "globals.h"

int kingqueen[64][64];
int kingknight[64][64];
int kingking[64][64];
int taxi[] = { 0,16,12,8,4,0,0,0 };
int kingtaxi[] = { 0,24,20,16,12,8,4,0 };

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

//matts 32 bit friendly version
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

const int colors[64] = {
	 1,0,1,0,1,0,1,0,
	 0,1,0,1,0,1,0,1,
	 1,0,1,0,1,0,1,0,
	 0,1,0,1,0,1,0,1,
	 1,0,1,0,1,0,1,0,
	 0,1,0,1,0,1,0,1,
	 1,0,1,0,1,0,1,0,
	 0,1,0,1,0,1,0,1 };

const int nwdiag[64] = {
	 14,13,12,11,10, 9, 8, 7,
	 13,12,11,10, 9, 8, 7, 6,
	 12,11,10, 9, 8, 7, 6, 5,
	 11,10, 9, 8, 7, 6, 5, 4,
	 10, 9, 8, 7, 6, 5, 4, 3,
	  9, 8, 7, 6, 5, 4, 3, 2,
	  8, 7, 6, 5, 4, 3, 2, 1,
	  7, 6, 5, 4, 3, 2, 1, 0 };

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

BITBOARD bishop_a7[2];
BITBOARD bishop_h7[2];
BITBOARD knight_a7[2];
BITBOARD knight_h7[2];

BITBOARD passed_list[2];
BITBOARD mask_left_col[64];
BITBOARD mask_right_col[64];

BITBOARD bit_adjacent[64];

BITBOARD mask_centre;

BITBOARD mask_nwdiag[64];
BITBOARD mask_nediag[64];

BITBOARD mask_abc;
BITBOARD mask_def;
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

BITBOARD bit_knightmoves[64];
BITBOARD bit_bishopmoves[64];
BITBOARD bit_rookmoves[64];
BITBOARD bit_queenmoves[64];
BITBOARD bit_kingmoves[64];

//current position
BITBOARD bit_pieces[2][7];
BITBOARD bit_units[2];//pieces+pawns
BITBOARD bit_all;

//current attacks
BITBOARD bit_leftcaptures[2];
BITBOARD bit_rightcaptures[2];
BITBOARD bit_pawnattacks[2];

BITBOARD bit_colors;
BITBOARD bit_color[2];

BITBOARD mask_passed[2][64];
BITBOARD mask_path[2][64];
BITBOARD mask_backward[2][64];

BITBOARD mask_ranks[2][8];
BITBOARD mask_files[8];
BITBOARD mask_cols[64];

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
BITBOARD not_a_file;
BITBOARD not_h_file;
BITBOARD not_mask_edge;
BITBOARD not_mask_corner;
BITBOARD not_rank6;
BITBOARD not_rank1;
BITBOARD not_mask_files[8];
BITBOARD not_mask_rows[8];

BITBOARD mask[64];
BITBOARD not_mask[64];

void SetBit(BITBOARD& bb, int square);
void SetBitFalse(BITBOARD& bb, int square);
void PrintBitBoard(BITBOARD bb);
void PrintCell(int x, BITBOARD bb);

void SetColors();
void SetPawnBits();
void SetPawnMoves();
void SetRanksFiles();
void SetSquares();
void SetKingPawns();
void SetMoves();
void SetChecks();
void SetRanks();
void SetMaskPawns();
void SetLinks();
void SetDifference();
void SetBetweenVector();

void SetKingDistance();

void SetBit(BITBOARD& bb, int square)
{
	bb |= (1ui64 << square);
}

void SetBitFalse(BITBOARD& bb, int square)
{
	bb &= ~mask[square];
}

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

void SetBits()
{
	SetColors();
	SetRanks();
	SetPawnMoves();
	SetPawnBits();
	SetRanksFiles();
	SetSquares();
	SetKingPawns();
	SetChecks();
	SetMaskPawns();
	SetBetweenVector();
	SetMoves();
	SetDifference();
	SetKingDistance();
}

void SetColors()
{
	bit_colors = 0;
	for (int x = 0; x < 64; x++)
	{
		if (colors[x] == 1)
			SetBit(bit_colors, x);
	}

	for (int x = 0; x < 64; x++)
	{
		if (colors[x] == 0)
			SetBit(bit_color[0], x);
		if (colors[x] == 1)
			SetBit(bit_color[1], x);
	}
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
}

void SetPawnMoves()
{
	int x;

	for (x = 0; x < 64; x++)
	{
		pawnleft[0][x] = -1;
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
		if (row[x] < 7)
		{
			pawnplus[0][x] = x + 8;
		}
		if (row[x] < 6)
		{
			pawndouble[0][x] = x + 16;
		}
		if (row[x] > 0)
		{
			pawnplus[1][x] = x - 8;
		}
		if (row[x] > 1)
		{
			pawndouble[1][x] = x - 16;

		}
	}
}

void SetPawnBits()
{
	int x;
	memset(bit_pawncaptures, 0, sizeof(bit_pawncaptures));

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
}

void SetRanksFiles()
{
	int x, y;

	for (y = 0; y < 8; y++)
		for (x = 0; x < 64; x++)
		{
			if (col[x] == y)
				SetBit(mask_files[y], x);
			if (row[x] == y)
			{
				SetBit(mask_ranks[0][y], x);
			}
			if (row[x] == 7 - y)
			{
				SetBit(mask_ranks[1][y], x);
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
	not_rank6 = ~mask_ranks[0][6];
	not_rank1 = ~mask_ranks[0][1];
	mask_rookfiles = mask_files[0] | mask_files[7];
}

void SetSquares()
{
	for (int x = 0; x < 64; x++)
	{
		SetBit(mask[x], x);
	}
	for (int x = 0; x < 64; x++)
	{
		if (col[x] > 0)
			SetBit(bit_adjacent[x], x - 1);
		if (col[x] < 7)
			SetBit(bit_adjacent[x], x + 1);
	}
	for (int x = 0; x < 64; x++)
	{
		if (col[x] > 0)
			SetBit(mask_adjacent[x], x - 1);
		if (col[x] < 7)
			SetBit(mask_adjacent[x], x + 1);
	}
	for (int x = 0; x < 64; x++)
	{
		if (row[x] == 0 || row[x] == 7 || col[x] == 0 || col[x] == 7)
			SetBit(mask_edge, x);
	}
	for (int x = 0; x < 64; x++)
	{
		if (col[x] < 3)
			SetBit(mask_abc, x);
		if (col[x] > 4)
			SetBit(mask_def, x);
	}
	SetBit(mask_centre, D4);
	SetBit(mask_centre, E4);
	SetBit(mask_centre, D5);
	SetBit(mask_centre, E4);

	SetBit(mask_corner, A1);
	SetBit(mask_corner, A8);
	SetBit(mask_corner, H1);
	SetBit(mask_corner, H8);

	not_mask_corner = ~mask_corner;
	not_mask_rookfiles = ~mask_rookfiles;
	not_mask_edge = ~mask_edge;
}

void SetKingPawns()
{
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
}

void SetChecks()
{
	BITBOARD b1;
	int sq = -1;
	int x, y, z;

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

void SetDifference()
{
	int x, y;

	int col_diff, row_diff;
	for (x = 0; x < 64; x++)
	{
		for (y = 0; y < 64; y++)
		{
			col_diff = abs(col[x] - col[y]);
			row_diff = abs(row[x] - row[y]);
			if (col_diff > row_diff)
			{
				difference[x][y] = col_diff;
				pawn_difference[x][y] = col_diff * 20;
			}
			else
			{
				difference[x][y] = row_diff;
				pawn_difference[x][y] = row_diff * 20;
			}
		}
	}
}

void SetMoves()
{
	memset(bit_knightmoves, 0, sizeof(bit_knightmoves));
	memset(bit_bishopmoves, 0, sizeof(bit_bishopmoves));
	memset(bit_rookmoves, 0, sizeof(bit_rookmoves));
	memset(bit_queenmoves, 0, sizeof(bit_queenmoves));
	memset(bit_kingmoves, 0, sizeof(bit_kingmoves));

	for (int x = 0; x < 64; x++)
	{
		bit_knightmoves[x] = 0;
		if (row[x] < 6 && col[x] < 7)
			bit_knightmoves[x] |= mask[x + 17];
		if (row[x] < 7 && col[x] < 6)
			bit_knightmoves[x] |= mask[x + 10];
		if (row[x] < 6 && col[x]>0)
			bit_knightmoves[x] |= mask[x + 15];
		if (row[x] < 7 && col[x]>1)
			bit_knightmoves[x] |= mask[x + 6];
		if (row[x] > 1 && col[x] < 7)
			bit_knightmoves[x] |= mask[x - 15];
		if (row[x] > 0 && col[x] < 6)
			bit_knightmoves[x] |= mask[x - 6];
		if (row[x] > 1 && col[x] > 0)
			bit_knightmoves[x] |= mask[x - 17];
		if (row[x] > 0 && col[x] > 1)
			bit_knightmoves[x] |= mask[x - 10];
	}
	for (int x = 0; x < 64; x++)
	{
		if (col[x] > 0)
			bit_kingmoves[x] |= mask[x - 1];
		if (col[x] < 7)
			bit_kingmoves[x] |= mask[x + 1];
		if (row[x] > 0)
			bit_kingmoves[x] |= mask[x - 8];
		if (row[x] < 7)
			bit_kingmoves[x] |= mask[x + 8];
		if (col[x] < 7 && row[x] < 7)
			bit_kingmoves[x] |= mask[x + 9];
		if (col[x] > 0 && row[x] < 7)
			bit_kingmoves[x] |= mask[x + 7];
		if (col[x] > 0 && row[x] > 0)
			bit_kingmoves[x] |= mask[x - 9];
		if (col[x] < 7 && row[x]>0)
			bit_kingmoves[x] |= mask[x - 7];
	}
	for (int x = 0; x < 64; x++)
	{
		bit_bishopmoves[x] = 0;
		bit_rookmoves[x] = 0;
		bit_queenmoves[x] = 0;

		for (int y = 0; y < 64; y++)
		{
			if (x == y)
				continue;
			if (nwdiag[x] == nwdiag[y] || nediag[x] == nediag[y])
				bit_bishopmoves[x] |= mask[y];
			if (row[x] == row[y] || col[x] == col[y])
				bit_rookmoves[x] |= mask[y];
			if (nwdiag[x] == nwdiag[y] || nediag[x] == nediag[y] || row[x] == row[y] || col[x] == col[y])
				bit_queenmoves[x] |= mask[y];
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
}

void SetMaskPawns()
{
	int x, y;

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
}

void SetTrappedMinorPiece()
{
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

void SetKingDistance()
{
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
	memset(kingking, 0, sizeof(kingking));
	for (int x = 0; x < 64; x++)
	{
		for (int y = 0; y < 64; y++)
		{
			kingking[x][y] = kingtaxi[difference[x][y]];
		}
	}
}

int NextBit(BITBOARD bb)
{
	return index64[((bb & -bb) * debruijn64) >> 58];
}

/*
int NextBit(BITBOARD bb)//folded - 32 bit friendly alternative
{
	unsigned int folded;
	assert (bb != 0);
	bb ^= bb - 1;
	folded = (int)bb ^ (bb >> 32);
	return lsb_64_table[folded * 0x78291ACF >> 26];
}
//*/









