#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*

The objective of this project is to emulate the mini-game triple triad from final fantasy 8 (PC, PS1)

It consists in a card game for two players, each with five cards, playing them one at a time, switching turns, on a board with 9 slots disposed on a grid 3x3.

https://www.youtube.com/watch?v=LBYtH97AvyA

https://www.onlinegdb.com/online_c_compiler

https://www.youtube.com/watch?v=fXADMoL8wbU simple game

The cards are rectangular and have 4 numeric values, each value associated with each side.

Wins the game the player who ends up with most cards. There are several ways to capture the adversary card's.

 and develop a function to return which is the best play in a given scenario.

Although simple in the beginning, this game add some complexity later on, and became really challenging. 



- Rotina pra imprimir as cartas da mão dos jogadores - ok
- Testar rotina de carregar cartas - ok
- Fazer menu e configurar opções - ok
- Refatorar a LoadCardsAuto para receber uma array - ok
- Refatorar a funcao LoadCards para usar a LoadCardsAuto - ok
- Refatorar prints para receber passagem por valor - ok
- Criar funcao de "fazer jogada" - ok
- Ajustar EvaluateCardPlay para contemplar as regras Same, Plus, SameWall
- Funcao de calcular placar - ok
- Funcao print Game envolvendo printBoard, printCardHand e placar - ok
- Substituir menções a eixos X e Y por linhas e colunas
- Passagem de slot por ID

    Round	Cards On Hand	Spots Available	Possible Plays	Aggregated Possibilities	Possible Scenarios
        1	            5	              9	            45	                      45	        5225472000
        2	            5	              8	            40	                    1800	         116121600
        3	            4	              7	            28	                   50400	           2903040
        4	            4	              6	            24	                 1209600	            103680
        5	            3	              5	            15	                18144000	              4320
        6	            3	              4	            12	               217728000	               288
        7	            2	              3	             6	              1306368000	                24
        8	            2	              2	             4	              5225472000	                 4
        9	            1	              1	             1	              5225472000	                 1

	
	Elements
	
		N = Neutral
		L = Lightning
		E = Earth
		I = Ice
		W = Wind
		P = Poison
		F = Fire
		A = Water
		H = Holy
*/

//strings to change the color of the text
const char colorReset[] = "\033[0m",
		   colorBlue[] = "\033[0;36m",
		   colorRed[] = "\033[0;31m";

typedef struct Card
{
	long int Id;
	int Level,
	    Order;
	char Name[16];
	int UpValue,
	    RightValue,
	    DownValue,
	    LeftValue;
	char Element,
	     Color;

} Card;

typedef struct Slot
{
	int Id,
		UpValue,
        RightValue,
        DownValue,
        LeftValue;
	char Element,
         Occupied;
	Card *Card;

} Slot;

typedef struct Board
{
    Slot Slot[3][3];
	
} Board;

typedef struct Player
{
    Card CardsHand[5];
    char CardsAvailable[5],
	     Color,
		 *Name;
	const char *TxtColor;

} Player;

typedef struct Score
{
    int Blue, 
		Red;
    char outcome;
} Score;

typedef struct Rules
{
	char 	Same,
			Plus,
		 	SameWall,
		 	Elemental;
} Rules;

typedef struct Game
{
    Player Player[2];
    Board Board;
    Score Score;
    Rules Rules;
	Player *PlayerTurn;
    int Round;

} Game;

typedef struct AffectedSlot
{
	Slot *Slot;
	int SubtractionFieldValue,
		SubtractionCardValue,
		Sum;
	char Valid,
	     SameWallTrigger,
		 Turned;

} AffectedSlot;

typedef struct AffectedSlots
{
	AffectedSlot AffSlot[4];
	char Combo,
		 SameWallEvent;

} AffectedSlots;

