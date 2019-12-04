#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tools.h"

bool cantCastle[2] = {false, false}; // white, black
int enPassantFile = -1; // 0..7 = file of opponent's pawn open for en passant, -1 = no pawns open for en passant
char board[64+1] =
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

char moves[][strlength("Ra1xh7 Ra1xh7")+1] =
{
	"e4 d5",
	"exd5",
};

void printBoard()
{
	for (Uint rank=0,i=0; rank<8; rank++)
	{
		for (Uint file=0; file<8; file++,i++)
			putchar(board[i]);
		putchar('\n');
	}
	putchar('\n');
}

enum Turn
{
	Turn_White = -1,
	Turn_Black = +1,
};

void parseMove(Turn turn, char *move)
{
	char *s = move;
	char piece = 'P', promote = '\0';
	int   toFile=-1,   toRank=-1;
	int fromFile=-1, fromRank=-1;
	bool capture = false;
	int check = 0; // -1=mate, 1=check, 2=double check

	switch (*s)
	{
	case 'N':
	case 'B':
	case 'R':
	case 'Q':
	case 'K':
		piece = *s++;
		break;
	default:
		break;
	}

	for (Uint i=0; i<2; i++)
	{
		if (i!=0 && (*s=='x' || *s==':'))
		{
			s++;
			capture = true;
		}
		if (inrange(*s, 'a', 'h'))
		{
			int file = *s++ - 'a';
			if (toFile < 0)
				toFile = file;
			else
			{
				fromFile = toFile;
				fromRank = toRank;
				toFile = file;
				toRank = -1;
			}
		}
		if (inrange(*s, '1', '8'))
		{
			int rank = 8-1 - (*s++ - '1');
			if (toRank < 0)
			{
				if (toFile < 0)
					throw "Error";
				toRank = rank;
			}
			else
				throw "Error";
		}
	}

	if (piece == 'P')
	{
		if (*s == '=')
			s++;
		switch (*s)
		{
		case 'N':
		case 'B':
		case 'R':
		case 'Q':
			promote = *s++;
			break;
		default:
			break;
		}
	}

	if (*s == '+')
	{
		s++;
		check = 1;
		if (*s == '+')
		{
			s++;
			check = 2;
		}
	}
	else
	if (*s == '#')
	{
		s++;
		check = -1;
	}

	if (*s != '\0')
		throw "Unexpected character";

	if (toFile<0 || toRank<0)
		throw "Error";

	printf("%s: %c from %d,%d to %d,%d%s", turn<0?"White":"Black", piece, fromRank, fromFile, toRank, toFile, capture ? " (capturing)" : "");
	if (promote)
		printf(" - promoted to %c", promote);
	if (check>0)
		printf(" - %scheck", check>1 ? "double ": "");
	else
	if (check<0)
		printf(" - checkmate");
	putchar('\n');
	putchar('\n');

	char *from, *to;

	switch (piece)
	{
	case 'P':
		if (capture)
		{
			if (fromFile<0)
				throw "Missing data";
			if (fromRank<0)
				fromRank = toRank - turn;
			else
			if (fromRank != toRank - turn)
				throw "Invalid move";

			if (abs(fromFile - toFile) != 1 || !inrangex(fromRank,0,8))
				throw "Invalid move";

			from = board+fromRank*8+fromFile;
			to   = board+  toRank*8+  toFile;

			if (*to == '-')
				throw "Invalid move";
		}
		else
		{
			if (fromRank<0)
				fromRank = toRank - turn;
			else
			if (fromRank != toRank - turn)
				throw "Invalid move";

			if (fromFile<0)
				fromFile = toFile;

			from = board+fromRank*8+fromFile;
			to   = board+  toRank*8+  toFile;

			if (*to != '-')
				throw "Invalid move";
			if (*from == '-' && toRank == (7-turn)/2)
				from -= turn*8;
		}
		char c = *from ^ piece;
		if ((c & ~0x20) != 0 || (c==0x20) != (turn>0))
			throw "Invalid move";
		*to = *from;
		*from = '-';
		break;
	}
}

int main(int argc, char *argv[])
{
	printBoard();
	for (Uint i=0; i<_countof(moves); i++)
	{
		char *blackMove = strchr(moves[i], ' ');
		if (blackMove)
		{
			*blackMove = '\0';
			blackMove++;
		}
		try
		{
			parseMove(Turn_White, moves[i]);
		}
		catch (char *message)
		{
			printf("%s parsing White's move in turn #%u\n", message, i+1);
			return -1;
		}
		printBoard();
		if (blackMove)
		{
			try
			{
				parseMove(Turn_Black, blackMove);
			}
			catch (char *message)
			{
				printf("%s parsing Black's move in turn #%u\n", message, i+1);
				return -1;
			}
			printBoard();
		}
		else
		{
			if (i+1 != _countof(moves))
			{
				printf("Error: Black's move missing in turn #%u\n", i+1);
				return -1;
			}
		}
	}
	return 0;
}
