#pragma once
#include "globals.h"

bool KingAttack(const int s, int k, const int start, const int dest);
void UnMakeCapture();
void AfterCastle();
void BeforeCastle();

void UpdatePawn(const int s, const int start, const int dest)
{
	bit_units[s] &= not_mask[start];
	bit_units[s] |= mask[dest];
	bit_all = bit_units[0] | bit_units[1];
	AddKeys(s, 0, start, dest);
	b[dest] = 0;
	b[start] = EMPTY;
	bit_pieces[s][0] &= not_mask[start];
	bit_pieces[s][0] |= mask[dest];
	AddPawnKey(s, start);
	AddPawnKey(s, dest);
}

void UpdatePiece(const int s, const int p, const int start, const int dest)
{
	bit_units[s] &= not_mask[start];
	bit_units[s] |= mask[dest];
	bit_all = bit_units[0] | bit_units[1];
	AddKeys(s, p, start, dest);
	b[dest] = p;
	b[start] = EMPTY;
	bit_pieces[s][p] &= not_mask[start];
	bit_pieces[s][p] |= mask[dest];

	if (p == 0)
	{
		AddPawnKeys(s, start, dest);
		return;
	}
	table_score[s] -= PieceScore[s][p][start];
	table_score[s] += PieceScore[s][p][dest];
	
	for (int x = 0; x < total[s][p]; x++)
	{
		if (pieces[s][p][x] == start)
		{
			pieces[s][p][x] = dest;
			break;
		}
	}
}

void RemovePiece(const int s, const int p, const int sq)
{
	AddKey(s, p, sq);
	b[sq] = EMPTY;
	bit_units[s] &= not_mask[sq];
	bit_all = bit_units[0] | bit_units[1];
	bit_pieces[s][p] &= not_mask[sq];
	
	if (p == 0)
	{
		pawn_mat[s] -= 100;
		AddPawnKey(s, sq);
		return;
	}
	table_score[s] -= PieceScore[s][p][sq];	

	total[s][p]--;
	//if(pieces[s][p][0] == sq)
	//	pieces[s][p][0] = pieces[s][p][1];
	if (pieces[s][p][total[s][p]] != sq)
	{
		for (int x = 0; x < total[s][p]; x++)
		{
			if (pieces[s][p][x] == sq)
			{
				pieces[s][p][x] = pieces[s][p][total[s][p]];
				break;
			}
		}
	}
	piece_mat[s] -= piece_value[p];
}

void AddPiece(const int s, const int p, const int sq)
{
	b[sq] = p;
	AddKey(s, p, sq);
	bit_units[s] |= mask[sq];
	bit_all = bit_units[0] | bit_units[1];
	bit_pieces[s][p] |= mask[sq];
		
	if (p == 0)
	{
		pawn_mat[s] += 100;
		AddPawnKey(s, sq);
		return;
	}
	table_score[s] += PieceScore[s][p][sq];
	pieces[s][p][total[s][p]++] = sq;
	piece_mat[s] += piece_value[p];
}

bool MakeMove(const int from, const int to, const int flags)
{
	if (b[from] == 5 && abs(from - to) == 2)
	{
		if (!(flags & CASTLE))
		{
			//printf("1");
		}
		UpdatePiece(side, ROOK, castle_start[to], castle_dest[to]);
		KingScore[side][squares[side][E1]] = -40;
		if (col[to] == 6)
		{
			AfterCastle();
		}
	}

	game_list[hply].flags = flags;
	game_list[hply].from = from;
	game_list[hply].to = to;
	game_list[hply].capture = b[to];
	game_list[hply].castle = castle;
	game_list[hply].fifty = fifty;
	game_list[hply].hash = currentkey;
	game_list[hply].lock = currentlock;
	++ply;
	++hply;

	castle &= castle_mask[from] & castle_mask[to];

	fifty++;

	epflag = 0;

	if (b[from] == 0)
	{
		fifty = 0;
		if (b[to] == EMPTY && col[from] != col[to])
		{
			RemovePiece(xside, 0, pawnplus[xside][to]);
		}
		if (b[to] < 6)
		{
			RemovePiece(xside, b[to], to);
		}
		else
		{
			if (abs(to - from) == 16 && bit_adjacent[to] & bit_pieces[xside][0])
			{
				epflag = to;
			}
		}
		if (rank[side][to] == 7)
		{
			RemovePiece(side, PAWN, from);
			AddPiece(side, 4, to);
			game_list[hply - 1].flags |= PROMOTE;
		}
		else
		{
			UpdatePawn(side, from, to);
		}
	}
	else
	{
		if (b[to] < 6)
		{
			fifty = 0;
			RemovePiece(xside, b[to], to);
		}
		UpdatePiece(side, b[from], from, to);
	}

	side ^= 1;
	xside ^= 1;

	if (Attack(side, pieces[xside][5][0]))
	{
		UnMakeMove();
		return false;
	}
	return true;
}