Card CardList[] =
{
	{1335,       1,     3,    "Bite Bug",         1,    3,    3,    5,    'N'},
	{1415,       1,     1,    "Geezard",          1,    4,    1,    5,    'N'},
	{1541,       1,     7,    "Gesper",           1,    5,    4,    1,    'N'},
	{1647,       4,     38,   "Blitz",            1,    6,    4,    7,    'L'},
	{1848,       6,     64,   "Oilboyle",         1,    8,    4,    8,    'N'},
	{1877,       7,     76,   "Catoblepas",       1,    8,    7,    7,    'N'},
	{1883,       6,     60,   "Gerogero",         1,    8,    8,    3,    'P'},
	{2126,       1,     11,   "Cockatrice",       2,    1,    2,    6,    'N'},
	{2144,       1,     6,    "Gayla",            2,    1,    4,    4,    'L'},
	{2161,       1,     9,    "Blood Soul",       2,    1,    6,    1,    'N'},
	{2315,       1,     5,    "Blobra",           2,    3,    1,    5,    'N'},
	{2367,       4,     34,   "Turtapod",         2,    3,    6,    7,    'N'},
	{2763,       4,     37,   "Bomb",             2,    7,    6,    3,    'F'},
	{2884,       6,     56,   "Fujin & Raijin",   2,    8,    8,    4,    'N'},
	{2994,       8,     83,   "Quetzacotl",       2,    9,    9,    4,    'L'},
	{3217,       2,     21,   "Jelleye",          3,    2,    1,    7,    'L'},
	{3453,       2,     16,   "Belhelmel",        3,    4,    5,    3,    'N'},
	{3521,       1,     8,    "Fastitocalon-F",   3,    5,    2,    1,    'E'},
	{3555,       3,     25,   "Tri-Face",         3,    5,    5,    5,    'P'},
	{3644,       3,     32,   "Tonberry",         3,    6,    4,    4,    'N'},
	{3657,       5,     46,   "Behemoth",         3,    6,    5,    7,    'N'},
	{3736,       4,     41,   "Imp",              3,    7,    3,    6,    'N'},
	{3796,       8,     80,   "Gilgamesh",        3,    7,    9,    6,    'N'},
	{4243,       1,     10,   "Caterchipillar",   4,    2,    4,    3,    'N'},
	{4452,       2,     20,   "Grendel",          4,    4,    5,    2,    'L'},
	{4472,       3,     30,   "Death Claw",       4,    4,    7,    2,    'F'},
	{4489,       8,     78,   "Chubby Chocobo",   4,    4,    8,    9,    'N'},
	{4556,       4,     43,   "Adamantoise",      4,    5,    5,    6,    'E'},
	{4627,       4,     36,   "T-Rexaur",         4,    6,    2,    7,    'E'},
	{4674,       5,     54,   "Tonberry King",    4,    6,    7,    4,    'N'},
	{4856,       6,     63,   "Trauma",           4,    8,    5,    6,    'N'},
	{4873,       6,     58,   "X-ATM092",         4,    8,    7,    3,    'N'},
	{5113,       1,     2,    "Funguar",          5,    1,    1,    3,    'N'},
	{5135,       2,     18,   "Anacondaur",       5,    1,    3,    5,    'N'},
	{5199,       8,     87,   "Sacred",           5,    1,    9,    9,    'E'},
	{5252,       2,     19,   "Creeps",           5,    2,    5,    2,    'L'},
	{5253,       2,     22,   "Grand Mantis",     5,    2,    5,    3,    'N'},
	{5325,       2,     17,   "Thrustaevis",      5,    3,    2,    5,    'W'},
	{5334,       2,     14,   "Mesmerize",        5,    3,    3,    4,    'N'},
	{5376,       5,     53,   "Elnoyle",          5,    3,    7,    6,    'N'},
	{5574,       5,     50,   "GIM47N",           5,    5,    7,    4,    'N'},
	{5624,       3,     29,   "SAM08G",           5,    6,    2,    4,    'F'},
	{5633,       3,     28,   "Ochu",             5,    6,    3,    3,    'N'},
	{5668,       7,     70,   "Gargantua",        5,    6,    6,    8,    'N'},
	{5785,       7,     74,   "BGH251F2",         5,    7,    8,    5,    'N'},
	{6112,       1,     4,    "Red Bat",          6,    1,    1,    2,    'N'},
	{6143,       2,     15,   "Glacial Eye",      6,    1,    4,    3,    'I'},
	{6223,       2,     13,   "Buel",             6,    2,    2,    3,    'N'},
	{6263,       3,     31,   "Cactuar",          6,    2,    6,    3,    'N'},
	{6267,       5,     49,   "Elastoid",         6,    2,    6,    7,    'N'},
	{6273,       4,     42,   "Blue Dragon",      6,    2,    7,    3,    'P'},
	{6316,       3,     24,   "Armadodo",         6,    3,    1,    6,    'E'},
	{6545,       4,     35,   "Vysage",           6,    5,    4,    5,    'N'},
	{6565,       5,     45,   "Iron Giant",       6,    5,    6,    5,    'N'},
	{6584,       6,     65,   "Shumi Tribe",      6,    5,    8,    4,    'N'},
	{6627,       5,     55,   "Wedge & Biggs",    6,    6,    2,    7,    'N'},
	{6632,       3,     23,   "Forbidden",        6,    6,    3,    2,    'N'},
	{6749,       8,     84,   "Shiva",            6,    7,    4,    9,    'I'},
	{6845,       6,     62,   "Abadon",           6,    8,    4,    5,    'N'},
	{6847,       7,     75,   "Red Giant",        6,    8,    4,    7,    'N'},
	{7131,       2,     12,   "Grat",             7,    1,    3,    1,    'N'},
	{7153,       3,     27,   "Snow Lion",        7,    1,    5,    3,    'I'},
	{7235,       3,     33,   "Abyss Worm",       7,    2,    3,    5,    'E'},
	{7274,       5,     52,   "Ruby Dragon",      7,    2,    7,    4,    'F'},
	{7285,       6,     59,   "Granaldo",         7,    2,    8,    5,    'N'},
	{7316,       4,     39,   "Wendigo",          7,    3,    1,    6,    'N'},
	{7444,       4,     40,   "Torama",           7,    4,    4,    4,    'N'},
	{7513,       3,     26,   "Fastitocalon",     7,    5,    1,    3,    'E'},
	{7543,       4,     44,   "Hexadragon",       7,    5,    4,    3,    'F'},
	{7581,       6,     66,   "Krysta",           7,    5,    8,    1,    'N'},
	{7653,       5,     47,   "Chimera",          7,    6,    5,    3,    'A'},
	{7728,       7,     77,   "Ultima Weapon",    7,    7,    2,    8,    'N'},
	{7742,       5,     51,   "Malboro",          7,    7,    4,    2,    'P'},
	{7834,       6,     57,   "Elvoret",          7,    8,    3,    4,    'W'},
	{8282,       6,     61,   "Iguion",           8,    2,    8,    2,    'N'},
	{8358,       7,     72,   "Sphinxaur",        8,    3,    5,    8,    'N'},
	{8448,       7,     67,   "Propagator",       8,    4,    4,    8,    'N'},
	{8528,       7,     69,   "Tri-Point",        8,    5,    2,    8,    'L'},
	{8673,       7,     71,   "Mobile Type 8",    8,    6,    7,    3,    'N'},
	{8844,       7,     68,   "Jumbo Cactuar",    8,    8,    4,    4,    'N'},
	{8854,       7,     73,   "Tiamat",           8,    8,    5,    4,    'N'},
	{8962,       8,     86,   "Siren",            8,    9,    6,    2,    'N'},
	{9392,       8,     81,   "MiniMog",          9,    3,    9,    2,    'N'},
	{9484,       8,     82,   "Chicobo",          9,    4,    8,    4,    'N'},
	{9529,       8,     88,   "Minotaur",         9,    5,    2,    9,    'E'},
	{9628,       8,     85,   "Ifrit",            9,    6,    2,    8,    'F'},
	{9673,       8,     79,   "Angelo",           9,    6,    7,    3,    'N'},
	{10177,      9,     93,   "Pandemona",       10,    1,    7,    7,    'W'},
	{10469,     10,     110,  "Squall",          10,    4,    6,    9,    'N'},
	{10728,     10,     100,  "Ward",            10,    7,    2,    8,    'N'},
	{10826,      9,     97,   "Bahamut",         10,    8,    2,    6,    'N'},
	{10864,     10,     103,  "Selphie",         10,    8,    6,    4,    'N'},
	{26910,     10,     105,  "Irvine",           2,    6,    9,   10,    'N'},
	{31021,      5,     48,   "PuPu",             3,   10,    2,    1,    'N'},
	{44910,      9,     99,   "Eden",             4,    4,    9,   10,    'N'},
	{51039,     10,     102,  "Laguna",           5,   10,    3,    9,    'N'},
	{51083,      9,     90,   "Diablos",          5,   10,    8,    3,    'N'},
	{67610,     10,     101,  "Kiros",            6,    7,    6,   10,    'N'},
	{69104,     10,     109,  "Seifer",           6,    9,   10,    4,    'N'},
	{71017,      9,     91,   "Leviathan",        7,   10,    1,    7,    'A'},
	{72710,      9,     96,   "Phoenix",          7,    2,    7,   10,    'F'},
	{74610,      9,     94,   "Cerberus",         7,    4,    6,   10,    'N'},
	{81035,      9,     92,   "Odin",             8,   10,    3,    5,    'N'},
	{84104,      9,     89,   "Carbuncle",        8,    4,   10,    4,    'N'},
	{85106,     10,     106,  "Zell",             8,    5,   10,    6,    'N'},
	{91042,      9,     95,   "Alexander",        9,   10,    4,    2,    'H'},
	{96102,     10,     104,  "Quistis",          9,    6,   10,    2,    'N'},
	{101033,    10,     108,  "Edea",            10,   10,    3,    3,    'N'},
	{311010,     9,     98,   "Doomtrain",        3,    1,   10,   10,    'P'},
	{410210,    10,     107,  "Rinoa",            4,   10,    2,   10,    'N'}
};

