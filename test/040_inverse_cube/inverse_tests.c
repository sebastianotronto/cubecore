#include "../test.h"

int main(void) {
	char str[STRLENMAX];
	cube_t cube, inv;

	fgets(str, STRLENMAX, stdin);
	cube = readcube("H48", str);
	inv = cube_inverse(cube);

	if (cube_error(inv)) {
		printf("Error inverting cube\n");
	} else if (!cube_solvable(inv)) {
		printf("Inverted cube is not solvable\n");
	} else {
		writecube("H48", inv, str);
		printf("%s\n", str);
	}

	return 0;
}
