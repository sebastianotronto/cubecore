#include "../test.h"

cube_t applymoves(cube_t, char *);

int main(void) {
	char movestr[STRLENMAX], cubestr[STRLENMAX];
	int i, n;
	move_t moves[STRLENMAX];
	cube_t cube;

	fgets(movestr, STRLENMAX, stdin);
	fgets(cubestr, STRLENMAX, stdin);
	cube = cube_read("H48", cubestr);

	n = cube_readmoves(movestr, moves);

	if (n == -1)
		printf("Error reading moves!\n");

	for (i = 0; i < n; i++)
		cube = cube_move(cube, moves[i]);

	if (cube_error(cube)) {
		printf("Error moving cube\n");
	} else if (!cube_solvable(cube)) {
		printf("Moved cube is not solvable\n");
	} else {
		cube_write("H48", cube, cubestr);
		printf("%s\n", cubestr);
	}

	return 0;
}