int binarySearch(Card arr[], int l, int r, int x) 
{ 
    while (l <= r) { 
        int m = l + (r - l) / 2; 
  
        // Check if x is present at mid 
        if (arr[m].Id == x) 
            return m; 
  
        // If x greater, ignore left half 
        if (arr[m].Id < x) 
            l = m + 1; 
  
        // If x is smaller, ignore right half 
        else
            r = m - 1; 
    } 
  
    // if we reach here, then element was 
    // not present 
    return -1; 
}

void SetRules (Rules *rules)
{
	printf("\nSetting the game's rules: \n");

	while( getchar() != '\n' );
	printf("\nEnable *Elemental* rule? (Y/N)");
	scanf("%c", &rules->Elemental);

	while( getchar() != '\n' );
	printf("\nEnable *Plus* rule? (Y/N)");
	scanf("%c", &rules->Plus);

	while( getchar() != '\n' );
	printf("\nEnable *Same* rule? (Y/N)");
	scanf("%c", &rules->Same);

	if(rules->Same == 'Y')
	{
		while( getchar() != '\n' );
		printf("\nEnable *Same Wall* rule? (Y/N)");
		scanf("%c", &rules->SameWall);
	}
	else
	{
		rules->SameWall = 'N';
	}
}

Card RetrieveCard (long int ID)
{
	return CardList[binarySearch(CardList, 0, 110, ID)];
}

