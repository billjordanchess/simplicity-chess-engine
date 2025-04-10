#include "globals.h"

void HashTest();

move_data hash_move;

void Free();
void Alg(int a, int b);
void SetBit(BITBOARD& bb, int square);

//Hash table with 2^16 =64000
BITBOARD hash[2][6][64];
BITBOARD lock[2][6][64];
BITBOARD ep_hash[64];

BITBOARD currentkey, currentlock;
BITBOARD currentpawnkey, currentpawnlock;

BITBOARD pawnhash[2][64];
BITBOARD pawnlock[2][64];

BITBOARD clashes, bestmatches, collisions, pcollisions;

BITBOARD Random(int x);

void AddBookHash(const int s, const int score, const int type, const move_data move);
int LookUpBook();
move_data GetHashMove();

const int HASH_SIZE = 23;
const int PAWNHASH_SIZE = 18;

const BITBOARD MAXPAWNHASH = 2 << PAWNHASH_SIZE;
const BITBOARD MAXHASH = 2 << HASH_SIZE;

struct hashp
{
	BITBOARD hashlock;
	int score;
	char depth;
	char type;
	char from;
	char to;
	char age;
	unsigned int flags;
};

hashp* hashpos[2];
BITBOARD hashpositions[2];

struct hashpawn
{
	BITBOARD hashlock;
	int score[2];
	int defence[2][3];
	BITBOARD passed_pawns[2];
	BITBOARD pawn_attacks[2];
};

hashpawn hashpawns[MAXPAWNHASH];

void RandomizeHash()
{
	int p, x;
	for (p = 0; p < 6; p++)
		for (x = 0; x < 64; x++)
		{
			hash[0][p][x] = Random(HASH_SIZE);
			hash[1][p][x] = Random(HASH_SIZE);
			lock[0][p][x] = Random(HASH_SIZE);
			lock[1][p][x] = Random(HASH_SIZE);
		}
	hashpos[0] = new hashp[MAXHASH];
	hashpos[1] = new hashp[MAXHASH];

	for (x = 0; x < 64; x++)
	{
		pawnhash[0][x] = Random(PAWNHASH_SIZE);
		pawnhash[1][x] = Random(PAWNHASH_SIZE);
		pawnlock[0][x] = Random(PAWNHASH_SIZE);
		pawnlock[1][x] = Random(PAWNHASH_SIZE);
		ep_hash[x] = Random(HASH_SIZE);
	}
}

BITBOARD Random(int size)
{
	BITBOARD r = 0;
	for (int y = 0; y < size; y++)
	{
		if (rand() % 100 < 50)
			SetBit(r, y);
	}
	return r;
}

void Free()
{
	delete hashpos[0];
	delete hashpos[1];
}

int GetHashScore(const int s)
{
	return hashpos[s][currentkey].score;
}

void AddHash(const int s, const int depth, const int score, const int type, const int from, const int to, const unsigned int flags)
{
	hashp* ptr = &hashpos[s][currentkey];
	if (ptr->hashlock == 0)
	{
		ptr->hashlock = currentlock;
		ptr->score = score;
		ptr->depth = depth;
		ptr->type = type;
		ptr->from = from;
		ptr->to = to;
		ptr->age = turn;
		ptr->flags = flags;

		hashpositions[s]++;

		return;
	}

	if (ptr->hashlock == currentlock)
	{
		if (depth < ptr->depth)
			return;
		ptr->score = score;
		ptr->depth = depth;
		ptr->type = type;
		ptr->from = from;
		ptr->to = to;
		ptr->age = turn;
		ptr->flags = flags;
	}
	else
	{
		collisions++;
		if (ptr->type == EXACT && type != EXACT && ptr->age == turn)
		{
			return;
		}

		ptr->hashlock = currentlock;
		ptr->score = score;
		ptr->depth = depth;
		ptr->type = type;
		ptr->from = from;
		ptr->to = to;
		ptr->age = turn;
		ptr->flags = flags;
	}
	return;
}

void AddKey(const int s, const int p, const int x)
{
	currentkey ^= hash[s][p][x];
	currentlock ^= lock[s][p][x];
}

void AddKeys(const int s, const int p, const int x, const int y)
{
	currentkey ^= hash[s][p][x];
	currentkey ^= hash[s][p][y];
	currentlock ^= lock[s][p][x];
	currentlock ^= lock[s][p][y];
}

BITBOARD GetLock()
{
	BITBOARD loc = 0;
	for (int x = 0; x < 64; x++)
	{
		if (b[x] != EMPTY)
		{
			if (bit_units[0] & mask[x])
				loc ^= lock[0][b[x]][x];
			if (bit_units[1] & mask[x])
				loc ^= lock[1][b[x]][x];
		}
	}
	return loc;
}

