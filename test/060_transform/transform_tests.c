#include "../test.h"

cube_t applytrans(cube_t, char *);

int main(void) {
	char cubestr[STRLENMAX], transtr[STRLENMAX];
	cube_t cube;
	trans_t t;

	fgets(transtr, STRLENMAX, stdin);
	fgets(cubestr, STRLENMAX, stdin);
	cube = cube_read("H48", cubestr);

	t = cube_readtrans(transtr);
	cube = cube_transform(cube, t);

	if (cube_error(cube)) {
		printf("Error transforming cube\n");
	} else if (!cube_solvable(cube)) {
		printf("Transformed cube is not solvable\n");
	} else {
		cube_write("H48", cube, cubestr);
		printf("%s\n", cubestr);
	}

	return 0;
}