void ShowAllCards()
{
	int i, j = 0;
	printf("Order - Level -    ID    - NAME               --- UP | RG | DN | LF | EL \n");

	for(i = 1; i <= 110; i++)
	{
		while(CardList[j].Order != i)
			j++;
		
		printf("  %3i     %3i %7ld    - %-18s --- %2i | %2i | %2i | %2i | %2c\n", 	CardList[j].Order,
																					CardList[j].Level,
																					CardList[j].Id,
																					CardList[j].Name,
																					CardList[j].UpValue,
																					CardList[j].RightValue,
																					CardList[j].DownValue,
																					CardList[j].LeftValue,
																					CardList[j].Element
		);
		j = 0;
	}
}

void LoadCardsAuto(Player *player, long int cards[])
{
	int i;

	for(i = 0; i < 5; i++)
    {
        player->CardsHand[i] = RetrieveCard(cards[i]);
		player->CardsHand[i].Color = player->Color;
		player->CardsAvailable[i] = 'Y';
    }

}

void LoadCards(Player *player)
{
	long int cardIds[5];
	int i, cardsNotValid = 1;

	while(cardsNotValid)
	{
		printf("\n%sLoading cards for %s player\nEnter 5 card's IDs, separated by spaces: %s\n", player->TxtColor, player->Name, colorReset);
		scanf("%ld %ld %ld %ld %ld", &cardIds[0], &cardIds[1], &cardIds[2], &cardIds[3], &cardIds[4]);
		
		cardsNotValid = 0;

		for(i = 0; i < 5; i++)
		{
			if(binarySearch(CardList, 0, 110, cardIds[i]) == -1)
			{
				cardsNotValid = 1;
				printf("\n%sCard ID %ld does not exist. Please enter 5 valid card's ID.%s\n", player->TxtColor, cardIds[i], colorReset);
			}
		}		
	}

	LoadCardsAuto(player, cardIds);
}

void PrintBoard(Board Board)
{
	int i = 0, j = 0, k = 0;
	char Linha[7][151], aux[34] = "";
	printf("\n");
	
	for(k = 0; k < 7; k++)
		Linha[k][0] = '\0';

	for(i = 0; i < 3; i++)
	{
		for(j = 0; j < 3; j++)
		{
			if(Board.Slot[i][j].Occupied == 'Y' && Board.Slot[i][j].Element == 'N')
			{
				//set color
				if(Board.Slot[i][j].Card->Color == 'R')
				{
					for(k = 0; k < 7; k++)
						strcat(Linha[k], colorRed);
				} 
				else 
				{
					for(k = 0; k < 7; k++)
						strcat(Linha[k], colorBlue);
				}
				
				//                    1 234567891123456789212345678931
				sprintf(aux,   		"%i ------------------------------|", 	i * 3 + j + 1);
				strcat(Linha[0], aux);
				sprintf(aux,   		"|               %-2i              |", 	Board.Slot[i][j].Card->UpValue);
				strcat(Linha[1], aux);
				strcat(Linha[2], 	"|                               |");
				sprintf(aux,   		"|%-2i     %-14s[%1c]     %2i|", 		Board.Slot[i][j].Card->LeftValue,
																			Board.Slot[i][j].Card->Name,
																			Board.Slot[i][j].Card->Element,
																			Board.Slot[i][j].Card->RightValue);
				strcat(Linha[3], aux);
				strcat(Linha[4], 	"|                               |");
				sprintf(aux,   		"|               %-2i              |", 	Board.Slot[i][j].Card->DownValue);
				strcat(Linha[5], aux);
				strcat(Linha[6], 	"|-------------------------------|");
				
				for(k = 0; k < 7; k++)
					strcat(Linha[k], colorReset);
			}
			else if(Board.Slot[i][j].Occupied == 'N' && Board.Slot[i][j].Element == 'N')
			{
				//                    1 234567891123456789212345678931
				sprintf(aux,   		"%i ------------------------------|", 	i * 3 + j + 1);
				strcat(Linha[0], aux);
				for(k = 1; k < 6; k++)
						strcat(Linha[k], 	"|                               |");
				strcat(Linha[6], 			"|-------------------------------|");
			
			}
			else if(Board.Slot[i][j].Occupied == 'Y' && Board.Slot[i][j].Element != 'N')
			{
				//set color
				if(Board.Slot[i][j].Card->Color == 'R')
				{
					for(k = 0; k < 7; k++)
						strcat(Linha[k], colorRed);
				} 
				else 
				{
					for(k = 0; k < 7; k++)
						strcat(Linha[k], colorBlue);
				}
				
				//                    1 234567891123456789212345678931
				sprintf(aux,   		"%i ------------------------------|", 	i * 3 + j + 1);
				strcat(Linha[0], aux);
				sprintf(aux,   		"|            %2i -> %-2i           |", Board.Slot[i][j].Card->UpValue,	
																			Board.Slot[i][j].UpValue);
				strcat(Linha[1], aux);
				strcat(Linha[2], 	"|                               |");
				sprintf(aux,   		"|%-2i->%-2i %-14s[%1c] %2i<-%2i|", 	Board.Slot[i][j].Card->LeftValue,
																			Board.Slot[i][j].LeftValue,
																			Board.Slot[i][j].Card->Name,
																			Board.Slot[i][j].Card->Element,
																			Board.Slot[i][j].RightValue,
																			Board.Slot[i][j].Card->RightValue);
				strcat(Linha[3], aux);
				sprintf(aux,   		"|           Slot Elem:[%1c]       |", 	Board.Slot[i][j].Element);
				strcat(Linha[4], aux);
				sprintf(aux,   		"|            %2i -> %-2i           |", Board.Slot[i][j].Card->DownValue,
																			Board.Slot[i][j].DownValue);
				strcat(Linha[5], aux);
				strcat(Linha[6], 	"|-------------------------------|");
				
				for(k = 0; k < 7; k++)
					strcat(Linha[k], colorReset);
			}
			else if(Board.Slot[i][j].Occupied == 'N' && Board.Slot[i][j].Element != 'N')
			{
				sprintf(aux,   		"%i ------------------------------|", 	i * 3 + j + 1);
				strcat(Linha[0], aux);
				for(k = 1; k < 4; k++)
					strcat(Linha[k], 	"|                               |");
				sprintf(aux,   		"|           Slot Elem:[%1c]       |", 	Board.Slot[i][j].Element);
				strcat(Linha[4], aux);				
				strcat(Linha[5], 	        "|                               |");
				strcat(Linha[6], 			"|-------------------------------|");
			
			}
			aux[0] = '\0';
		}
	
		for(k = 0; k < 7; k++)
		{
			printf("%s \n", Linha[k]);
			Linha[k][0] = '\0';
		}	
	}
}