BITBOARD GetKey()
{
	BITBOARD key = 0;
	for (int x = 0; x < 64; x++)
	{
		if (b[x] != EMPTY)
		{
			if (bit_units[0] & mask[x])
				key ^= hash[0][b[x]][x];
			if (bit_units[1] & mask[x])
				key ^= hash[1][b[x]][x];
		}
	}
	return key;
}

int LookUp(const int s, const int depth, const int alpha, const int beta)
{
	hashp* ptr = &hashpos[s][currentkey];
	if (ptr->hashlock != currentlock)
	{
		return -1;
	}
	hash_move.from = ptr->from;
	hash_move.to = ptr->to;
	hash_move.flags = ptr->flags;

	if (depth > ptr->depth)
	{
		if (depth == ptr->depth + 1)
		{
			if (ptr->type == BETA && ptr->score >= beta)
				return(BETA);
			if (ptr->type == ALPHA && ptr->score <= alpha)
			{
				return(ALPHA);
			}
		}
		return 0;
	}
	if (ptr->type == BETA)
	{
		if (ptr->score >= beta)
			return(BETA);
		if (ptr->score >= 9900)
			return(BETA);
		return 0;
	}
	else if (ptr->type == ALPHA)
	{
		if (ptr->score <= alpha)
			return(ALPHA);
		return 0;
	}
	else if (ptr->type == EXACT)
	{
		hash_move.score = ptr->score;
		return(EXACT);
	}
	return 0;
}

int LookUp2(const int s)
{
	hashp* ptr = &hashpos[s][currentkey];
	if (ptr->hashlock != currentlock)
	{
		return -1;
	}
	hash_move.score = ptr->score;
	hash_move.from = ptr->from;
	hash_move.to = ptr->to;
	hash_move.flags = ptr->flags;
	return 0;
}

BITBOARD GetPawnKey()
{
	int colour = 0;
	BITBOARD key = 0;
	for (int x = 0; x < 64; x++)
	{
		if (b[x] != EMPTY)
		{
			if (bit_units[0] & mask[x]) colour = 0;
			if (bit_units[1] & mask[x]) colour = 1;
			key ^= pawnhash[colour][x];
		}
	}
	return key;
}

BITBOARD GetPawnLock()
{
	int colour = 0;
	BITBOARD key = 0;
	for (int x = 0; x < 64; x++)
	{
		if (b[x] != EMPTY)
		{
			if (bit_units[0] & mask[x]) colour = 0;
			if (bit_units[1] & mask[x]) colour = 1;
			key ^= pawnlock[colour][x];
		}
	}
	return key;
}

void AddPawnHash(const int s1, const int s2, const BITBOARD p1, const BITBOARD p2)
{
	hashpawns[currentpawnkey].score[0] = s1;
	hashpawns[currentpawnkey].score[1] = s2;

	hashpawns[currentpawnkey].passed_pawns[0] = p1;
	hashpawns[currentpawnkey].passed_pawns[1] = p2;
	return;
}

void AddPawnKey(const int s, const int x)
{
	currentpawnkey ^= pawnhash[s][x];
	currentpawnlock ^= pawnlock[s][x];
}

void AddPawnKeys(const int s, const int x, const int y)
{
	currentpawnkey ^= pawnhash[s][x];
	currentpawnkey ^= pawnhash[s][y];
	currentpawnlock ^= pawnlock[s][x];
	currentpawnlock ^= pawnlock[s][y];
}

bool LookUpPawn()
{
	if (hashpawns[currentpawnkey].hashlock == currentpawnlock)
	{
		return true;
	}
	return false;
}

int GetHashPawn(const int s)
{
	return hashpawns[currentpawnkey].score[s];
}

BITBOARD GetHashPassed(const int s)
{
	return hashpawns[currentpawnkey].passed_pawns[s];
}

int GetHashDefence(const int s, const int board_side)
{
	return hashpawns[currentpawnkey].defence[s][board_side];
}

void AddKingHash(const int s, const int value)
{
	hashpawns[currentpawnkey].defence[s][1] = value;
}

void AddQueenHash(const int s, const int value)
{
	hashpawns[currentpawnkey].defence[s][0] = value;
}

void AddPawnAttackHash(const int s, const BITBOARD value)
{
	hashpawns[currentpawnkey].pawn_attacks[s] = value;
}

BITBOARD GetHashPawnAttacks(const int s)
{
	return hashpawns[currentpawnkey].pawn_attacks[s];
}

move_data GetHashMove()
{
	return hash_move;
}

void HashTest()
{
	//*
if(//hashpos[1][6495712].from == D5 && hashpos[1][6495712].to==C4 ||
hashpos[1][5690949].from == E8 && hashpos[1][2610156].to==D8 ||
hashpos[1][6495712].from == D5 && hashpos[1][6495712].to==C4
)
{
	Alg(hashpos[1][396057].from,hashpos[1][1394300].to);
	z();
	//ShowAll(ply);
	nodes=nodes;
}
//*/
}
