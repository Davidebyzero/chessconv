#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tools.h"

bool cantCastle[2][2] = {false}; // [white, black][-1 (O-O-O), +1 (O-O)]
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

static int knightMoves[][2] =
{
	{-2,-1},
	{-2,+1},
	{+2,-1},
	{+2,+1},
	{-1,-2},
	{-1,+2},
	{+1,-2},
	{+1,+2},
};

bool inCheck(Turn player)
{
	char king = player<0 ? 'K' : 'k';
	int rank, file;
	for (rank=0; rank<8; rank++)
	for (file=0; file<8; file++)
		if (board[rank*8+file] == king)
			goto found_king;
	throw "Assert failed: can't find king";
found_king:

	char piece, queen;

	piece = player>0 ? 'P' : 'p';
	if (file>0   && board[(rank+player)*8+(file-1)] == piece)
		return true;
	if (file<8-1 && board[(rank+player)*8+(file+1)] == piece)
		return true;

	piece = player>0 ? 'N' : 'n';
	for (Uint i=0; i<_countof(knightMoves); i++)
	{
		int rank2 = rank + knightMoves[i][0];
		int file2 = file + knightMoves[i][1];
		if (board[rank2*8+file2] == piece)
			return true;
	}

	queen = player>0 ? 'Q' : 'q';

	piece = player>0 ? 'R' : 'r';
	for (int file2=file+1; file2<8; file2++)
	{
		char *pos = board+rank*8+file2;
		if (*pos == piece || *pos == queen)
			return true;
		if (*pos != '-')
			break;
	}
	for (int file2=file-1; file2>=0; file2--)
	{
		char *pos = board+rank*8+file2;
		if (*pos == piece || *pos == queen)
			return true;
		if (*pos != '-')
			break;
	}
	for (int rank2=rank+1; rank2<8; rank2++)
	{
		char *pos = board+rank2*8+file;
		if (*pos == piece || *pos == queen)
			return true;
		if (*pos != '-')
			break;
	}
	for (int rank2=rank-1; rank2>=0; rank2--)
	{
		char *pos = board+rank2*8+file;
		if (*pos == piece || *pos == queen)
			return true;
		if (*pos != '-')
			break;
	}

	piece = player>0 ? 'B' : 'b';
	for (int rank2=rank+1,file2=file+1; rank2<8 && file2<8; rank2++, file2++)
	{
		char *pos = board+rank2*8+file2;
		if (*pos == piece || *pos == queen)
			return true;
		if (*pos != '-')
			break;
	}
	for (int rank2=rank-1,file2=file-1; rank2>=0 && file2>=0; rank2--, file2--)
	{
		char *pos = board+rank2*8+file2;
		if (*pos == piece || *pos == queen)
			return true;
		if (*pos != '-')
			break;
	}
	for (int rank2=rank-1,file2=file+1; rank2>=0 && file2<8; rank2--, file2++)
	{
		char *pos = board+rank2*8+file2;
		if (*pos == piece || *pos == queen)
			return true;
		if (*pos != '-')
			break;
	}
	for (int rank2=rank+1,file2=file-1; rank2<8 && file2>=0; rank2++, file2--)
	{
		char *pos = board+rank2*8+file2;
		if (*pos == piece || *pos == queen)
			return true;
		if (*pos != '-')
			break;
	}

	return false;
}

void parseMove(Turn turn, char *move)
{
	char *s = move;
	int castling = 0;
	char piece = 'P', promote = '\0';
	int fromFile=-1, fromRank=-1;
	int   toFile=-1,   toRank=-1;
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
	case '0':
	case 'O':
		piece = *s++;
		castling = 1;
		if (*s != '-')
			throw "Unexpected character";
		s++;
		if (*s != piece)
			throw "Unexpected character";
		s++;
		if (*s == '-')
		{
			castling = -1;
			s++;
			if (*s != piece)
				throw "Unexpected character";
			s++;
		}
		goto no_more_expected_characters;
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
			(i==0 ? fromFile : toFile) = file;
		}
		if (inrange(*s, '1', '8'))
		{
			int rank = 8-1 - (*s++ - '1');
			(i==0 ? fromRank : toRank) = rank;
		}
	}
	if (toRank<0 && toFile<0)
	{
		toRank = fromRank;
		toFile = fromFile;
		fromRank = -1;
		fromFile = -1;
	}
	if (toFile<0 || toRank<0)
		throw "Error";

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