void SetElementalBoard (Board *board)
{
	printf("\nSetting Elements of the board's slots\n");
	printf("\nElements\n\nN = Neutral\nL = Lightning\nE = Earth\nI = Ice\nW = Wind\nP = Poison\nF = Fire\nA = Water\nH = Holy\n");

	int x, y;
	for(x = 0; x < 3; x++)
		for(y = 0; y < 3; y++)
		{
			while( getchar() != '\n' );
			printf("\nSet the element of the slot %i (%i, %i): ", (3 * x + y+1), x, y);
			scanf("%c", &board->Slot[x][y].Element);
		}

	PrintBoard(*board);
}

char *ReduceChars(char *Text, int Size)
{
	char *ReducedText;
	ReducedText = malloc (sizeof (char) * Size + 1);

	int i, CurrentSize = strlen(Text);
    int Offset = CurrentSize - Size;
	
	if(CurrentSize > Size) 
	{
		*ReducedText = *Text;

		for(i = 1; i < Size; i++)
			*(ReducedText + i) = *(Text + i + Offset);

		*(ReducedText + i) = '\0';
		return ReducedText;
	}
	else
		return Text;
}

void PrintCardHand(Player player)
{
	int k = 0, i = 0;
	char Linha[5][117], aux[20] = "";
	
	for(k = 0; k < 5; k++)
		Linha[k][0] = '\0';

	//set color
	if(player.Color == 'R')
	{
		for(k = 0; k < 5; k++)
			strcat(Linha[k], colorRed);
		
		printf("\n%s Red Player Cards: %s\n", colorRed, colorReset);
	} 
	else 
	{
		for(k = 0; k < 5; k++)
			strcat(Linha[k], colorBlue);

		printf("\n%s Blue Player Cards: %s\n", colorBlue, colorReset);
	}


	for(k = 0; k < 5; k++)
	{
		if(player.CardsAvailable[k] == 'Y')
		{
			//                   1 234567891123456789
			sprintf(aux,   		"%i ----------------| ", 	k + 1);
			strcat(Linha[0], aux);
			sprintf(aux,   		"|        %-2i       | ", 	player.CardsHand[k].UpValue);
			strcat(Linha[1], aux);
			sprintf(aux,   		"|%-2i %-8s[%1c] %2i| ", 	player.CardsHand[k].LeftValue,
															ReduceChars(player.CardsHand[k].Name, 8),
															player.CardsHand[k].Element,
															player.CardsHand[k].RightValue);
			strcat(Linha[2], aux);
			sprintf(aux,   		"|        %-2i       | ", 	player.CardsHand[k].DownValue);
			strcat(Linha[3], aux);
			strcat(Linha[4], 	"|-----------------| ");
		}
		else
		{	//                   1 234567891123456789
			sprintf(aux,   		"%i ----------------| ", 	k + 1);
			strcat(Linha[0], aux);
			for(i = 1; i < 4; i++)
					strcat(Linha[i], 	"|                 | ");
			strcat(Linha[4], 			"|-----------------| ");
		
		}
	}

	for(k = 0; k < 5; k++)
	{
		strcat(Linha[k], colorReset);
		printf("%s \n", Linha[k]);
	}
	
	printf("\n");
}

