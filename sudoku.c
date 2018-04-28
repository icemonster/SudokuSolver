#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

/* my sudoku cracker */
int crackBoard(int **board);

void printBoard(int **board){
	for (int i = 0; i < 9; i++){
		for (int j = 0; j < 9; j++)
			printf("%d | ", board[i][j]);
		printf("\n------------------------------------\n");
	}
}

void initBoard(int **board){
	for (int i = 0; i < 9; i++)
		board[i] = (int *) malloc(sizeof(int)*9);
	for (int i = 0; i < 9; i++)
		for(int j = 0; j < 9; j++){
			printBoard(board);
			printf("\nNumber for index (%d,%d): ", i, j);
			scanf("%d",board[i]+j);
		}
}

void freeBoard(int **board){
	for (int i = 0; i < 9; i++)
		free(board[i]);
	free(board);
}

int calcBlock(int l1, int c1){
	int block = 0;
	int rest = l1*9+c1;
	rest = rest % 9;
	if (rest == 0 || rest == 1 || rest == 2)
		block = 1;
	if (rest == 3 || rest == 4 || rest == 5)
		block = 2;
	if (rest == 6 || rest == 7 || rest == 8)
		block = 3;
	rest = l1*9+c1;
	rest = rest / 9;
	if (rest == 3 || rest == 4 || rest == 5)
		block += 3;
	if (rest == 6 || rest == 7 || rest == 8)
		block += 6;
	return block;
}
int sameBlock(int l1, int c1, int l2, int c2){
	return (calcBlock(l1,c1) == calcBlock(l2,c2));
}

int crackTile(int **board, int i, int j){
	/* Check if there is only one possible number in a Tile 
		returns 1 if a number was found. 0 otherwise        */
	int found = 0;
	int numberFound = 0;
	for (int pos = 1; pos < 10; pos++){
		/* check lines */
		int found2 = 1;
		for (int line = 0; line < 9; line++){
			if (board[line][j] == pos)
				found2 = 0;
		}
		/* check columns */
		for (int column = 0; column < 9; column++){
			if (board[i][column] == pos)
				found2 = 0;
		}

		/* check squares */
		for (int line = 0; line < 9; line ++){
			for (int column = 0; column < 9; column ++){
				if (sameBlock(i, j, line, column) && board[line][column] == pos)
					found2 = 0;
			}
		}

		if (found2 && !found){
			found = 1;
			numberFound = pos;
		}
		else if (found2 && found)
			return 0;
	}
	if (found){
		board[i][j] = numberFound;
		return 1;
	}
	return 0;
}

int complete(int **board){
	/* returns 1 if board is complete. 0 otherwise */
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
			if (board[i][j] == 0)
				return 0;
	return 1;
}

int validPosition(int **board, int line, int column, int number){
	/* Check if a number can be in board[line][column] */ 

	/* check if that place is empty */
	if (board[line][column] != 0)
		return 0;

	/* check if there is already that number on the same line */
	for (int c = 0; c < 9; c++)
		if (board[line][c] == number)
			return 0;

	/* check if there is already that number on the same column */
	for (int l = 0; l < 9; l++)
		if (board[l][column] == number)
			return 0;

	/* check if there is already that number on the same square */
	for (int l = 0; l < 9; l++)
		for (int c = 0; c < 9; c++)
			if (sameBlock(l, c, line, column)){
				if (board[l][c] == number)
					return 0;
			}

	/* otherwise, its valid :D */
	return 1;
}

int crackLine(int **board, int line, int number){
	int place = -1;
	for (int column = 0; column < 9; column ++){
		if (validPosition(board, line, column, number)){
			if (place == -1)
				place = column;
			else
				return 0;
		}
	}
	if (place != -1){
		board[line][place] = number;
		return 1;
	}
	return 0;
}

int crackLines(int **board){
	for (int line = 0; line < 9; line++){
		for (int pos = 1; pos < 10; pos ++){
			int found = 0;
			for (int column = 0; column < 9; column++)
				if (board[line][column] == pos)
					found = 1;
			if (!found)
				if (crackLine(board, line, pos))
					return 1;
		}
	}
	return 0;
}

int crackColumn(int **board, int column, int number){
	int place = -1;
	for (int line = 0; line < 9; line ++){
		if (validPosition(board, line, column, number)){
			if (place == -1)
				place = line;
			else
				return 0;
		}
	}
	if (place != -1){
		board[place][column] = number;
		return 1;
	}
	return 0;
}
int crackColumns(int **board){
	for (int column = 0; column < 9; column++){
		for (int pos = 1; pos < 10; pos ++){
			int found = 0;
			for (int line = 0; line < 9; line++)
				if (board[line][column] == pos)
					found = 1;
			if (!found)
				if (crackColumn(board, column, pos))
					return 1;
		}
	}
	return 0;
}

