#pragma once
#include "globals.h"

int index[64];

/*
BTBOARD old = bit_all;
bit_all &= not_mask[start];
bit_all |= mask[dest];
if (Attack(side, pieces[xside][5][0]))
{
bit_all = old;
return false;
}
bit_all = old;


at top of MakeMove and MakeCapture
*/

void UnMakeCapture();
void AfterCastle();
void BeforeCastle();
void AddPawnKey(const int s, const int x);

void UpdatePawn(const int s, const int from, const int to)
{
	bit_units[s] &= not_mask[from];
	bit_units[s] |= mask[to];
	bit_all = bit_units[0] | bit_units[1];
	AddKeys(s, P, from, to);
	b[to] = P;
	b[from] = EMPTY;
	bit_pieces[s][P] &= not_mask[from];
	bit_pieces[s][P] |= mask[to];
	AddPawnKeys(s, from, to);
}

void UpdatePiece(const int s, const int p, const int from, const int to)
{
	bit_units[s] &= not_mask[from];
	bit_units[s] |= mask[to];
	bit_all = bit_units[0] | bit_units[1];
	AddKeys(s, p, from, to);
	b[to] = p;
	b[from] = EMPTY;
	bit_pieces[s][p] &= not_mask[from];
	bit_pieces[s][p] |= mask[to];

	if (p == P)
	{
		AddPawnKeys(s, from, to);
		return;
	}
	table_score[s] -= PieceScore[s][p][from];
	table_score[s] += PieceScore[s][p][to];

	index[to] = index[from];
	pieces[s][p][index[to]] = to;
}

void RemovePiece(const int s, const int p, const int sq)
{
	AddKey(s, p, sq);
	b[sq] = EMPTY;
	bit_units[s] &= not_mask[sq];
	bit_all = bit_units[0] | bit_units[1];
	bit_pieces[s][p] &= not_mask[sq];

	if (p == P)
	{
		pawn_mat[s] --;
		AddPawnKey(s, sq);
		return;
	}
	table_score[s] -= PieceScore[s][p][sq];

	total[s][p]--;

	if (index[sq] < total[s][p])
	{
		pieces[s][p][index[sq]] = pieces[s][p][total[s][p]];
		index[pieces[s][p][total[s][p]]] = 0;
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

	if (p == P)
	{
		pawn_mat[s]++;
		AddPawnKey(s, sq);
		return;
	}
	table_score[s] += PieceScore[s][p][sq];
	index[sq] = total[s][p];
	pieces[s][p][total[s][p]] = sq;
	total[s][p]++;
	piece_mat[s] += piece_value[p];
}

bool MakeMove(const int from, const int to, const int flags)
{
	if (b[from] == K)
	{
		if (Attack(xside, to))
		{
			return false;
		}
		if (abs(from - to) == 2)
		{
			UpdatePiece(side, ROOK, castle_start[to], castle_dest[to]);
			KingScore[side][squares[side][E1]] = -40;
			if (col[to] == 6)
			{
				AfterCastle();
			}
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

	castle &= castle_mask[from] & castle_mask[to];

	fifty++;

	if (b[from] == P)
	{
		fifty = 0;
		if (b[to] == EMPTY && col[from] != col[to])
		{
			RemovePiece(xside, P, pawnplus[xside][to]);
		}
		if (b[to] != EMPTY)
		{
			RemovePiece(xside, b[to], to);
		}
		if (rank[side][to] == 7)
		{
			RemovePiece(side, PAWN, from);
			AddPiece(side, Q, to);
			game_list[hply].flags |= PROMOTE;
		}
		else
		{
			UpdatePawn(side, from, to);
		}
	}
	else
	{
		if (b[to] != EMPTY)
		{
			fifty = 0;
			RemovePiece(xside, b[to], to);
		}
		UpdatePiece(side, b[from], from, to);
	}
	ply++;
	hply++;
	side ^= 1;
	xside ^= 1;

	if (Attack(side, pieces[xside][K][0]))
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
		AddPiece(side, P, m->from);
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

	if (b[m->from] == K && abs(m->from - m->to) == 2)
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
	if (b[m->from] == P && m->capture == EMPTY && col[m->from] != col[m->to])
	{
		AddPiece(xside, P, pawnplus[xside][m->to]);
	}
}

int MakeCapture(const int from, const int to, const int flags)
{
	game_list[hply].flags = flags;
	game_list[hply].from = from;
	game_list[hply].to = to;
	game_list[hply].capture = b[to];
	game_list[hply].fifty = 0;
	game_list[hply].hash = currentkey;
	game_list[hply].lock = currentlock;
	++ply;
	++hply;
	if (b[to] != EMPTY)
	{
		RemovePiece(xside, b[to], to);
	}
	else if (b[from] == P && col[from] != col[to])
	{
		RemovePiece(xside, P, pawnplus[xside][to]);
	}

	if (b[from] == P)
	{
		if (rank[side][to] == 7)
		{
			RemovePiece(side, P, from);
			AddPiece(side, Q, to);
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
	if (Attack(side, pieces[xside][K][0]))
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
		AddPiece(side, P, h->from);
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
	if (b[h->from] == P && h->capture == EMPTY && col[h->from] != col[h->to])
	{
		AddPiece(xside, P, pawnplus[xside][h->to]);
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

void BeforeCastle()
{
	KingScore[side][squares[side][F1]] = -60;
	KingScore[side][squares[side][F2]] = -45;
	KingScore[side][squares[side][G1]] = -20;
	PieceScore[side][P][squares[side][E2]] = -8;
	PieceScore[side][B][squares[side][G2]] = 4;
	PieceScore[side][B][squares[side][F1]] = -12;
	PieceScore[side][N][squares[side][G1]] = -20;
	PieceScore[side][P][squares[side][F2]] = 5;
	PieceScore[side][P][squares[side][F3]] = 3;
}

void AfterCastle()
{
	KingScore[side][squares[side][F1]] = -20;
	KingScore[side][squares[side][F2]] = -25;
	KingScore[side][squares[side][G1]] = 20;
	PieceScore[side][P][squares[side][E2]] = 0;
	PieceScore[side][B][squares[side][G2]] = 8;
	PieceScore[side][B][squares[side][F1]] = -10;
	PieceScore[side][N][squares[side][G1]] = -16;
	PieceScore[side][P][squares[side][F2]] = 4;
	PieceScore[side][P][squares[side][F3]] = 4;
}