#include "globals.h"

int GetCurrentDepth();
int GetBest(int ply);
int GetBest2(int ply);

void Alg(int a, int b)
{
	Algebraic(a);
	Algebraic(b);
}

void Alg1(int a)
{
	Algebraic(a);
}

void Alg2(int a, int b)
{
	Algebraic(a);
	printf("X");
	Algebraic(b);
}

void Algebraic(int a)
{
	switch (a)
	{
	case A1: printf("a1"); break;
	case B1: printf("b1"); break;
	case C1: printf("c1"); break;
	case D1: printf("d1"); break;
	case E1: printf("e1"); break;
	case F1: printf("f1"); break;
	case G1: printf("g1"); break;
	case H1: printf("h1"); break;
	case A2: printf("a2"); break;
	case B2: printf("b2"); break;
	case C2: printf("c2"); break;
	case D2: printf("d2"); break;
	case E2: printf("e2"); break;
	case F2: printf("f2"); break;
	case G2: printf("g2"); break;
	case H2: printf("h2"); break;
	case A3: printf("a3"); break;
	case B3: printf("b3"); break;
	case C3: printf("c3"); break;
	case D3: printf("d3"); break;
	case E3: printf("e3"); break;
	case F3: printf("f3"); break;
	case G3: printf("g3"); break;
	case H3: printf("h3"); break;
	case A4: printf("a4"); break;
	case B4: printf("b4"); break;
	case C4: printf("c4"); break;
	case D4: printf("d4"); break;
	case E4: printf("e4"); break;
	case F4: printf("f4"); break;
	case G4: printf("g4"); break;
	case H4: printf("h4"); break;
	case A5: printf("a5"); break;
	case B5: printf("b5"); break;
	case C5: printf("c5"); break;
	case D5: printf("d5"); break;
	case E5: printf("e5"); break;
	case F5: printf("f5"); break;
	case G5: printf("g5"); break;
	case H5: printf("h5"); break;
	case A6: printf("a6"); break;
	case B6: printf("b6"); break;
	case C6: printf("c6"); break;
	case D6: printf("d6"); break;
	case E6: printf("e6"); break;
	case F6: printf("f6"); break;
	case G6: printf("g6"); break;
	case H6: printf("h6"); break;
	case A7: printf("a7"); break;
	case B7: printf("b7"); break;
	case C7: printf("c7"); break;
	case D7: printf("d7"); break;
	case E7: printf("e7"); break;
	case F7: printf("f7"); break;
	case G7: printf("g7"); break;
	case H7: printf("h7"); break;
	case A8: printf("a8"); break;
	case B8: printf("b8"); break;
	case C8: printf("c8"); break;
	case D8: printf("d8"); break;
	case E8: printf("e8"); break;
	case F8: printf("f8"); break;
	case G8: printf("g8"); break;
	case H8: printf("h8"); break;
	default:
		printf("-1"); break;
	}

}

void ShowAll2()
{
	if (nodes < 1)
		return;

	print_board();
	memset(done, 0, sizeof(done));

	printf(" ply ");
	printf("%d", ply);
	printf(" current max ");
	printf("%d", currentmax);
	printf(" currentdepth %d ", GetCurrentDepth());
	printf(" nodes ");
	printf("%d", nodes);
	printf(" side ");
	printf("%d", side);
	printf(" xside ");
	printf("%d", xside);
	printf("\n");
	printf(" alpha %d", Alpha);
	printf(" beta %d \n", Beta);

	for (int z = ply; z > 0; z--)
	{
		if (game_list[hply - z].capture == 6)
			Alg(game_list[hply - z].from, game_list[hply - z].to);
		else
			Alg2(game_list[hply - z].from, game_list[hply - z].to);
		printf(" ");
	}
	printf("\n  order ");
	for (int z = 1; z <= ply; z++)
		printf("%d ", PlyMove[z]);

	printf("\n");
	printf(" extend ");
	for (int x = 1; x <= ply; x++)
	{
		printf(" %d ", extend[x]);
	}
	printf("\n");
	printf(" check ");
	for (int x = 1; x <= ply; x++)
	{
		printf(" %d ", (game_list[x].flags & INCHECK));
	}
	printf("\n");
	printf(" threat ");
	for (int x = 1; x <= ply; x++)
	{
		printf(" %d ", Threat[x]);
	}
	printf("\n");
	printf(" prom ");
	for (int x = 1; x <= ply; x++)
	{
		;//   printf(" %d ",Prom[x]);
	}
	printf("\n");
	_getch();
}

