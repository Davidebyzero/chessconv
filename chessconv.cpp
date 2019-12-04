#include <stdio.h>
#include "tools.h"

bool cantCastle[2] = {false, false}; // white, black
int enPassantFile = -1; // 0..7 = file of opponent's pawn open for en passant, -1 = no pawns open for en passant
Uchar turn = 0; // 0 = white's turn, 1 = black's turn
char state[64+1] =
{
	"rnbqkbnr"
	"pppppppp"
	"--------"
	"--------"
	"--------"
	"--------"
	"PPPPPPPP"
	"RNBQKBNR"
};

void printBoard()
{
	for (Uint rank=0,i=0; rank<8; rank++)
	{
		for (Uint file=0; file<8; file++,i++)
			putchar(state[i]);
		putchar('\n');
	}
	putchar('\n');
}

int main(int argc, char *argv[])
{
	printBoard();
	return 0;
}
