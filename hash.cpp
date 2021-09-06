#include "globals.h"

BITBOARD Random(int x);

void AddBookHash(const int s, const int score, const int type, const move_data move);
int LookUpBook();

int hash_start;
int hash_dest;
int hash_flags;
int hash_score;

void ResetHash();
void Free();

void Alg(int a, int b);

int GetHashEval();

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

const BITBOARD MAXPAWNHASH = 600000;//600000;//1200000//2<<19+1;//65536;//50000;
const BITBOARD MAXHASH = 12000000;//16777216;//20000000;

const int HASH = 23;
const int PAWNHASH = 18;

struct hashp
{
	BITBOARD hashlock;
	int score;
	char depth;
	char type;
	char from;
	char to;
};

hashp* hashpos[2];
BITBOARD hashpositions[2];

struct hashpawn
{
	BITBOARD hashlock;
	int score[2];
	int defence[2][3];
	BITBOARD passed_pawns[2];
	BITBOARD pa[2];
	BITBOARD bp0;
	BITBOARD bp1;
};

hashpawn hashpawns[MAXPAWNHASH];

struct hasheval
{
	BITBOARD hashlock;
	int score;
};

void RandomizeHash()
{
	int p, x;
	for (p = 0; p < 6; p++)
		for (x = 0; x < 64; x++)
		{
			hash[0][p][x] = Random(HASH);
			hash[1][p][x] = Random(HASH);
			lock[0][p][x] = Random(HASH);
			lock[1][p][x] = Random(HASH);
		}
	hashpos[0] = new hashp[MAXHASH];
	hashpos[1] = new hashp[MAXHASH];

	for (x = 0; x < 64; x++)
	{
		pawnhash[0][x] = Random(PAWNHASH);
		pawnhash[1][x] = Random(PAWNHASH);
		pawnlock[0][x] = Random(PAWNHASH);
		pawnlock[1][x] = Random(PAWNHASH);
		ep_hash[x] = Random(HASH);
	}
	//printf(" b1 %d ",sizeof(hashpawns));
	//_getch();
}