int CalcScorePlayer0(Game game)
{
	int i, scorePlayer0 = 0;

	for(i = 0; i < 5; i++)
	{
		if(game.Player[0].CardsHand[i].Color == game.Player[0].Color)
			scorePlayer0++;

		if(game.Player[1].CardsHand[i].Color == game.Player[0].Color)
			scorePlayer0++;
	}

	return scorePlayer0;
}

void PrintScore(Game game)
{
	printf("\n%s                                      %4s   %i%s   -%s   %i   %-4s                                      %s",game.Player[0].TxtColor, 
																															game.Player[0].Name, 
																															CalcScorePlayer0(game), 
																															colorReset,
																															game.Player[1].TxtColor, 
																															10-CalcScorePlayer0(game), 
																															game.Player[1].Name,
																															colorReset);
	printf("\n|-------------------------------||--------------- ---------------||-------------------------------|\n");

}

void PrintGame(Game game)
{
	PrintCardHand(game.Player[0]);
	PrintCardHand(game.Player[1]);
	PrintBoard(game.Board);
	PrintScore(game);
}

void ResetGame(Game *game)
{
	int x, y;

	for(x = 0; x < 3; x++)
		for(y = 0; y < 3; y++)
		{
			game->Board.Slot[x][y].Occupied = 'N';
		}

	for(x = 0; x < 5; x++)
	{
		game->Player[0].CardsAvailable[x] = 'Y';
		game->Player[0].CardsHand[x].Color = game->Player[0].Color;
		game->Player[1].CardsAvailable[x] = 'Y';
		game->Player[1].CardsHand[x].Color = game->Player[1].Color;
	}
}

Game initGame(void) 
{
	int x, y;
	Game Game;

	Game.Round = 0;

	for(x = 0; x < 3; x++)
		for(y = 0; y < 3; y++)
		{
			Game.Board.Slot[x][y].Element = 'N';
			Game.Board.Slot[x][y].Occupied = 'N';
			Game.Board.Slot[x][y].Id = 3 * x + y;
		}

	Game.Player[0].Color = 'B';
	Game.Player[0].TxtColor = colorBlue;
	Game.Player[0].Name = "Blue";

	Game.Player[1].Color = 'R';
	Game.Player[1].TxtColor = colorRed;
	Game.Player[1].Name = "Red";

	return Game;
}

AffectedSlot AssignAffectedSlot(Game *game, int slotNo, int xOffset, int yOffset)
{
	int xSlot, ySlot, xCombined, yCombined, 
		lastPlayedClashingValue, affectedSlotClashingValue,
		*lastPlayedFieldValue, *affectedSlotFieldValue,
		*lastPlayedCardValue, *affectedSlotCardValue;
	xSlot = (slotNo-1) / 3;
	ySlot = (slotNo-1) % 3;
	xCombined = xSlot + xOffset;
	yCombined = ySlot + yOffset;
	Slot *lastPlayed = &game->Board.Slot[xSlot][ySlot];
	AffectedSlot affectedSlot;

	if(!(xCombined >= 0 && xCombined <= 2 && yCombined >= 0 && yCombined <= 2))
	{
		affectedSlot.Valid = 'N';

		if(xOffset == 1)
		{
			if(lastPlayed->Card->DownValue == 10)
				affectedSlot.SameWallTrigger = 'Y';
		}
		else if(xOffset == -1)
		{
			if(lastPlayed->Card->UpValue == 10)
				affectedSlot.SameWallTrigger = 'Y';
		}
		else if(yOffset == 1)
		{
			if(lastPlayed->Card->RightValue == 10)
				affectedSlot.SameWallTrigger = 'Y';
		}
		else if(yOffset == -1)
		{
			if(lastPlayed->Card->LeftValue == 10)
				affectedSlot.SameWallTrigger = 'Y';
		}

		return affectedSlot;
	}
	else
	{
		if(game->Board.Slot[xCombined][yCombined].Occupied == 'N')
		{
			affectedSlot.Valid = 'N';
			affectedSlot.SameWallTrigger = 'N';

			return affectedSlot;
		}

		if(xOffset == 1)
		{
			lastPlayedClashingValue = 2;
			affectedSlotClashingValue = 0;
		}
		else if(xOffset == -1)
		{
			lastPlayedClashingValue = 0;
			affectedSlotClashingValue = 2;
		}
		else if(yOffset == 1)
		{
			lastPlayedClashingValue = 1;
			affectedSlotClashingValue = 3;
		}
		else if(yOffset == -1)
		{
			lastPlayedClashingValue = 3;
			affectedSlotClashingValue = 1;
		}

		affectedSlot.Slot = &game->Board.Slot[xCombined][yCombined];
		affectedSlot.Valid = 'Y';

		lastPlayedFieldValue = &lastPlayed->UpValue + lastPlayedClashingValue;
		lastPlayedCardValue = &lastPlayed->Card->UpValue + lastPlayedClashingValue;
		affectedSlotFieldValue = &affectedSlot.Slot->UpValue + affectedSlotClashingValue;
		affectedSlotCardValue = &affectedSlot.Slot->Card->UpValue + affectedSlotClashingValue;

		
		affectedSlot.SubtractionFieldValue = *lastPlayedFieldValue - *affectedSlotFieldValue;
		affectedSlot.SubtractionCardValue = *lastPlayedCardValue - *affectedSlotCardValue;
		affectedSlot.Sum = *lastPlayedCardValue + *affectedSlotCardValue;
		affectedSlot.SameWallTrigger = 'N';
		printf("x=%i, y=%i, aff=%i, lp=%i", xOffset, yOffset, *affectedSlotFieldValue, *lastPlayedFieldValue);

		return affectedSlot;
	}
}

