#include "../test.h"

int main(void) {
	char str[STRLENMAX], *aux;
	cube_t cube;

        aux = str;
	while (fgets(aux, STRLENMAX, stdin) != NULL)
		while (*aux != '\n')
			aux++;

	cube = readcube("H48", str);

	if (cube_error(cube)) {
		printf("Error reading cube\n");
	} else if (!cube_solvable(cube)) {
		printf("Cube is not solvable\n");
	} else {
		writecube("LST", cube, str);
		printf("%s\n", str);
	}

	return 0;
}