int crackSquare(int **board, int block, int number){
	int placeLine = -1;
	int placeColumn = -1;
	for (int line = 0; line < 9; line ++){
		for (int column = 0; column < 9; column++){
			if (block == calcBlock(line,column) && validPosition(board, line, column, number)){
				if (placeLine == -1){
					placeLine = line;
					placeColumn = column;
				}
				else
					return 0;
			}
		}
	}
	if (placeLine != -1){
		board[placeLine][placeColumn] = number;
		return 1;
	}
	return 0;
}

int crackSquares(int **board){
	for (int block = 1; block < 10; block ++){
		for (int pos = 1; pos < 10; pos ++){
			int found = 0;
			for (int l = 0; l < 9; l++){
				for (int c = 0; c < 9; c++){
					if (block == calcBlock(l,c) && board[l][c] == pos)
						found = 1;
				}
			}
			if (!found)
				if (crackSquare(board, block, pos))
					return 1;
		}
	}
	return 0;
}

void copyBoard(int **boardTarget, int **boardSource){
	for (int l = 0; l < 9; l++)
		for (int c = 0; c < 9; c++)
			boardTarget[l][c] = boardSource[l][c];
}

int bruteForceTile(int **board, int line, int column){
	int **board2 = (int **) malloc(sizeof(int *)*9);

	for (int i = 0; i < 9; i++)
		board2[i] = (int *) malloc(sizeof(int)*9);

	for (int pos = 1; pos < 10; pos ++){
		if (validPosition(board, line, column, pos)){
			copyBoard(board2, board); /* copy board to board2 */
			board2[line][column] = pos; /* try this number */
			printf("Maybe putting %d at (%d,%d)\n", pos, line, column);
			if (!crackBoard(board2)){
				copyBoard(board, board2); /* copy completed board2 to board */
				return 0;
			}
		}
	}
	return 1;
}

int bruteForce(int **board){
	/* Try proof by refutation.
		Returns 0 if successful
		Returns 1 if it is impossible */
	for (int line = 0; line < 9; line ++){
		for (int column = 0; column < 9; column++){
			/* dumb way to find a place to brute force. smartest thing to do would be find the tile with less possibilies i guess */
			if (board[line][column] == 0)
				return bruteForceTile(board, line, column);
		}
	}
	return 1; /* shouldnt happen XD but oke */
}


int crackBoard(int **board){
	/* Returns 0 if successful 
	   Returns 1 if it is impossible or couldnt crack it
	*/
	int didSomething;
	while (!complete(board)){
		didSomething = 0;
		for (int i = 0; i < 9; i++){
			for (int j = 0; j < 9; j++){
				if (!board[i][j]) /* if there is a zero */
					if(crackTile(board, i, j))
						didSomething = 1;
			}
		}
		if (crackLines(board) || crackColumns(board) || crackSquares(board))
			didSomething = 1;
		if (!didSomething && bruteForce(board)) /* If those logical steps wont work, try brute-force */
			return 1;
	}
	return 0;
}

void loadPuzzle(int **board, const char *filename){
	char buffer[90];
	int fd = open(filename, O_RDONLY);
	int line = 0, column = 0;

	for (int i = 0; i < 9; i++)
		board[i] = (int *) malloc(sizeof(int)*9);

	read(fd, buffer, 90);
	buffer[90] = '\0';
	printf("%s\n", buffer);

	for (int i = 0; i < 90; i++){
		if (buffer[i] >= '0' && buffer[i] <= '9'){
			board[column][line] = buffer[i]-'0'; /* was lazy with the names */
			line ++;
		}
		if (line == 9){
			column++;
			line = 0;
		}
	}

}

int main(int argc, char **argv){
	int **board = (int **) malloc(sizeof(int *)*9);

	/*
	if (argc == 1){
		printf("Please provide the name of the file where the puzzle to be solved is\n");
		return 1;
	}*/

	/*initBoard(board);*/

	char filename[100];

	printf("Filename: ");
	scanf("%s", filename);
	printf("\nChosen %s for filename\n", filename);
	loadPuzzle(board, filename);
	/* loadPuzzle(board, argv[1]); */
	printf("\nCracking this board...\n");
	printBoard(board);
	printf("Cracking result: %d\n", crackBoard(board));
	printBoard(board);
	printf("Done\n");
	freeBoard(board);

	return 0;
}