void EvaluateCardPlay (Game *game, int slotNo, char isCombo)
{
	int i, j = 0;
	Slot *currentSlot = &game->Board.Slot[(slotNo-1)/3][(slotNo-1)%3];
	AffectedSlots affectedSlots;
	affectedSlots.AffSlot[0] = AssignAffectedSlot(game, slotNo, -1, 0);
	affectedSlots.AffSlot[1] = AssignAffectedSlot(game, slotNo, 0 , 1);
	affectedSlots.AffSlot[2] = AssignAffectedSlot(game, slotNo, 1 , 0);
	affectedSlots.AffSlot[3] = AssignAffectedSlot(game, slotNo, 0 , -1);
	char lastPlayedColor = currentSlot->Card->Color,
		 comboEvent = 'N';

	for(i = 0; i < 4; i++)
	{
		if(affectedSlots.AffSlot[i].SameWallTrigger == 'Y')
			affectedSlots.SameWallEvent = 'Y';
	}

	if(isCombo == 'N') 
	{
		for(i = 0; i < 4; i++)
		{
			if(game->Rules.Plus == 'Y')
			{
				for(j = i + 1; j < 4; j++)
				{
					if(affectedSlots.AffSlot[i].Valid == 'Y' && affectedSlots.AffSlot[j].Valid == 'Y'
					&&  affectedSlots.AffSlot[i].Sum == affectedSlots.AffSlot[j].Sum)
					{
						if(affectedSlots.AffSlot[i].Slot->Card->Color != lastPlayedColor)
							affectedSlots.AffSlot[i].Turned = 'Y';

						if(affectedSlots.AffSlot[j].Slot->Card->Color != lastPlayedColor)
							affectedSlots.AffSlot[j].Turned = 'Y';

						comboEvent = 'Y';
					}
				}
			}

			if(game->Rules.Same == 'Y')
			{
				for(j = i + 1; j < 4; j++)
				{
					if(affectedSlots.AffSlot[i].Valid == 'Y' && affectedSlots.AffSlot[j].Valid == 'Y'
					&&  affectedSlots.AffSlot[i].SubtractionCardValue == 0
					&&  affectedSlots.AffSlot[j].SubtractionCardValue == 0)
					{
						if(affectedSlots.AffSlot[i].Slot->Card->Color != lastPlayedColor)
							affectedSlots.AffSlot[i].Turned = 'Y';

						if(affectedSlots.AffSlot[j].Slot->Card->Color != lastPlayedColor)
							affectedSlots.AffSlot[j].Turned = 'Y';

						comboEvent = 'Y';
					}
				}
			}

			printf("gmRl samwl: %c smwe %c", game->Rules.SameWall, affectedSlots.SameWallEvent);
			if(game->Rules.SameWall == 'Y' && affectedSlots.SameWallEvent == 'Y')
			{
				printf("smwe i: %i subtract: %i", i, affectedSlots.AffSlot[i].SubtractionCardValue);
				if(affectedSlots.AffSlot[i].Valid == 'Y' && affectedSlots.AffSlot[i].SubtractionCardValue == 0)
				{
					if(affectedSlots.AffSlot[i].Slot->Card->Color != lastPlayedColor)
						affectedSlots.AffSlot[i].Turned = 'Y';

					comboEvent = 'Y';
				}
			}
		}
	}

	for(i = 0; i < 4; i++)
	{
		if(affectedSlots.AffSlot[i].Valid == 'Y' 
		&& affectedSlots.AffSlot[i].SubtractionFieldValue > 0 
		&& affectedSlots.AffSlot[i].Slot->Card->Color != lastPlayedColor)
			   affectedSlots.AffSlot[i].Turned = 'Y';

		if(affectedSlots.AffSlot[i].Turned == 'Y')
			affectedSlots.AffSlot[i].Slot->Card->Color = lastPlayedColor;
	}

	/*if(comboEvent == 'Y')
		for(i = 0; i < 4; i++)
			if(affectedSlots.AffSlot[i].Turned == 'Y')
			{

			}*/
}