void z()
{
	ShowAll2();
}

void z2()
{
	ShowAll(ply);
}

void p(BITBOARD bb)
{
	PrintBitBoard(bb);
}

void ShowAll(int ply)
{
	move_data* g;
	print_board();
	memset(done, 0, sizeof(done));

	printf(" ply ");
	printf("%d", ply);
	printf(" current max ");
	printf("%d", currentmax);
	printf(" currentdepth %d", GetCurrentDepth());
	printf(" nodes ");
	printf("%d", nodes);
	printf(" side ");
	printf("%d", side);
	printf(" xside ");
	printf("%d", xside);
	printf("\n");
	for (int z = ply; z > 0; z--)
	{
		printf(" %s ", MoveString(game_list[hply - z].from, game_list[hply - z].to, game_list[hply - z].flags));
	}
	printf("\n");

	int j;

	printf(" non-caps \n");
	for (int i = first_move[ply]; i < first_move[ply + 1]; i++)
		//     for(int i=first_move[ply+1];i<first_move[ply + 2];i++)
	{
		j = GetBest(ply);
		{
			//how to display current line?
			g = &move_list[j];
			printf("%s", MoveString(move_list[j].from, move_list[j].to, move_list[j].flags));
			printf(" ");
			printf(" score ");
			printf("%d", g->score);
			printf("\n");
		}
	}
	printf("\n");
	/*
	   for(int i=first_move[ply];i<first_move[ply + 1];i++)//realone
	 //     for(int i=first_move[ply+1];i<first_move[ply + 2];i++)
	   {
		   j = GetBest(ply);
		   {
		   //how to display current line?
		   g = &move_list[j];
		   printf("%s",MoveString(move_list[j]));
		   printf(" ");
		   printf(" score ");
		   printf("%d",g->score);
		   printf("\n");
		   }
	   }
	printf("\n");

	   for(int i=first_move[ply+1];i<first_move[ply + 2];i++)
	 //     for(int i=first_move[ply+1];i<first_move[ply + 2];i++)
	   {
		   //j = GetBest(ply);
		   {
		   //how to display current line?
		   g = &move_list[i];
		   printf("%s",move_str(move_list[i]));
		   printf(" ");
		   printf(" score ");
		   printf("%d",g->score);
		   printf("\n");
		   }
	   }
	 */
	_getch();
}

void ShowMoves(int p)
{
	move_data* g;
	printf("\n");

	int j;
	for (int i = first_move[p]; i < first_move[p + 1]; i++)
	{
		j = GetBest(p);
		{
			//how to display current line?
			g = &move_list[j];
			printf("%s", MoveString(g->from, g->to, g->flags));
			printf(" ");
			printf(" score ");
			printf("%d", g->score);
			printf("\n");
		}
	}
	for (int i = first_move[p]; i < first_move[p + 1]; i++)
	{
		j = GetBest(p);
		{
			//how to display current line?
			g = &move_list[j];
			printf("%s", MoveString(g->from, g->to, g->flags));
			printf(" ");
			printf(" score ");
			printf("%d", g->score);
			printf("\n");
		}
	}
	_getch();
}