no_more_expected_characters:
	if (*s != '\0')
		throw "Unexpected character";

	printf("%s: ", turn<0?"White":"Black");
	if (castling)
		printf("Castling(%d)", castling);
	else
	{
		printf("%c from %d,%d to %d,%d%s", piece, fromRank, fromFile, toRank, toFile, capture ? " (capturing)" : "");
		if (promote)
			printf(" - promoted to %c", promote);
		if (check>0)
			printf(" - %scheck", check>1 ? "double ": "");
		else
		if (check<0)
			printf(" - checkmate");
	}
	putchar('\n');
	putchar('\n');

	char *from, *to, c;

	if (castling)
	{
		if (cantCastle[(turn+1)/2][(castling+1)/2])
			throw "Invalid castling";
		cantCastle[(turn+1)/2][0] = true;
		cantCastle[(turn+1)/2][1] = true;
		enPassantFile = -1;
		return;
	}

	switch (piece)
	{
	case 'P':
		if (promote && toRank != (turn+1)*4)
			throw "Invalid promotion";
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
			{
				if (toFile == enPassantFile && toRank == (7+turn*2)/2)
					to[-turn*8] = '-';
				else
					throw "Invalid move";
			}
			else
			if (*to == '-' || ((*to & 0x20)==0x20) != (turn<0))
				throw "Invalid capture";

			enPassantFile = -1;
		}
		else
		{
			if (fromRank>=0)
				throw "Unnecessary information in pawn move";
			fromRank = toRank - turn;

			if (fromFile<0)
				fromFile = toFile;

			from = board+fromRank*8+fromFile;
			to   = board+  toRank*8+  toFile;

			if (*to != '-')
				throw "Invalid move";
			if (*from == '-' && toRank == (7-turn)/2)
			{
				from -= turn*8;
				enPassantFile = toFile;
			}
			else
				enPassantFile = -1;
		}
		c = *from ^ piece;
		if ((c & ~0x20) != 0 || (c==0x20) != (turn>0))
			throw "Invalid move";
		*to = promote ? promote : *from;
		*from = '-';
		break;

	case 'N':
		from = NULL;
		for (Uint i=0; i<_countof(knightMoves); i++)
		{
			int rank = toRank + knightMoves[i][0];
			int file = toFile + knightMoves[i][1];

			if (fromRank>=0 && rank!=fromRank) continue;
			if (fromFile>=0 && file!=fromFile) continue;

			if (inrangex(rank,0,8) && inrangex(file,0,8))
			{
				char *candidateFrom = board+rank*8+file;
				c = *candidateFrom ^ piece;
				if ((c & ~0x20) == 0 && (c==0x20) == (turn>0))
				{
					if (from != NULL)
						throw "Ambiguous knight move";
					from = candidateFrom;
				}
			}
		}
		if (from == NULL)
			throw "Invalid knight move";
		goto standard_move;

	case 'R':
	case 'B':
	case 'Q':
		from = NULL;
		for (int rankDiff=-1; rankDiff<=+1; rankDiff++)
		for (int fileDiff=-1; fileDiff<=+1; fileDiff++)
			if (rankDiff || fileDiff)
			{
				if (piece == 'R' && ( rankDiff &&  fileDiff)) continue;
				if (piece == 'B' && (!rankDiff || !fileDiff)) continue;

				int rank = toRank;
				int file = toFile;
				for (;;)
				{
					rank += rankDiff;
					file += fileDiff;

					if (!inrangex(rank,0,8) || !inrange(file,0,8))
						break;

					if (fromRank>=0 && rank!=fromRank) continue;
					if (fromFile>=0 && file!=fromFile) continue;

					char *candidateFrom = board+rank*8+file;
					c = *candidateFrom ^ piece;
					if ((c & ~0x20) == 0 && (c==0x20) == (turn>0))
					{
						if (from != NULL)
							throw piece=='R' ? "Ambiguous rook move" : piece=='B' ? "Ambiguous bishop move" : "Ambiguous queen move";
						from = candidateFrom;
					}
				}
			}
		if (from == NULL)
			throw piece=='R' ? "Invalid rook move" : piece=='B' ? "Invalid bishop move" : "Invalid queen move";
		goto standard_move;

	case 'K':
		if (fromRank>=0 || fromFile>=0)
			throw "Unnecessary information in king move";
		for (int rankDiff=-1; rankDiff<=+1; rankDiff++)
		for (int fileDiff=-1; fileDiff<=+1; fileDiff++)
			if (rankDiff || fileDiff)
			{
				int rank = toRank + rankDiff;
				int file = toFile + fileDiff;

				if (inrangex(rank,0,8) && inrangex(file,0,8))
				{
					from = board+rank*8+file;
					c = *from ^ piece;
					if ((c & ~0x20) == 0 && (c==0x20) == (turn>0))
					{
						cantCastle[(turn+1)/2][0] = true;
						cantCastle[(turn+1)/2][1] = true;
						goto standard_move;
					}
				}
			}
		throw "Invalid king move";

	standard_move:
		to = board+toRank*8+toFile;

		if ((*to != '-') != capture)
			throw "Notation mismatch (capture)";

		if (capture && ((*to & 0x20)==0x20) != (turn<0))
			throw "Invalid capture";

		*to = *from;
		*from = '-';

		enPassantFile = -1;
		break;
	}

	if (inCheck(turn))
		throw "Illegal move - putting self into check";

	if (inCheck((Turn)-turn) != !!check)
		throw "Notation mismatch (check)";
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