void SetCardPlay (Game *game, int cardNo, int slotNo)
{
	Player *player = game->PlayerTurn;
	Slot *currentSlot = &game->Board.Slot[(slotNo-1)/3][(slotNo-1)%3];

	if(player->CardsAvailable[cardNo - 1] != 'Y' || currentSlot->Occupied != 'N')
	{
		printf("\n%sPlease choose an available card and an available slot.\n%s", player->TxtColor, colorReset);
		return;
	}

	currentSlot->Card = &player->CardsHand[cardNo - 1];	
	player->CardsAvailable[cardNo - 1] = 'N';
	currentSlot->Occupied = 'Y';

	if(game->Rules.Elemental == 'N' || currentSlot->Element == 'N')
	{
		currentSlot->UpValue = currentSlot->Card->UpValue;
		currentSlot->RightValue = currentSlot->Card->RightValue;
		currentSlot->DownValue = currentSlot->Card->DownValue;
		currentSlot->LeftValue = currentSlot->Card->LeftValue;
	} 
	else if(currentSlot->Element == currentSlot->Card->Element)
	{
		currentSlot->UpValue = currentSlot->Card->UpValue + 1;
		currentSlot->RightValue = currentSlot->Card->RightValue + 1;
		currentSlot->DownValue = currentSlot->Card->DownValue + 1;
		currentSlot->LeftValue = currentSlot->Card->LeftValue + 1;
	}
	else
	{
		currentSlot->UpValue = currentSlot->Card->UpValue - 1;
		currentSlot->RightValue = currentSlot->Card->RightValue - 1;
		currentSlot->DownValue = currentSlot->Card->DownValue - 1;
		currentSlot->LeftValue = currentSlot->Card->LeftValue - 1;	
	}

	EvaluateCardPlay(game, slotNo, 'N');

	game->Round++;
	
	if(game->PlayerTurn->Color == game->Player[0].Color)
		game->PlayerTurn = &game->Player[1];
	else
		game->PlayerTurn = &game->Player[0];
}

void StartGame(Game *game)
{
	int whoStart, cardNo, slotNo, prevRound;

	printf("\nStarting the Game\n");

	printf("\nWho starts?\n1 - %s\n2 - %s\n", game->Player[0].Name, game->Player[1].Name);
	while( getchar() != '\n' );
	scanf("%i", &whoStart);

	if(whoStart == 1)
		game->PlayerTurn = &game->Player[0];
	else
		game->PlayerTurn = &game->Player[1];

	while(game->Round < 9)
	{
		prevRound = game->Round;
		PrintGame(*game);

		printf("\n%s%s Player Turn. Pick a Card and a slot to play, separated by space: \n%s", game->PlayerTurn->TxtColor, game->PlayerTurn->Name, colorReset);
		while( getchar() != '\n' );
		scanf("%i %i", &cardNo, &slotNo);
		
		SetCardPlay(game, cardNo, slotNo);
	}

	if(prevRound != game->Round)
		PrintGame(*game);
}

void Test(Game *game)
{
	int i;
	long int BlueCards[] = {10469, 69104, 101033, 410210, 85106};
	long int RedCards[] = {26910, 311010, 96102, 10864, 51039};

	game->Rules.Plus = 'Y';
	game->Rules.Same = 'Y';
	game->Rules.SameWall = 'Y';

	LoadCardsAuto(&game->Player[0], BlueCards);
	LoadCardsAuto(&game->Player[1], RedCards);
	int arr[][2] = 
	{
		{2, 8},
		{5, 9},
		{1, 6},
		{1, 5},
		{3, 1},
		{3, 4},
		{4, 2},
		{2, 3},
		{5, 7}
	};

	game->PlayerTurn = &game->Player[0];
	
	for(i = 0; i < 9; i++)
	{
		SetCardPlay(game, arr[i][0], arr[i][1]);
		PrintGame(*game);
	}
	

}

void main(void)
{
	Game Game = initGame();
	int chosenOption;

	printf("\n\nFinal Fantasy 8 Triple Triad\n");
	
	while (chosenOption != 6) 
	{
		printf("\nChoose an option (or enter 0 to list all options): ");
		scanf("%i", &chosenOption);

		switch(chosenOption)
		{
			case 0:
				printf("1 - List all Cards\n2 - Set Rules\n3 - Set Elemental Board\n4 - Set Players\' Cards\n5 - Start Game\n6 - Exit\n7 - Test\n");
				break;

			case 1:
				ShowAllCards();
				break;

			case 2:
				SetRules(&Game.Rules);
				break;

			case 3:
				SetElementalBoard(&Game.Board);
				break;
			
			case 4:
				ShowAllCards();
				LoadCards(&Game.Player[0]);
				LoadCards(&Game.Player[1]);
				PrintCardHand(Game.Player[0]);
				PrintCardHand(Game.Player[1]);
				break;

			case 5:
				StartGame(&Game);
				ResetGame(&Game);
				break;

			case 7:
				Test(&Game);
				break;
		}
	}
}