BITBOARD Random(int x)
{
	BITBOARD r = 0;
	for (int y = 0; y < x; y++)//23
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

void FreeAllHash()
{
	printf(" free hash/n");
	hashpositions[0] = 0;
	hashpositions[1] = 0;
	for (int x = 0; x < MAXHASH; x++)
		for (int y = 0; y < 2; y++)
		{
			hashpos[y][x].type = 0;
			hashpos[y][x].depth = 0;
			hashpos[y][x].score = 0;
			hashpos[y][x].hashlock = 0;
			//also move
		}
	currentkey = currentlock = 0;
	currentpawnkey = currentpawnlock = 0;
}

void ResetHash()
{
	memset(hashpositions, 0, sizeof(hashpositions));
}

int GetHashScore(const int s)
{
	return hashpos[s][currentkey].score;
}

void AddHash(const int s, const int depth, const int score, const int type, const int from, const int to)
{
	if (hashpos[s][currentkey].hashlock == 0)
	{
		hashpos[s][currentkey].hashlock = currentlock;
		hashpos[s][currentkey].score = score;
		hashpos[s][currentkey].depth = depth;
		hashpos[s][currentkey].type = type;
		hashpos[s][currentkey].from = from;
		hashpos[s][currentkey].to = to;

		hashpositions[s]++;

		return;
	}

	if (hashpos[s][currentkey].hashlock == currentlock)
	{
		//if (hashpos[s][currentkey].type == 4)//book move
		//	return;
		if (depth < hashpos[s][currentkey].depth)
			return;
		hashpos[s][currentkey].hashlock = currentlock;
		hashpos[s][currentkey].score = score;
		hashpos[s][currentkey].depth = depth;
		hashpos[s][currentkey].type = type;
		hashpos[s][currentkey].from = from;
		hashpos[s][currentkey].to = to;
	}
	else
		//position already stored
	{
		collisions++;

		hashpos[s][currentkey].hashlock = currentlock;
		hashpos[s][currentkey].score = score;
		hashpos[s][currentkey].depth = depth;
		hashpos[s][currentkey].type = type;
		hashpos[s][currentkey].from = from;
		hashpos[s][currentkey].to = to;
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
		if (b[x] != 6)
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
		if (b[x] != 6)
		{
			if (bit_units[0] & mask[x])
			key ^= hash[0][b[x]][x];
			if (bit_units[1] & mask[x]) 
			key ^= hash[1][b[x]][x];
		}
	}
	return key;
}

int LookUpBook(const int s)
{
	if (hashpos[s][currentkey].hashlock == currentlock && hashpos[s][currentkey].type == 4)
	{
		hash_start = hashpos[s][currentkey].from;
		hash_dest = hashpos[s][currentkey].to;
		return 1;
	}
	hash_start = 0;
	hash_dest = 0;
	hash_flags = 0;
	return 0;
}

int LookUp(const int s, const int depth, const int alpha, const int beta)
{
	if (hashpos[s][currentkey].hashlock != currentlock)
	{
		return -1;
	}
	hash_start = hashpos[s][currentkey].from;
	hash_dest = hashpos[s][currentkey].to;
	//hash_flags = hashpos[s][currentkey].

	if (depth > hashpos[s][currentkey].depth)
	{
		if (depth == hashpos[s][currentkey].depth + 1)
		{
			if (hashpos[s][currentkey].type == BETA && hashpos[s][currentkey].score >= beta)
				return(BETA);
			if (hashpos[s][currentkey].type == ALPHA && hashpos[s][currentkey].score <= alpha) 
			{
				return(ALPHA);
			}
		}
		return 0;
	}

	int type = hashpos[s][currentkey].type;

	if (type == BETA)
	{
		if (hashpos[s][currentkey].score >= beta)
			return(BETA);
		if (hashpos[s][currentkey].score >= 9900)
			return(BETA);
		return 0;
	}
	else if (type == ALPHA)
	{
		if (hashpos[s][currentkey].score <= alpha)
			return(ALPHA);
		return 0;
	}
	else if (type == EXACT)
	{
		hash_score = hashpos[s][currentkey].score;
		return(EXACT);
	}
	return 0;
}

int LookUp2(const int s)
{
	if (hashpos[s][currentkey].hashlock != currentlock)
	{
		return -1;
	}
	hash_score = hashpos[s][currentkey].score;
	hash_start = hashpos[s][currentkey].from;
	hash_dest = hashpos[s][currentkey].to;
	return 0;
}

void AddBookHash(const int s, const int score, const int type, move_data move)
{
	BITBOARD key = currentkey;

	if (hashpos[s][key].score == 0)//hash entry is empty
	{
		hashpos[s][key].hashlock = currentlock;
		hashpos[s][key].score = score;
		hashpos[s][key].depth = 0;
		hashpos[s][key].type = 4;
		hashpos[s][key].from = move.from;
		hashpos[s][key].to = move.to;

		hashpositions[s]++;

		return;
	}

	if (hashpos[s][key].hashlock == currentlock)
	{
		if (Random(100) > hashpos[s][key].score)
		{
			hashpos[s][key].hashlock = currentlock;
			hashpos[s][key].score = score;
			hashpos[s][key].depth = 0;
			hashpos[s][key].type = type;
			hashpos[s][key].from = move.from;
			hashpos[s][key].to = move.to;
		}
	}
	else
		//position already stored
	{
		//if(score > hashpos[s][key].score)
		if (Random(100) > hashpos[s][key].score)
		{
			hashpos[s][key].hashlock = currentlock;
			hashpos[s][key].score = score;
			hashpos[s][key].depth = 0;
			hashpos[s][key].type = type;
			hashpos[s][key].from = move.from;
			hashpos[s][key].to = move.to;
		}
		collisions++;
	}
	return;
}

BITBOARD GetPawnKey()
{
	int c=0;
	BITBOARD key = 0;
	for (int x = 0; x < 64; x++)
	{
		if (b[x] != 6)
		{
			if (bit_units[0] & mask[x]) c = 0;
			if (bit_units[1] & mask[x]) c = 1;
			key ^= pawnhash[c][x];
		}
	}
	return key;
}

BITBOARD GetPawnLock()
{
	int c=0;
	BITBOARD key = 0;
	for (int x = 0; x < 64; x++)
	{
		if (b[x] != 6)
		{
			if (bit_units[0] & mask[x]) c = 0;
			if (bit_units[1] & mask[x]) c = 1;
			key ^= pawnlock[c][x];
		}
	}
	return key;
}

void AddPawnHash(const int s1, const int s2, const BITBOARD p1, const BITBOARD p2)
{
	/*
	if(hashpawns[currentpawnkey].hashlock!=currentpawnlock && hashpawns[currentpawnkey].hashlock!=0)
		pcollisions++;
	else
		collisions++;
hashpawns[currentpawnkey].hashlock=currentpawnlock;
*/
	hashpawns[currentpawnkey].score[0] = s1;
	hashpawns[currentpawnkey].score[1] = s2;

	hashpawns[currentpawnkey].passed_pawns[0] = p1;
	hashpawns[currentpawnkey].passed_pawns[1] = p2;
	return;
}

bool LookUpPawn()
{
	if (hashpawns[currentpawnkey].hashlock == currentpawnlock)
	{
		return true;
	}
	return false;
}

int GetHashPawn0()
{
	return hashpawns[currentpawnkey].score[0];
}

int GetHashPawn1()
{
	return hashpawns[currentpawnkey].score[1];
}

int GetHashDefence(const int s, const int n)
{
	return hashpawns[currentpawnkey].defence[s][n];
}

int GetHashQueenside0()
{
	return hashpawns[currentpawnkey].defence[0][0];
}

int GetHashQueenside1()
{
	return hashpawns[currentpawnkey].defence[1][0];
}

int GetHashKingside(const int s)
{
	return hashpawns[currentpawnkey].defence[s][1];
}

int GetHashKingside0()
{
	return hashpawns[currentpawnkey].defence[0][1];
}

int GetHashKingside1()
{
	return hashpawns[currentpawnkey].defence[1][1];
}

BITBOARD GetHashPassed(const int s)
{
	return hashpawns[currentpawnkey].passed_pawns[s];
}

BITBOARD GetHashPassed0()
{
	return hashpawns[currentpawnkey].passed_pawns[0];
}

BITBOARD GetHashPassed1()
{
	return hashpawns[currentpawnkey].passed_pawns[1];
}

BITBOARD GetHashbp0()
{
	return hashpawns[currentpawnkey].bp0;
}

BITBOARD GetHashbp1()
{
	return hashpawns[currentpawnkey].bp1;
}

void AddHashbp0(const BITBOARD bp)
{
	hashpawns[currentpawnkey].bp0 = bp;
}

void AddHashbp1(const BITBOARD bp)
{
	hashpawns[currentpawnkey].bp1 = bp;
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

void AddKingHash(const int s, const int value)
{
	hashpawns[currentpawnkey].defence[s][1] = value;
}

void AddQueenHash(const int s, const int value)
{
	hashpawns[currentpawnkey].defence[s][0] = value;
}
/*
void AddCentreHash()
{
  hashpawns[currentpawnkey].defence[0][2] = 0;
}
*/

BITBOARD GetHashPawnAttacks(const int s)
{
	return hashpawns[currentpawnkey].pa[s];
}

void AddPawnAttackHash(const int s, const int value)
{
	hashpawns[currentpawnkey].pa[s] = value;
}