int Debug(const int p)
{
	//return 0;
	//*
	for (int x = 0; x < 64; x++)
	{
		if (!(mask[x] & bit_all) && b[x] < 6)
		{
			//Alg(x,x);
			//z();
			//_getch();
		}
	}

	for (int x = 0; x < 64; x++)
	{
		if (b[x] == 6 && mask[x] & bit_all)
		{
			Alg1(x);
			printf(" units place %d ", p);
			z();
			return 0;
		}
	}
	//return 0;

	if (fifty > 200)
	{
		printf(" fifty ");
		z();
	}

	/*
	if(color[pieces[0][5][0]]==1 || color[pieces[1][5][0]]==0)
	{
	  printf(" kingloc bug ");
	  ShowAll2();
	  return 0;
	}
	*/
	int wr = 0, wb = 0, wn = 0;
	for (int x = 0; x < 64; x++)
	{
		if (mask[x] & bit_pieces[0][1])
			wn++;
		if (mask[x] & bit_pieces[0][2])
			wb++;
		if (mask[x] & bit_pieces[0][3])
			wr++;

		if (wn > 2)
		{
			printf("\n 3 knights");
			printf(" place %d ", p);
			z();
		}
		if (wb > 2)
		{
			printf("\n 3 bishops");
			printf(" place %d ", p);
			z();
		}
		if (wr > 2)
		{
			printf("\n 3 rooks");
			printf(" place %d ", p);
			z();
		}
	}
	int knights[2];
	knights[0] = 0;
	knights[1] = 0;
	if (knights[0] > 2 || knights[1] > 2)
	{
		//printf(" bug piecess %d ",pieces[0]);
		//printf(" piecess %d ",pieces[1]);
		printf(" pieces[0][1][0] %d ", pieces[0][1][0]);
		printf(" pieces[0][1][1] %d ", pieces[0][1][1]);
		printf(" pieces[1][1][0] %d ", pieces[1][1][0]);
		printf(" pieces[1][1][1] %d ", pieces[1][1][1]);
		PrintBitBoard(bit_pieces[0][1]);
		PrintBitBoard(bit_pieces[1][1]);
		printf(" place %d ", p);
		ShowAll2();
	}

	if (ply > 1)
		for (int s = 0; s < 2; s++)
		{
			for (int x = 0; x < 2; x++)
			{
				if (total[s][1] > x && b[pieces[s][1][x]] != KNIGHT)
				{
					printf(" bug place %d ", p);
					printf(" pieces[s][1][x] %d ", pieces[s][1][x]);
					printf("debug 4");
					PrintBitBoard(bit_pieces[0][1]);
					ShowAll2();
				}
				if (total[s][2] > x && b[pieces[s][2][x]] != BISHOP)
				{
					printf(" bug bishop ");
					printf(" place %d ", p);
					printf(" x %d ", x);
					printf(" bishop[s][0] %d ", pieces[s][2][0]);
					printf(" bishop[s][1] %d ", pieces[s][2][1]);
					printf(" total %d ", total[s][2]);
					ShowAll2();
				}
				if (total[s][3] > x && b[pieces[s][3][x]] != ROOK)
				{
					printf(" bug rook ");
					printf(" rook[s][x] %d ", pieces[s][3][x]);
					printf(" place %d ", p);
					ShowAll2();
					printf("debug 6");
				}
			}
			if (b[pieces[s][5][0]] != KING)
			{
				printf("bug no king");
				Alg(pieces[0][5][0], pieces[1][5][0]);
				printf(" place %d ", p);
				ShowAll2();
				return 1;
			}
			if (pieces[s][5][0] == -1)
			{
				printf(" bug place %d ", p);
				printf("no kingloc ");
				ShowAll2();
			}
		}
	return 0;
}

int GetBest(int ply)
{
	move_data* g;
	int bestscore = -100000000;
	int best = 0;
	for (int i = 0; i < first_move[ply + 1] - first_move[ply]; i++)
	{
		if (done[i] == 1) continue;
		g = &move_list[first_move[ply] + i];
		if (g->from == 0 && g->to == 0)
			continue;//
		if (g->score > bestscore)
		{
			bestscore = g->score;
			best = i;
		}
	}
	if (best < 1000) done[best] = 1;//1000?
	return first_move[ply] + best;
}

int GetBest2(int ply)
{
	move_data* g;
	int bestscore = -100000000;
	int best = 0;
	for (int i = 0; i < first_move[ply + 1] - first_move[ply]; i++)
	{
		g = &move_list[first_move[ply] + i];
		if (g->score > bestscore)
		{
			bestscore = g->score;
			best = i;
		}
	}
	return first_move[ply] + best;
	//*/
}


