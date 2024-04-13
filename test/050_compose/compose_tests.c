#include "../test.h"

int main(void) {
	char str[STRLENMAX];
	cube_t c1, c2, c3;

	fgets(str, STRLENMAX, stdin);
	c1 = cube_read("H48", str);
	fgets(str, STRLENMAX, stdin);
	c2 = cube_read("H48", str);

	c3 = cube_compose(c1, c2);

	if (cube_error(c3)) {
		printf("Error composing cubes\n");
	} else if (!cube_solvable(c3)) {
		printf("Composed cube is not solvable\n");
	} else {
		cube_write("H48", c3, str);
		printf("%s\n", str);
	}

	return 0;
}
