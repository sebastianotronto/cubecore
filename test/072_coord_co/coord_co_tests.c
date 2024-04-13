#include "../test.h"

int main(void) {
	char str[STRLENMAX];
	cube_t cube;
	int64_t result;

	fgets(str, STRLENMAX, stdin);
	cube = cube_read("H48", str);

	result = cube_coord_co(cube);

	printf("%" PRId64 "\n", result);

	return 0;
}