void UnMakeMove()
{
	side ^= 1;
	xside ^= 1;
	--ply;
	--hply;
	game* m = &game_list[hply];
	castle = m->castle;
	fifty = m->fifty;

	if (m->flags & PROMOTE)
	{
		AddPiece(side, PAWN, m->from);
		RemovePiece(side, b[m->to], m->to);
	}
	else
	{
		UpdatePiece(side, b[m->to], m->to, m->from);
	}
	if (m->capture != EMPTY)
	{
		AddPiece(xside, m->capture, m->to);
	}

	if (b[m->from] == 5 && abs(m->from - m->to) == 2)
	{
		int from, to;
		to = castle_start[m->to];
		from = castle_dest[m->to];
		UpdatePiece(side, ROOK, from, to);
		KingScore[side][squares[side][E1]] = 10;
		if (col[to] == 7)
		{
			BeforeCastle();
		}
	}
	if (b[m->from] == 0 && m->capture == EMPTY && col[m->from] != col[m->to])//ep
	{
		AddPiece(xside, 0, pawnplus[xside][m->to]);
	}	
}

int MakeCapture(const int from, const int to, const int flags)
{
	game_list[hply].flags = flags;
	game_list[hply].from = from;
	game_list[hply].to = to;
	game_list[hply].capture = b[to];
	game_list[hply].fifty = 0;//
	game_list[hply].hash = currentkey;
	game_list[hply].lock = currentlock;
	++ply;
	++hply;
	if (b[to] < 6)
	{
		RemovePiece(xside, b[to], to);
	}
	else if (b[from] == 0 && col[from] != col[to])
	{
		RemovePiece(xside, 0, pawnplus[xside][to]);
	}

	if (b[from] == 0)
	{
		if (rank[side][to] == 7)
		{
			RemovePiece(side, 0, from);
			AddPiece(side, 4, to);
			game_list[hply - 1].flags |= PROMOTE;
		}
		else
			UpdatePawn(side, from, to);
	}
	else
	{
		UpdatePiece(side, b[from], from, to);
	}
	side ^= 1;
	xside ^= 1;
	if (Attack(side, pieces[xside][5][0]))
	{
		UnMakeCapture();
		return false;
	}
	return true;
}

void UnMakeCapture()
{
	side ^= 1;
	xside ^= 1;
	--ply;
	--hply;
	game* h = &game_list[hply];
	fifty = h->fifty;

	if (h->flags & PROMOTE)
	{
		AddPiece(side, 0, h->from);
		RemovePiece(side, b[h->to], h->to);
		if (h->capture != EMPTY)
		{
			AddPiece(xside, h->capture, h->to);
		}
		return;
	}
	UpdatePiece(side, b[h->to], h->to, h->from);
	if (h->capture != EMPTY)
	{
		AddPiece(xside, h->capture, h->to);
	}
	if (b[h->from] == 0 && h->capture == EMPTY && col[h->from] != col[h->to])//ep
	{
		AddPiece(xside, 0, pawnplus[xside][h->to]);
	}
}

void MakeRecapture(const int from, const int to)
{
	game_list[hply].from = from;
	game_list[hply].to = to;
	game_list[hply].capture = b[to];

	++ply;
	++hply;

	RemovePiece(xside, b[to], to);
	UpdatePiece(side, b[from], from, to);
	side ^= 1;
	xside ^= 1;
}

void UnMakeRecapture()
{
	side ^= 1;
	xside ^= 1;
	--ply;
	--hply;
	UpdatePiece(side, b[game_list[hply].to], game_list[hply].to, game_list[hply].from);
	AddPiece(xside, game_list[hply].capture, game_list[hply].to);
}


bool KingAttack(const int s, int k, const int start, const int dest)
{
	BITBOARD b1 = bit_bishopmoves[k] & (bit_pieces[s][2] | bit_pieces[s][4]);
	b1 |= (bit_rookmoves[k] & (bit_pieces[s][3] | bit_pieces[s][4]));
	b1 &= not_mask[dest];

	BITBOARD b2 = (bit_all | mask[dest]) & not_mask[start];

	while (b1)
	{
		int i = NextBit(b1);
		if (!(bit_between[i][k] & b2))
			return true;
		b1 &= not_mask[i];
	}
	return false;
}

void BeforeCastle()
{
	KingScore[side][squares[side][F1]] = -60;
	KingScore[side][squares[side][F2]] = -45;
	KingScore[side][squares[side][G1]] = -20;
	PieceScore[side][0][squares[side][E2]] = -8;
	PieceScore[side][2][squares[side][G2]] = 4;
	PieceScore[side][2][squares[side][F1]] = -12;
	PieceScore[side][1][squares[side][G1]] = -20;
}

void AfterCastle()
{
	KingScore[side][squares[side][F1]] = -20;
	KingScore[side][squares[side][F2]] = -25;
	KingScore[side][squares[side][G1]] = 20;
	PieceScore[side][0][squares[side][E2]] = 0;
	PieceScore[side][2][squares[side][G2]] = 8;
	PieceScore[side][2][squares[side][F1]] = -10;
	PieceScore[side][1][squares[side][G1]] = -16;	
}